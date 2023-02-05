#include "tusb_option.h"

#if (TUSB_OPT_DEVICE_ENABLED && CFG_TUD_VENDOR)
#include "device/usbd_pvt.h"
#include "class/vendor/vendor_device.h"

#include "device/usbd.h"

#include "pico/stdlib.h"

#include "cmd_proc.h"
#include "config.h"


static uint8_t command_buffer[MAX_COMMAND_SIZE];
static uint8_t command_size = 0;
static uint8_t offset = 0;
static uint8_t saved_byte = 0;

// Signal the presence of first pkt
static bool first_pkt_received = false;



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

USBDM_ErrorCode receive_USB_command(void)
{
  uint8_t temp_buffer[MAX_COMMAND_SIZE];

  uint8_t byte_count = tud_vendor_read(temp_buffer, MAX_COMMAND_SIZE);

  // Get first byte
  uint8_t first_byte = temp_buffer[0];

  // 1st pkt or additional data
  if (first_byte!=0)
  {
    if (!first_pkt_received)
    {
      // Save entire command size
      command_size = first_byte;

      // Save last byte of the first pkt
      saved_byte = command_buffer[byte_count-1];

      first_pkt_received = true;
    }

    if (command_size > MAX_COMMAND_SIZE) 
    {
      command_size = MAX_COMMAND_SIZE;
    }

    // Save data in command buffer
    memcpy(command_buffer + offset, temp_buffer, byte_count);
  }
  //2nd packet
  else
  {
    // Save data in command buffer
    memcpy(command_buffer + (offset-1), temp_buffer, byte_count);

    // Overwrite the first byte of the second packet, which contains zero, with the last byte of the first packet previously saved.
    command_buffer[offset-1] = saved_byte;

    // Do not consider the first 0x00 byte in 2nd pkt in data count
    byte_count--;
  }

  offset += byte_count;

  // All data has been received
  if(offset == command_size)
  {
    //command_toggle = command_buffer[1] & 0x80;
    //command_buffer[1] &= 0x7F;
    
    // Execute the command
    // NOTE: after excecuting a command, command_exec return the number of bytes to send back to host;
    uint8_t return_size = command_exec(command_buffer);
    //command_buffer[0] |= command_toggle;

    send_USB_response(command_buffer, return_size);

    // Reset
    first_pkt_received = false;
    offset = 0;

    // Return command status
    return command_buffer[0];
  }
  else
  {
    return BDM_RC_BUSY;
  }
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
      
      uint8_t versionConstant[5];
      versionConstant[0] = BDM_RC_OK; 
      versionConstant[1] = VERSION_SW;      // BDM SW/HW version
      versionConstant[2] = VERSION_HW;      // ICP_Version_SW;
      versionConstant[3] = 0;               // ICP_Version_HW;
      versionConstant[4] = VERSION_HW;

      return tud_control_xfer(rhport, request, versionConstant, sizeof(versionConstant));
      
      break;
    }
    default: return false; // stall unsupported request
  }

  return true;
}


USBDM_ErrorCode send_USB_error_response(USBDM_ErrorCode code, uint8_t size)
{
  // Error
  command_buffer[0] = code;
  set_command_status(code);

  send_USB_response(command_buffer, size);

  return code;
}

#endif
  

