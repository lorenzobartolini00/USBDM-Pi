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
 */

#ifndef USB_DESCRIPTORS_H_
#define USB_DESCRIPTORS_H_

#endif /* USB_DESCRIPTORS_H_ */

//--------------------------------------------------------------------+
// IDs
//--------------------------------------------------------------------+

#define USB_VID   0x16D0
#define USB_BCD   0x0200
#define USB_PID   0x0567

#define VERSION_MAJOR 4 
#define VERSION_MINOR 12
#define VERSION_MICRO 1
#define VERSION_STR "4.12.1"
#define VERSION_SW  ((VERSION_MAJOR<<4)+VERSION_MINOR)
#define VERSION_ID VERSION_SW

#define VENDOR_CODE 0x30
static const uint8_t OS_StringDescriptor[] = {18, 3, 'M',0,'S',0,'F',0,'T',0,'1',0,'0',0,'0',0,VENDOR_CODE,0x00};

//--------------------------------------------------------------------+
// Interfaces
//--------------------------------------------------------------------+

enum InterfaceNumbers {
   BULK_INTF_ID,
   //ITF_NUM_CDC_0,
   //ITF_NUM_CDC_0_DATA,
   
   NUMBER_OF_INTERFACES,
};

//--------------------------------------------------------------------+
// Endpoints
//--------------------------------------------------------------------+

//! Endpoint direction masks
#define USB_DIR_OUT 0x00u
#define USB_DIR_IN 0x80u

enum EndpointNumbers {
   /** USB Control endpoint number - must be zero */
   CONTROL_ENDPOINT = 0,

   /** Bulk endpoint number */
   BULK_ENDPOINT,
   
   // CDC 0 Notif endpoint number
   //CDC_0_NOTIF_ENDPOINT,
   // CDC 0 Data endpoint number
   // CDC_0_DATA_ENDPOINT,
   

   /** Total number of end-points */
   NUMBER_OF_ENDPOINTS,
};

//--------------------------------------------------------------------+
// Configurations
//--------------------------------------------------------------------+

enum Configurations {
  CONFIGURATION_NUM = 1,
  /*
   * Assumes single configuration
   */
  /** Total number of configurations */
  NUMBER_OF_CONFIGURATIONS = CONFIGURATION_NUM,
};


#define CONFIG_TOTAL_LEN  (TUD_CONFIG_DESC_LEN + TUD_VENDOR_DESC_LEN + CFG_TUD_CDC * TUD_CDC_DESC_LEN)


//--------------------------------------------------------------------+
// Strings
//--------------------------------------------------------------------+
enum StringIds {
   /** Language information for string descriptors */
   s_language_index = 0,    // Must be zero
   /** Manufacturer */
   s_manufacturer_index,
   /** Product Description */
   s_product_index,
   /** Serial Number */
   s_serial_index,
   /** Configuration Index */
   s_config_index,

   /** Name of Bulk interface */
   s_bulk_interface_index,

   /** Name of CDC interface */
   s_cdc_interface_index,

   // Marks last entry
   s_number_of_string_descriptors
};

#define MANUFACTURER                "pgo"
#define PRODUCT_DESCRIPTION         "USBDM ARM-SWD for OpenSDAv2.1"
#define SERIAL_NO                   "USBDM-OPENSDA-0001"
#define CONFIGURATION_DESCRIPTION   "Default configuration"
#define CDC_INTERFACE_DESCRIPTION   "CDC Interface"               // 5: CDC Interface
#define BULK_INTERFACE_DESCRIPTION  "Bulk Interface"

     
