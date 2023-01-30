#include "tusb.h"
#include "usb_descriptors.h"
#include "config.h"

//--------------------------------------------------------------------+
// Device Descriptors 
//--------------------------------------------------------------------+
// #define CONST_NATIVE_TO_LE16(x) (((((x)&0xFF))<<8)+(((x)>>8)&0xFF))
 #define CONST_NATIVE_TO_LE16(x) (x)

tusb_desc_device_t const desc_device =
{
    .bLength            = sizeof(tusb_desc_device_t),
    .bDescriptorType    = TUSB_DESC_DEVICE,
    .bcdUSB             = CONST_NATIVE_TO_LE16(USB_BCD),
    .bDeviceClass       = TUSB_CLASS_VENDOR_SPECIFIC,
    .bDeviceSubClass    = TUSB_CLASS_VENDOR_SPECIFIC,
    .bDeviceProtocol    = TUSB_CLASS_VENDOR_SPECIFIC,
    .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,

    .idVendor           = CONST_NATIVE_TO_LE16(USB_VID),
    .idProduct          = CONST_NATIVE_TO_LE16(USB_PID),
    .bcdDevice          = CONST_NATIVE_TO_LE16(VERSION_ID),

    .iManufacturer      = s_manufacturer_index,
    .iProduct           = s_product_index,
    .iSerialNumber      = s_serial_index,

    .bNumConfigurations = NUMBER_OF_CONFIGURATIONS
};

// Invoked when received GET DEVICE DESCRIPTOR
// Application return pointer to descriptor
uint8_t const * tud_descriptor_device_cb(void)
{
  return (uint8_t const *) &desc_device;
}


//--------------------------------------------------------------------+
// Configuration Descriptor
//--------------------------------------------------------------------+
uint8_t const desc_configuration[] =
{
  // Config number, interface count, string index, total length, attribute, power in mA
  TUD_CONFIG_DESCRIPTOR(CONFIGURATION_NUM, NUMBER_OF_INTERFACES, s_config_index, CONFIG_TOTAL_LEN, TUSB_DESC_CONFIG_ATT_SELF_POWERED, 500),

// Interface number, string index, EP Out & IN address, EP size
  TUD_VENDOR_DESCRIPTOR(BULK_INTF_ID, s_bulk_interface_index, USB_DIR_OUT | BULK_ENDPOINT, USB_DIR_IN | BULK_ENDPOINT, CFG_TUD_VENDOR_EPSIZE),

  // 1st CDC: Interface number, string index, EP notification address and size, EP data address (out, in) and size.
  //TUD_CDC_DESCRIPTOR(ITF_NUM_CDC_0, s_cdc_interface_index, USB_DIR_IN | CDC_0_NOTIF_ENDPOINT, 8, USB_DIR_OUT | CDC_0_DATA_ENDPOINT, USB_DIR_IN | CDC_0_DATA_ENDPOINT, 64)

  // 2nd CDC: Interface number, string index, EP notification address and size, EP data address (out, in) and size.
  // TUD_CDC_DESCRIPTOR(ITF_NUM_CDC_1, s_cdc_interface_index, USB_DIR_IN | CDC_1_NOTIF_ENDPOINT, 8, USB_DIR_OUT | CDC_1_DATA_ENDPOINT, USB_DIR_IN | CDC_1_DATA_ENDPOINT, 64),

  
};

// Invoked when received GET CONFIGURATION DESCRIPTOR
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t const * tud_descriptor_configuration_cb(uint8_t index)
{
  (void) index; // for multiple configurations
  return desc_configuration;
}

//--------------------------------------------------------------------+
// String Descriptors
//--------------------------------------------------------------------+

char const* string_desc_arr[] = {
      (const char[]) {0x09, 0x0C},
      MANUFACTURER,
      PRODUCT_DESCRIPTION,
      SERIAL_NO,
      CONFIGURATION_DESCRIPTION,

      BULK_INTERFACE_DESCRIPTION
};

static uint16_t _desc_str[32];

// Invoked when received GET STRING DESCRIPTOR request
// Application return pointer to descriptor, whose contents must exist long enough for transfer to complete
uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid)
{
  (void) langid;

  uint8_t chr_count;

  if ( index == 0)
  {
    memcpy(&_desc_str[1], string_desc_arr[0], 2);
    chr_count = 1;
  }else
  {
    // Note: the 0xEE index string is a Microsoft OS 1.0 Descriptors.
    // https://docs.microsoft.com/en-us/windows-hardware/drivers/usbcon/microsoft-defined-usb-descriptors

    if (index==0xEE)
    {
      return (uint16_t *) OS_StringDescriptor;
    }

    if ( !(index < sizeof(string_desc_arr)/sizeof(string_desc_arr[0])) ) return NULL;

    const char* str = string_desc_arr[index];

    // Cap at max char
    chr_count = strlen(str);
    if ( chr_count > 31 ) chr_count = 31;

    // Convert ASCII string into UTF-16
    for(uint8_t i=0; i<chr_count; i++)
    {
      _desc_str[1+i] = str[i];
    }
  }

  // first byte is length (including header), second byte is string type
  _desc_str[0] = (TUSB_DESC_STRING << 8 ) | (2*chr_count + 2);

  return _desc_str;
}
