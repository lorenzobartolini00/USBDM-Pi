#include <stdio.h>
#include "pico/stdlib.h"
#include <stdlib.h>
#include <string.h>

#include "hardware/pll.h"
#include "hardware/clocks.h"
#include "hardware/structs/pll.h"
#include "hardware/structs/clocks.h"
#include "hardware/pio.h"

#include "config.h"
#include "commands.h"

// Define the time(in ms) to wait for a CDC connection to be established.
// This prevent initial program output being lost, at cost of requiring an active CDC connection
#ifndef PICO_STDIO_USB_CONNECT_WAIT_TIMEOUT_MS
#define PICO_STDIO_USB_CONNECT_WAIT_TIMEOUT_MS 15000
#endif


// Print every clk sources frequency
void measure_freqs(void);

// Get the division value for pio_freq calculation
float get_pio_clk_div(float desired_freq);

// Init I/O
void start_usb_connection(void);

// Put one word in tx fifo
void put_tx_fifo(PIO pio, uint sm, uint data, uint bit, bool shift_right);

// Put data in TX FIFO
void fill_tx_fifo(PIO pio, uint sm, uint *data, uint length, uint bit, bool shift_right);

// Wait until some data are received on rx fifo
void wait_end_operation(PIO pio, uint sm);

// Stop running sm, clear instruction memory, clear fifos and add new program. Return offset
uint pio_program_init(PIO pio, uint sm, const struct pio_program *pio_prog);

// Replace an instruction at offset pio instruction memory location
void pio_add_instr(PIO pio, uint instr, uint offset);

// Set pull threshold
void pio_set_pull_threshold(PIO pio, uint sm, uint pull_threshold);

// Init bdm by setting up bdm-data.pio program in pio instruction memory
uint bdm_init(PIO pio, uint sm, float pio_freq);

// Transmit a bdm command and eventual data
void do_bdm_command(PIO pio, uint sm, uint data, uint tx_bit, uint rx_bit, uint offset);

// Do the SYNC command
float sync(PIO pio, uint sm, float pio_freq);