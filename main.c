#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "bsp/board.h"
#include "tusb.h"

#include "usb_descriptors.h"

#include "functions.h"
#include "pio_functions.h"

enum  {
  BLINK_FAST = 125,
  BLINK_NOT_MOUNTED = 250,
  BLINK_MOUNTED     = 1000,
  BLINK_SUSPENDED   = 2500,

  BLINK_ALWAYS_ON   = UINT32_MAX,
  BLINK_ALWAYS_OFF  = 0
};

static uint32_t blink_interval_ms = BLINK_NOT_MOUNTED;

uint8_t data[4]={'c','i','a','o'};

//------------- prototypes -------------//
void usbdm_task(void);
static void cdc_task(void);
void led_blinking_task(void);


/*------------- MAIN -------------*/
int main(void)
{
  board_init();
  tusb_init();

  while (1)
  {
    tud_task(); // tinyusb device task

    usbdm_task();
    // cdc_task();

    led_blinking_task();
  }

  return 0;
}

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void)
{
  blink_interval_ms = BLINK_MOUNTED;
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
  blink_interval_ms = BLINK_NOT_MOUNTED;
}

//--------------------------------------------------------------------+
// Device init
//--------------------------------------------------------------------+
void device_init()
{
  // Set GPIO function to SIO
  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, true);

  // Set sys clock to 64MHz
  set_sys_clock_pll(VCO_FREQ * MHZ, POST_DEV1, POST_DEV2);
}

//--------------------------------------------------------------------+
// USBDM task
//--------------------------------------------------------------------+
void usbdm_task(void)
{
  
}

//--------------------------------------------------------------------+
// BLINKING TASK
//--------------------------------------------------------------------+
void led_blinking_task(void)
{
  static uint32_t start_ms = 0;
  static bool led_state = false;

  // Blink every interval ms
  if ( board_millis() - start_ms < blink_interval_ms) return; // not enough time
  start_ms += blink_interval_ms;

  board_led_write(led_state);
  led_state = 1 - led_state; // toggle
}