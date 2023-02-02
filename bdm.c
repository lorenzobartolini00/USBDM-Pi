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

    // Overwrite pio instruction memory with "bdm-sync.pio" program, execute it and return measured frequency
    pio_freq = sync(pio, sm, SYNC_FREQ);

    // Pio memory has been ovewritten, so it needs to be re-initialized
    is_bdm_data_init = false;
    is_freq_known = true;
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

// Set target in active background mode
void bdm_cmd_halt(void)
{
    data_buffer[TX_BYTE_COUNT] = 1; // 1 byte to transmit
    data_buffer[RX_BYTE_COUNT] = 0; // 1 byte to receive
    data_buffer[COMMAND] = BACKGROUND;

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