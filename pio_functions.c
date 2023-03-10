#include "pio_functions.h"
#include "bdm-data.pio.h"
#include "bdm-sync.pio.h"
#include "hardware/claim.h"

#include "config.h"
#include "commands.h"

// Utils------------------------------------------------------
void measure_freqs(void) {
    uint f_pll_sys = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_PLL_SYS_CLKSRC_PRIMARY);
    uint f_pll_usb = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_PLL_USB_CLKSRC_PRIMARY);
    uint f_rosc = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_ROSC_CLKSRC);
    uint f_clk_sys = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_SYS);
    uint f_clk_peri = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_PERI);
    uint f_clk_usb = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_USB);
    uint f_clk_adc = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_ADC);
    uint f_clk_rtc = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_RTC);

    printf("pll_sys  = %dkHz\n", f_pll_sys);
    printf("pll_usb  = %dkHz\n", f_pll_usb);
    printf("rosc     = %dkHz\n", f_rosc);
    printf("clk_sys  = %dkHz\n", f_clk_sys);
    printf("clk_peri = %dkHz\n", f_clk_peri);
    printf("clk_usb  = %dkHz\n", f_clk_usb);
    printf("clk_adc  = %dkHz\n", f_clk_adc);
    printf("clk_rtc  = %dkHz\n", f_clk_rtc);

    // Can't measure clk_ref / xosc as it is the ref
}


float get_pio_clk_div(float desired_pio_freq)
{
    return (float)clock_get_hz(clk_sys) / desired_pio_freq;
}

void start_usb_connection(void)
{
    // Initialise I/O
    stdio_init_all();

    // Explicitly initialize USB stdio and add it to the current set of stdin drivers.
    // Wait for a CDC connection from the host before returning
    if(stdio_usb_init())
    {
        sleep_ms(5000);
        printf("CDC connection enstablished!\n");
    }
}


void fill_tx_fifo(PIO pio, uint sm, uint *data, uint length, uint bit, bool shift_right)
{
    // Put data in TX FIFO. 
    for(int i = 0; i< length; i++)
    {
        put_tx_fifo(pio, sm, data[i], bit, shift_right);
    }
}


void put_tx_fifo(PIO pio, uint sm, uint data, uint bit, bool shift_right)
{
    // If shift_right is disabled, align data to the left.
    if(!shift_right)
    {
        uint shift = REG_WIDTH - bit;
        data = data << shift;
    }

    pio_sm_put_blocking(pio, sm, data);
}


void wait_end_operation(PIO pio, uint sm)
{
    // Wait for an operation to complete. 
    // When any operation ends, some data are transferred to rx fifo
    while(pio_sm_is_rx_fifo_empty(pio, sm));
}

uint pio_program_init(PIO pio, uint sm, const struct pio_program *pio_prog)
{
    // Stop running PIO program in the state machine
    pio_sm_set_enabled(pio, sm, false);

    // Clear instruction memory
    pio_clear_instruction_memory(pio);

    // Add PIO program to PIO instruction memory, SDK will find location and
    // return with the memory offset of the program.
    uint offset = pio_add_program(pio, pio_prog);

    // Clear eventual data in rx and tx fifos
    pio_sm_clear_fifos(pio, sm);

    return offset;
}

void pio_add_instr(PIO pio, uint instr, uint offset)
{
    uint32_t save = hw_claim_lock();
    pio->instr_mem[offset] = instr;
    hw_claim_unlock(save);
}


void pio_set_pull_threshold(PIO pio, uint sm, uint pull_threshold)
{
    pio->sm[sm].shiftctrl = (pio->sm[sm].shiftctrl & ~(0x3e000000)) |
                            ((pull_threshold & 0x1fu) << PIO_SM0_SHIFTCTRL_PULL_THRESH_LSB);
}

// Actual commands---------------------------------------------------------------

// Set the line low for 5 seconds, so the MCU can enter active background mode
void bdm_connect(void)
{
    // Keep the line low at the rising edge of MCU reset
    gpio_init(DATA_PIN);
    gpio_set_dir(DATA_PIN, true);
    gpio_put(DATA_PIN, false);

    // Sleep for 5 seconds, while MCU enters Active Background mode
    sleep_ms(5000);

    gpio_put(DATA_PIN, true);
    gpio_set_dir(DATA_PIN, false);

    // Set GPIO to be pulled up
    gpio_pull_up(DATA_PIN);

    // Set GPIO function to PIO_0
    gpio_set_function(DATA_PIN, GPIO_FUNC_PIO0 );
}

// Init bdm
uint bdm_init(PIO pio, uint sm, float pio_freq)
{
    // Clear memory and fifos and add program
    uint offset = pio_program_init(pio, sm, &bdm_data_program);

    // Calculate the PIO clock divider 
    float div = get_pio_clk_div(pio_freq);

    // Initialize the program using the helper function in our .pio file
    bdm_data_program_init(pio, sm, offset, DATA_PIN, div, SHIFT_RIGHT, AUTO_PULL, AUTO_PUSH, 32, 32);

    pio_sm_set_enabled(pio, sm, true);

    return offset;
}


// Data command
void do_bdm_command(PIO pio, uint sm, uint data, uint tx_bit, uint rx_bit, uint offset)
{
    // Set number of bit to shift out by changing pull threshold
    pio_set_pull_threshold(pio, sm, tx_bit);

    // Set number of bit to shift in by encoding a "set x, rx_bit" instruction in pio instr memory
    uint instr = pio_encode_set(pio_x, rx_bit);
    pio_add_instr(pio, instr, offset + 1);

    // Put data in tx fifo:
    put_tx_fifo(pio, sm, data, tx_bit, SHIFT_RIGHT);
}


// Sync
uint sync(PIO pio, uint sm, float pio_freq)
{
    // Clear memory and fifos and add program
    uint offset = pio_program_init(pio, sm, &bdm_sync_program);

    // Calculate the PIO clock divider 
    float div = get_pio_clk_div(pio_freq);

    // Initialize the program using the helper function in our .pio file
    bdm_sync_program_init(pio, sm, offset, DATA_PIN, div);

    // Start running bdm-sync PIO program in the state machine
    pio_sm_set_enabled(pio, sm, true);

    // Wait for the sm to push data in rx fifo
    uint ticks = pio_sm_get_blocking(pio, sm);

    return ticks;
}