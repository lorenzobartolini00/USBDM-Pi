#include "tusb_option.h"

#if (TUSB_OPT_DEVICE_ENABLED && CFG_TUD_VENDOR)
#include "device/usbd_pvt.h"
#include "class/vendor/vendor_device.h"

#include "device/usbd.h"

#include "pico/stdlib.h"

#include "cmd_proc.h"
#include "config.h"


static uint8_t command_buffer[MAX_COMMAND_SIZE];
static uint8_t current_usb_command = 0;
static uint8_t command_size = 0;
static uint8_t command_offset = 0;
static uint8_t saved_byte = 0;


//--------------------------------------------------------------------+
// USB UTILS
//--------------------------------------------------------------------+

/**
 *  Set a command response over EP1 IN
 * 
 *  @param total_bytes   = # of bytes to send
 *  @param byte_count = ptr to bytes to send
 *  
 *
 *  @note : Returns before the command has been sent.
 *
 *  @note : Format
 *      - [0]    = response
 *      - [1..N] = parameters
 */
void send_USB_response(uint8_t *buffer, uint8_t byte_count)
{
  if (tud_vendor_write_available())
  {
    tud_vendor_write(buffer, byte_count);
  }
}


/**
 *   Receive a command over EP1 OUT
 * 
 *  @param itf = number of the interface to use
 * 
 *  @note : Format
 *       - [0]    = size of command (N) 
 *       - [1]    = command 
 *       - [2..N] = parameters 
 *
 *   ======================================================= 
 *   Format - a command is made up of up to 2 pkts
 *   The size of the 1st pkt indicates if subsequent pkts
 *   are used.
 *
 *    1st pkt
 *   +--------------------------+
 *   |  Size of entire command  |  0 - size includes 2nd pkt
 *   +--------------------------+
 *   |  Command byte            |  1
 *   +--------------------------+
 *   |                          |  2... up to BDM_OUT_EP_MAXSIZE-2
 *   | //// DATA ////////////// |
 *   |                          |
 *   +--------------------------+
 *    2nd pkt (optional)
 *   +--------------------------+
 *   |  0                       |  0 - Ensures pkt can't be mistaken as 1st pkt
 *   +--------------------------+
 *   |                          |  1... up to BDM_OUT_EP_MAXSIZE-1
 *   | //// DATA ////////////// |
 *   |                          |
 *   +--------------------------+
*/
USBDM_cmd_status receive_USB_command(void)
{
  USBDM_cmd_status command_status=CMD_FAIL;

  uint8_t command_toggle = 0;

  uint8_t byte_count = tud_vendor_read(command_buffer + command_offset, MAX_COMMAND_SIZE - command_offset);

  // Retrieve data
  uint8_t first_byte = command_buffer[command_offset];

  // True if the first packet is the only one present or the second packet has just been received
  bool done_receiving_data = false;

  // 1st pkt
  if (first_byte!=0)
  {
    // The first byte of the first packet is the size of the entire command
    command_size = first_byte;
    if (command_size > MAX_COMMAND_SIZE) 
    {
      command_size = MAX_COMMAND_SIZE;
    }

    // The second byte of the first packet is the command
    current_usb_command = command_buffer[command_offset + 1];

    if(command_size > byte_count)
    {
      // Second packet is coming next, so save offset to concatenate data. 
      // The last byte of the first packet will be overwritten, in order to make room for the first (useless) byte of the second packet.
      command_offset = byte_count - 1;

      // Save the last byte of the first packet, since it will be overwritten by the first of the second packet
      saved_byte=command_buffer[command_offset];

      command_status=CMD_WAIT;
    }
    else
    {
      // Only first packet is present
      done_receiving_data = true;

      command_offset=0;
    }
  }
  //2nd packet
  else
  {
    // Overwrite the first byte of the second packet, which contains zero, with the last byte of the first packet previously saved.
    command_buffer[command_offset] = saved_byte;

    // The second packet has arrived
    done_receiving_data = true;

    // Reset offset(only 2 consecutive transactions)
    command_offset=0;
  }

  if(done_receiving_data)
  {
    command_toggle = command_buffer[1] & 0x80;
    command_buffer[1] &= 0x7F;
    
    // Execute the command
    // NOTE: after excecuting a command, command_exec return the number of bytes to send back to host;
    uint8_t return_size = command_exec(command_buffer);
    command_buffer[0] |= command_toggle;

    if(command_buffer[0] == BDM_RC_OK)
    {
      command_status = CMD_OK;
    }

    send_USB_response(command_buffer, return_size);
  }

  return command_status;
}


// Invoked on Control Requests(Vendor type)
bool tud_vendor_control_xfer_cb(uint8_t rhport, uint8_t stage, tusb_control_request_t const * request)
{
  // nothing to with DATA & ACK stage
  if (stage != CONTROL_STAGE_SETUP) return true;

  // Handle vendor request only
  TU_VERIFY(request->bmRequestType_bit.type == TUSB_REQ_TYPE_VENDOR);

  switch ( request->bRequest )
  {
    case CMD_USBDM_GET_VER:
    {
      if (request->wValue == 0x100) 
      {
        uint8_t versionConstant[5];
        versionConstant[0] = BDM_RC_OK; 
        versionConstant[1] = VERSION_SW;      // BDM SW/HW version
        versionConstant[2] = VERSION_HW;      // ICP_Version_SW;
        versionConstant[3] = 0;               // ICP_Version_HW;
        versionConstant[4] = VERSION_HW;

        return tud_control_xfer(rhport, request, versionConstant, sizeof(versionConstant));
      }
      break;
    }
    default: return false; // stall unsupported request
  }

  return true;
}

#endif
  

