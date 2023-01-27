/* 
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * This file is part of the TinyUSB stack.
 */

#include "tusb_option.h"

#if (TUSB_OPT_DEVICE_ENABLED && CFG_TUD_VENDOR)
#include "device/usbd_pvt.h"
#include "class/vendor/vendor_device.h"

#include "device/usbd.h"

#include "pico/stdlib.h"

#include "usbdm.h"
#include "usb_descriptors.h"

//--------------------------------------------------------------------+
// MACRO CONSTANT TYPEDEF
//--------------------------------------------------------------------+
typedef struct
{
  uint8_t itf_num;
  uint8_t ep_in;
  uint8_t ep_out;

  /*------------- From this point, data is not cleared by bus reset -------------*/
  tu_fifo_t rx_ff;
  tu_fifo_t tx_ff;

  uint8_t rx_ff_buf[CFG_TUD_VENDOR_RX_BUFSIZE];
  uint8_t tx_ff_buf[CFG_TUD_VENDOR_TX_BUFSIZE];
/*
#if CFG_FIFO_MUTEX
  osal_mutex_def_t rx_ff_mutex;
  osal_mutex_def_t tx_ff_mutex;
#endif
*/

  // Endpoint Transfer buffer
  CFG_TUSB_MEM_ALIGN uint8_t epout_buf[CFG_TUD_VENDOR_EPSIZE];
  CFG_TUSB_MEM_ALIGN uint8_t epin_buf[CFG_TUD_VENDOR_EPSIZE];
} vendord_interface_t;

CFG_TUSB_MEM_SECTION static vendord_interface_t _vendord_itf[CFG_TUD_VENDOR];

#define ITF_MEM_RESET_SIZE   offsetof(vendord_interface_t, rx_ff)

//--------------------------------------------------------------------+
// Read API
//--------------------------------------------------------------------+
static void _prep_out_transaction (vendord_interface_t* p_itf)
{
  // skip if previous transfer not complete
  if ( usbd_edpt_busy(TUD_OPT_RHPORT, p_itf->ep_out) ) return;

  // Prepare for incoming data but only allow what we can store in the ring buffer.
  uint16_t max_read = tu_fifo_remaining(&p_itf->rx_ff);
  if ( max_read >= CFG_TUD_VENDOR_EPSIZE )
  {
    usbd_edpt_xfer(TUD_OPT_RHPORT, p_itf->ep_out, p_itf->epout_buf, CFG_TUD_VENDOR_EPSIZE);
  }
}

//--------------------------------------------------------------------+
// Write API
//--------------------------------------------------------------------+
static bool maybe_transmit(vendord_interface_t* p_itf)
{
  // skip if previous transfer not complete
  TU_VERIFY( !usbd_edpt_busy(TUD_OPT_RHPORT, p_itf->ep_in) );

  uint16_t count = tu_fifo_read_n(&p_itf->tx_ff, p_itf->epin_buf, CFG_TUD_VENDOR_EPSIZE);
  if (count > 0)
  {
    TU_ASSERT( usbd_edpt_xfer(TUD_OPT_RHPORT, p_itf->ep_in, p_itf->epin_buf, count) );
  }
  return true;
}

//--------------------------------------------------------------------+
// USBD Driver API
//--------------------------------------------------------------------+
void usbdmd_init(void)
{
  tu_memclr(_vendord_itf, sizeof(_vendord_itf));

  for(uint8_t i=0; i<CFG_TUD_VENDOR; i++)
  {
    vendord_interface_t* p_itf = &_vendord_itf[i];

    // config fifo
    tu_fifo_config(&p_itf->rx_ff, p_itf->rx_ff_buf, CFG_TUD_VENDOR_RX_BUFSIZE, 1, false);
    tu_fifo_config(&p_itf->tx_ff, p_itf->tx_ff_buf, CFG_TUD_VENDOR_TX_BUFSIZE, 1, false);



/*
#if CFG_FIFO_MUTEX
    tu_fifo_config_mutex(&p_itf->rx_ff, osal_mutex_create(&p_itf->rx_ff_mutex));
    tu_fifo_config_mutex(&p_itf->tx_ff, osal_mutex_create(&p_itf->tx_ff_mutex));
#endif
*/
  }
}

void usbdmd_reset(uint8_t rhport)
{
  (void) rhport;

  for(uint8_t i=0; i<CFG_TUD_VENDOR; i++)
  {
    vendord_interface_t* p_itf = &_vendord_itf[i];

    tu_memclr(p_itf, ITF_MEM_RESET_SIZE);
    tu_fifo_clear(&p_itf->rx_ff);
    tu_fifo_clear(&p_itf->tx_ff);
  }
}

uint16_t usbdmd_open(uint8_t rhport, tusb_desc_interface_t const * itf_desc, uint16_t max_len)
{
  TU_VERIFY(TUSB_CLASS_VENDOR_SPECIFIC == itf_desc->bInterfaceClass, 0);

  uint16_t const drv_len = sizeof(tusb_desc_interface_t) + itf_desc->bNumEndpoints*sizeof(tusb_desc_endpoint_t);
  TU_VERIFY(max_len >= drv_len, 0);

  // Find available interface
  vendord_interface_t* p_vendor = NULL;
  for(uint8_t i=0; i<CFG_TUD_VENDOR; i++)
  {
    if ( _vendord_itf[i].ep_in == 0 && _vendord_itf[i].ep_out == 0 )
    {
      p_vendor = &_vendord_itf[i];
      break;
    }
  }
  TU_VERIFY(p_vendor, 0);

  // Open endpoint pair with usbd helper
  TU_ASSERT(usbd_open_edpt_pair(rhport, tu_desc_next(itf_desc), 2, TUSB_XFER_BULK, &p_vendor->ep_out, &p_vendor->ep_in), 0);

  p_vendor->itf_num = itf_desc->bInterfaceNumber;

  // Prepare for incoming data
  if ( !usbd_edpt_xfer(rhport, p_vendor->ep_out, p_vendor->epout_buf, sizeof(p_vendor->epout_buf)) )
  {
    TU_LOG_FAILED();
    TU_BREAKPOINT();
  }

  return drv_len;
}

// Invoked on
// 1.Device Control Requests(Class type)
// 1.Interface Control Requests(Class & STD type)
static bool usbdmd_control_xfer_cb(uint8_t __unused rhport, uint8_t stage, tusb_control_request_t const * request) {
  // nothing to do with DATA & ACK stage
    if (stage != CONTROL_STAGE_SETUP) return true;
    
    return false;
}

// Invoked on Control Requests(Vendor type)
bool tud_vendor_control_xfer_cb(uint8_t rhport, uint8_t stage, tusb_control_request_t const * request)
{
  // Handle vendor request only
  TU_VERIFY(request->bmRequestType_bit.type == TUSB_REQ_TYPE_VENDOR);

  uint8_t itf = 0;
  vendord_interface_t* p_vendor = _vendord_itf;

  // Identify which interface to use
  for ( ; ; itf++, p_vendor++)
  {
    if (itf >= TU_ARRAY_SIZE(_vendord_itf)) return false;

    if ( p_vendor->itf_num == request->wIndex ) break;
  }

  switch ( request->bRequest )
  {
    case CMD_USBDM_GET_VER:
      if (stage == CONTROL_STAGE_SETUP)
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
      }
      break;

    default: return false; // stall unsupported request
  }

  return true;
}


bool usbdmd_xfer_cb(uint8_t rhport, uint8_t ep_addr, xfer_result_t result, uint32_t xferred_bytes)
{
  (void) rhport;
  (void) result;

  uint8_t itf = 0;
  vendord_interface_t* p_itf = _vendord_itf;

  for ( ; ; itf++, p_itf++)
  {
    if (itf >= TU_ARRAY_SIZE(_vendord_itf)) return false;

    if ( ( ep_addr == p_itf->ep_out ) || ( ep_addr == p_itf->ep_in ) ) break;
  }

  if ( ep_addr == p_itf->ep_out )
  {
    // Receive new data
    tu_fifo_write_n(&p_itf->rx_ff, p_itf->epout_buf, xferred_bytes);

    // Invoked callback if any
    if (tud_vendor_rx_cb) tud_vendor_rx_cb(itf);

    _prep_out_transaction(p_itf);
  }
  else if ( ep_addr == p_itf->ep_in )
  {
    // Send complete, try to send more if possible
    maybe_transmit(p_itf);
  }

  return true;
}

#endif


static usbd_class_driver_t const _usbdmd_driver =
{
    .init             = usbdmd_init,
    .reset            = usbdmd_reset,
    .open             = usbdmd_open,
    .control_xfer_cb  = usbdmd_control_xfer_cb,
    .xfer_cb          = usbdmd_xfer_cb,
    .sof              = NULL
};

//--------------------------------------------------------------------+
// Callbacks
//--------------------------------------------------------------------+

// Implement callback to add our custom driver
usbd_class_driver_t const *usbd_app_driver_get_cb(uint8_t *driver_count) {
    *driver_count = 1;
    return &_usbdmd_driver;
}

uint32_t tud_usbdm_n_read (uint8_t itf, void* buffer, uint32_t bufsize)
{
  vendord_interface_t* p_itf = &_vendord_itf[itf];
  uint32_t num_read = tu_fifo_read_n(&p_itf->rx_ff, buffer, bufsize);
  _prep_out_transaction(p_itf);
  return num_read;
}

// Invoked when received new data
void tud_vendor_rx_cb(uint8_t itf)
{
  vendord_interface_t p_itf = _vendord_itf[itf];
  uint8_t buffer[16];

  tud_usbdm_n_read(itf, buffer, 16);

  if (buffer[0]==0x02)
  {
    // Turn on LED
    gpio_put(25, true);
  }
}
