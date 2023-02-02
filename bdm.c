#include "bdm.h"

#include "hardware/pio.h"
#include "pico/stdlib.h"

#include "pio_functions.h"
#include "config.h"

// Select state machine
static bool is_sm_init = false;
// BDM data program has been saved into PIO instruction memory?
static bool is_bdm_data_init = false;
// The target frequency has been ackowledged?
static bool is_freq_known = false; 
// Frequency of the PIO
static float pio_freq = PIO_FREQ;
// 16-bit Sync value in 1MHz ticks
static uint16_t ticks = 0;  
// Offset of the PIO instruction memory
static uint pio_offset = 0;
// Selected PIO
static PIO pio = pio0;
// Claimed PIO state machine
// Get first free state machine in PIO 0
static uint sm;

// Data buffer
//! @note : Format
//!         - [0]    = # of bytes to tx (up to 4)
//!         - [1]    = # of bytes to rx (up to 2)
//!         - [2]    = BDM command 
//!         - [3]    = 1st byte parameter (opt)
//!         - [4]    = 2nd byte parameter (opt)
static uint8_t data_buffer[MAX_BDM_COMMAND_SIZE];

// Build word data from data_buffer
uint _make_data()
{
    uint data = 0;
    uint8_t byte_count = data_buffer[TX_BYTE_COUNT];

    for (int i=COMMAND; i<byte_count+COMMAND; i++)
    {
        data = data_buffer[i] | (data<<BYTE);
    }

    return data;
}

//! Execute BDM command
//!
//! @param
//!    data: (max 32 bit) -> BDM COMMAND CODE | PARAMETERS (OPT) 
//! @param
//!    tx_bit_count: number of bit to transmit (command code included)
//! @param
//!    rx_bit_count: number of bit to receive (can be zero)
//!
//! @return
//!     Received data
//!
uint bdm_command_exec(void)
{
    if(!is_sm_init)
    {
        // Get first free state machine in PIO 0
        uint sm = pio_claim_unused_sm(pio, true);

        is_sm_init = true;
    }

    // Check if frequency is known
    if (!is_freq_known)
    {
        bdm_cmd_sync();
    }

    // Check if the program is in the pio memory
    if(!is_bdm_data_init)
    {
        // Overwrite pio instruction memory with "bdm-data.pio" program and return offset
        pio_offset = bdm_init(pio, sm, pio_freq);

        is_bdm_data_init = true;
    }

    // Make data based on bytes in data_buffer
    uint data = _make_data();
    uint8_t tx_bit_count = data_buffer[TX_BYTE_COUNT]*BYTE;
    uint8_t rx_bit_count = data_buffer[RX_BYTE_COUNT]*BYTE;

    do_bdm_command(pio, sm, data, tx_bit_count, rx_bit_count, pio_offset);

    // Wait the end of the operation
    wait_end_operation(pio, sm);

    uint received_data = 0;
    // Read data from rx fifo
    if(!pio_sm_is_rx_fifo_empty(pio, sm))
    {
        received_data = pio_sm_get(pio, sm);
    }

    return received_data;
}

//=====================================================================================
// BDM commands
//=====================================================================================
void bdm_cmd_sync(void)
{
    if(!is_sm_init)
    {
        // Get first free state machine in PIO 0
        uint sm = pio_claim_unused_sm(pio, true);

        is_sm_init = true;
    }

    // Overwrite pio instruction memory with "bdm-sync.pio" program, execute it and return ticks
    ticks = (uint16_t)sync(pio, sm, SYNC_FREQ);

    // 1 "tick" corresponds to 2 pio instruction cycles, since state machine increment "tick" every 2 instruction cycles
    // T_measured = ticks * T_tick, where T_tick = 2 * T_pio = 2 * (1/F_pio) = 2 * (1/2MHZ) = 1us.
    // T_measured corresponds to 128 MCU clock cycles, so T_MCU = T_measured / 128
    // Finally F_MCU = 1/T_MCU = (1/T_measured) * 128 
    float T_measured_us = (float)(ticks * 2 * SYNC_PERIOD);
    // F_MCU is in HZ
    float F_MCU = (1/T_measured_us) * 128 * MHZ;

    pio_freq = F_MCU;

    // Pio memory has been ovewritten, so it needs to be re-initialized
    is_bdm_data_init = false;
    is_freq_known = true;
}

// Return 16-bit Sync value in 60MHz ticks
uint16_t bdm_cmd_get_sync_length(void)
{
    uint16_t sync_length = (uint16_t)60*ticks;
}

//! Read status register
//! @return
//!     data_buffer: 
//!     [3]   = status
//!
void bdm_cmd_read_status(uint8_t *command_buffer)
{
    data_buffer[TX_BYTE_COUNT] = 1; // 1 byte to transmit
    data_buffer[RX_BYTE_COUNT] = 1; // 1 byte to receive
    data_buffer[COMMAND] = READ_STATUS;

    uint8_t status = (uint8_t)bdm_command_exec();

    // Save received data in command_buffer[4]
    command_buffer[4] = status;

    return;
}

// Start executing user program
void bdm_cmd_go(void)
{
    data_buffer[TX_BYTE_COUNT] = 1; // 1 byte to transmit
    data_buffer[RX_BYTE_COUNT] = 0; // 0 byte to receive
    data_buffer[COMMAND] = GO;

    bdm_command_exec();

    is_freq_known = false;

    return;
}

// Set target in active background mode
void bdm_cmd_halt(void)
{
    data_buffer[TX_BYTE_COUNT] = 1; // 1 byte to transmit
    data_buffer[RX_BYTE_COUNT] = 0; // 0 byte to receive
    data_buffer[COMMAND] = BACKGROUND;

    bdm_command_exec();

    return;
}

// Read register A
void bdm_cmd_read_a(uint8_t *command_buffer)
{
    data_buffer[TX_BYTE_COUNT] = 1; // Transmit 1 bytes
    data_buffer[RX_BYTE_COUNT] = 1;
    data_buffer[COMMAND] = READ_A;

    uint8_t reg_a = (uint8_t)bdm_command_exec();

    // 8 bit value
    command_buffer[3] = 0;
    command_buffer[4] = reg_a;

    return;
}

// Read register CCR
void bdm_cmd_read_ccr(uint8_t *command_buffer)
{
    data_buffer[TX_BYTE_COUNT] = 1; // Transmit 1 bytes
    data_buffer[RX_BYTE_COUNT] = 1;
    data_buffer[COMMAND] = READ_CCR;

    uint8_t reg_ccr = (uint8_t)bdm_command_exec();

    // 8 bit value
    command_buffer[3] = 0;
    command_buffer[4] = reg_ccr;

    return;
}

// Read register PC
void bdm_cmd_read_pc(uint8_t *command_buffer)
{
    data_buffer[TX_BYTE_COUNT] = 1; // Transmit 1 bytes
    data_buffer[RX_BYTE_COUNT] = 2;
    data_buffer[COMMAND] = READ_PC;

    uint16_t reg_ccr = (uint16_t)bdm_command_exec();

    // 16 bit value
    command_buffer[3] = (uint8_t)(reg_ccr>>8);
    command_buffer[4] = (uint8_t)(reg_ccr&0xFF);

    return;
}

// Read register HX
void bdm_cmd_read_hx(uint8_t *command_buffer)
{
    data_buffer[TX_BYTE_COUNT] = 1; // Transmit 1 bytes
    data_buffer[RX_BYTE_COUNT] = 2;
    data_buffer[COMMAND] = READ_HX;

    uint16_t reg_hx = (uint16_t)bdm_command_exec();

    // 16 bit value
    command_buffer[3] = (uint8_t)(reg_hx>>8);
    command_buffer[4] = (uint8_t)(reg_hx&0xFF);

    return;
}

// Read register SP
void bdm_cmd_read_sp(uint8_t *command_buffer)
{
    data_buffer[TX_BYTE_COUNT] = 1; // Transmit 1 bytes
    data_buffer[RX_BYTE_COUNT] = 2;
    data_buffer[COMMAND] = READ_SP;

    uint16_t reg_sp = (uint16_t)bdm_command_exec();

    // 16 bit value
    command_buffer[3] = (uint8_t)(reg_sp>>8);
    command_buffer[4] = (uint8_t)(reg_sp&0xFF);

    return;
}

// Write register A
void bdm_cmd_write_a(uint8_t *command_buffer)
{
    data_buffer[TX_BYTE_COUNT] = 2; // Transmit 2 bytes
    data_buffer[RX_BYTE_COUNT] = 0;
    data_buffer[COMMAND] = WRITE_A;
    data_buffer[FIRST_PARAMETER] = command_buffer[7];   // 8 bit register

    bdm_command_exec();

    return;
}

// Write register CCR
void bdm_cmd_write_ccr(uint8_t *command_buffer)
{
    data_buffer[TX_BYTE_COUNT] = 2; // Transmit 2 bytes
    data_buffer[RX_BYTE_COUNT] = 0;
    data_buffer[COMMAND] = WRITE_CCR;
    data_buffer[FIRST_PARAMETER] = command_buffer[7];   // 8 bit register

    bdm_command_exec();

    return;
}

// Write register PC
void bdm_cmd_write_pc(uint8_t *command_buffer)
{
    data_buffer[TX_BYTE_COUNT] = 3; // Transmit 3 bytes
    data_buffer[RX_BYTE_COUNT] = 0;
    data_buffer[COMMAND] = WRITE_PC;
    data_buffer[FIRST_PARAMETER] = command_buffer[6];
    data_buffer[SECOND_PARAMETER] = command_buffer[7];  // 16 bit register

    bdm_command_exec();

    return;
}

// Write register HX
void bdm_cmd_write_hx(uint8_t *command_buffer)
{
    data_buffer[TX_BYTE_COUNT] = 3; // Transmit 3 bytes
    data_buffer[RX_BYTE_COUNT] = 0;
    data_buffer[COMMAND] = WRITE_HX;
    data_buffer[FIRST_PARAMETER] = command_buffer[6];
    data_buffer[SECOND_PARAMETER] = command_buffer[7];  // 16 bit register

    bdm_command_exec();

    return;
}

// Write register SP
void bdm_cmd_write_sp(uint8_t *command_buffer)
{
    data_buffer[TX_BYTE_COUNT] = 3; // Transmit 3 bytes
    data_buffer[RX_BYTE_COUNT] = 0;
    data_buffer[COMMAND] = WRITE_SP;
    data_buffer[FIRST_PARAMETER] = command_buffer[6];
    data_buffer[SECOND_PARAMETER] = command_buffer[7];  // 16 bit register

    bdm_command_exec();

    return;
}


// Write an 8 bit data word to 16 bit register
void bdm_cmd_write_byte(uint8_t addr_h, uint8_t addr_l, uint8_t data)
{
    data_buffer[TX_BYTE_COUNT] = 4; // Transmit 4 bytes
    data_buffer[RX_BYTE_COUNT] = 0;
    data_buffer[COMMAND] = WRITE_BYTE;
    data_buffer[FIRST_PARAMETER] = addr_h;   // Address H
    data_buffer[SECOND_PARAMETER] = addr_l; // Address L
    data_buffer[THIRD_PARAMETER] = data;

    bdm_command_exec();

    return;
}

// Write an 8 bit data to the next memory location (in relation to the last location written)
void bdm_cmd_write_next(uint8_t data)
{
    data_buffer[TX_BYTE_COUNT] = 2;
    data_buffer[RX_BYTE_COUNT] = 0;
    data_buffer[COMMAND] = WRITE_NEXT;
    data_buffer[FIRST_PARAMETER] = data;

    bdm_command_exec();

    return;
}

// Read an 8 bit data word to 16 bit register
void bdm_cmd_read_byte(uint8_t addr_h, uint8_t addr_l, uint8_t* data_ptr)
{
    data_buffer[TX_BYTE_COUNT] = 3; 
    data_buffer[RX_BYTE_COUNT] = 1;
    data_buffer[COMMAND] = READ_BYTE;
    data_buffer[FIRST_PARAMETER] = addr_h;   // Address H
    data_buffer[SECOND_PARAMETER] = addr_l; // Address L

    uint8_t read_byte = (uint8_t)bdm_command_exec();

    // Save read value into the buffer
    data_ptr[0] = read_byte;

    return;
}

// Read an 8 bit data from the next memory location (in relation to the last location read)
void bdm_cmd_read_next(uint8_t* data_ptr)
{
    data_buffer[TX_BYTE_COUNT] = 1; 
    data_buffer[RX_BYTE_COUNT] = 1; 
    data_buffer[COMMAND] = READ_NEXT;

    uint8_t read_byte = (uint8_t)bdm_command_exec();

    // Save read value into the buffer
    data_ptr[0] = read_byte;

    return;
}