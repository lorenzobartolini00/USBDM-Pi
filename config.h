//--------------------------------------------------------------------+
// BDM CONFIG
//--------------------------------------------------------------------+
#define COMMAND_BITS    8       // How many bits in a command word
#define SHIFT_RIGHT     false   // Whether the bit are shift out/in to the right or not
#define AUTO_PULL       false   // Whehter autopull is enabled
#define AUTO_PUSH       false   // Whehter autopush is enabled
#define REG_WIDTH       32      // Max number of bit of the registers
#define DELAY_CYCLES    16      // Cycle of delay before a write operation or after a read operation
#define FIFO_WIDTH      4

#define DATA_PIN        15      // BKGD pin 
#define LED_PIN         25      // LED pin

#define BUFFER_LENGTH   15      // Max number of chars the get_string function can read
#define NEW_LINE '\r'           // New line character. In some terminal this should be replaced with "\n"

//                   REF     FBDIV VCO       POSTDIV
// PLL SYS: 12 / 1 = 12MHz * 128 = 1536 / 6 / 2 = 128MHz    (Final)
// PLL SYS: 12 / 1 = 12MHz * 48 = 768 / 6 / 6 = 21.3MHz     (Test)
#define VCO_FREQ        1536  
#define POST_DEV1       6
#define POST_DEV2       2

// Final freq: 16MHZ
// Test freq: 400Hz
#define PIO_FREQ        16000000   //The default pio's clock speed in Hz
// 2Mhz
#define SYNC_FREQ       2000000   //The slowest possible BDC clock(normally the reference oscillator/64) in Hz
#define SYNC_PERIOD     0.5       //in us

#define SYNC_COUNT_THRESHOLD    5   // Consecutive commands before a sync command should take place

#define AUTO_SYNC   false

//--------------------------------------------------------------------+
// USB CONFIG
//--------------------------------------------------------------------+
#include "usbdm.h"

// USB Serial Number
#define SERIAL_NO          "USBDM-TWR-HCS08-0001"
#define ProductDescription "USBDM RS08,HCS08 BDM for Tower"

#define MANUFACTURER                "pgo"
#define PRODUCT_DESCRIPTION         "USBDM ARM-SWD for OpenSDAv2.1"
#define CONFIGURATION_DESCRIPTION   "Default configuration"
#define BULK_INTERFACE_DESCRIPTION  "Bulk Interface"
#define CDC_INTERFACE_DESCRIPTION   "CDC Interface"               // 5: CDC Interface

// Capabilities of the hardware - used to enable/disable appropriate code
//
#define HW_CAPABILITY     (CAP_BDM) //(CAP_VDDCONTROL|CAP_CDC|CAP_BDM|CAP_FLASH|CAP_CORE_REGS)
#define TARGET_CAPABILITY (CAP_HCS08) //(CAP_VDDCONTROL|CAP_CDC|CAP_RS08|CAP_HCS08)

#define HW_JB        0x00
#define HW_JM        0x80
#define HW_UF        0xC0

#define VERSION_MAJOR 4     
#define VERSION_MINOR 12
#define VERSION_MICRO 1

#define VERSION_STR "4.12.1.295"
#define VERSION_SW  ((VERSION_MAJOR<<4)+VERSION_MINOR)
#define VERSION_ID VERSION_SW


#define TARGET_HARDWARE H_USBDM_TWR_HCS08

#define VERSION_HW  (HW_JM+TARGET_HARDWARE)

#define ICP_VERSION_SW_LOCATION (0xFFFC)
#define ICP_VERSION_HW_LOCATION (ICP_VERSION_SW_LOCATION+1)

//! ICP version (2 hex digits, major.minor)
#define ICP_VERSION_SW (2<<4|6) // 2.6

#define MAX_COMMAND_SIZE       (254)