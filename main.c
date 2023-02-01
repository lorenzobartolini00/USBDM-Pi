#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "bsp/board.h"
#include "tusb.h"

#include "usb_descriptors.h"

#include "functions.h"
#include "pio_functions.h"
#include "bdm.h"
#include "config.h"

enum  {
  BLINK_COMMAND_OK = 125,
  BLINK_NOT_MOUNTED = 250,
  BLINK_MOUNTED     = 1000,
  BLINK_SUSPENDED   = 2500,

  BLINK_ALWAYS_ON   = UINT32_MAX,
  BLINK_ALWAYS_OFF  = 0
};

static uint32_t blink_interval_ms = BLINK_NOT_MOUNTED;

//------------- prototypes -------------//
void device_init(void);
void usbdm_task(void);
void led_blinking_task(void);


/*------------- MAIN -------------*/
int main(void)
{
  board_init();
  tusb_init();

  // Set clock and connect BDM
  device_init();

  while (1)
  {
    tud_task(); // tinyusb device task

    usbdm_task();

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
void device_init(void)
{
  // Set sys clock to 64MHz
  set_sys_clock_pll(VCO_FREQ * MHZ, POST_DEV1, POST_DEV2);
}

//--------------------------------------------------------------------+
// USBDM task
//--------------------------------------------------------------------+
void usbdm_task(void)
{
  // Check if board button is pressed
  uint32_t const btn = board_button_read();

  if (btn)
  {
    // Hold BKGD pin low for 5 seconds
    bdm_connect();

    // SYNC command
    bdm_cmd_sync();
  }

  USBDM_ErrorCode command_status;

  if (tud_vendor_available())
  {
    // Receive command from EP1 OUT
    USBDM_ErrorCode command_status = receive_USB_command();

    if ((uint8_t)command_status==BDM_RC_OK)
    {
      blink_interval_ms = BLINK_COMMAND_OK;
    }
    else
    {
      blink_interval_ms = BLINK_ALWAYS_OFF;
    }
  }

  return;
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