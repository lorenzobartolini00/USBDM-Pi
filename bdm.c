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

// Tx data array
static uint8_t tx_data[MAX_BDM_COMMAND_SIZE];

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
uint bdm_command_exec(uint data, uint8_t tx_bit_count, uint8_t rx_bit_count)
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

uint make_data(uint8_t bdm_command, uint8_t byte_count)
{
    uint data = 0;

    for (int i=0;i<(byte_count-1);i++)
    {
        data |= tx_data[i];
        data = (data<<8);
    }

    data |= bdm_command;

    return data;
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

uint8_t bdm_cmd_read_status(void)
{
    // Transmit 1 byte, containing bdm command. 
    // Receive 1 byte, containing status
    uint8_t tx_bit_count = BYTE;
    uint8_t rx_bit_count = BYTE;

    uint8_t status = (uint8_t)bdm_command_exec(READ_STATUS, tx_bit_count, rx_bit_count);

    return status;
}