#include "device/usbd.h"

//! Memory space indicator - includes element size
//!
typedef enum {
   // One of the following
   MS_Byte     = 1,        // Byte (8-bit) access
   MS_Word     = 2,        // Word (16-bit) access
   MS_Long     = 4,        // Long (32-bit) access
   // One of the following
   MS_None     = 0<<4,     // Memory space unused/undifferentiated
   MS_Program  = 1<<4,     // Program memory space (e.g. P: on DSC)
   MS_Data     = 2<<4,     // Data memory space (e.g. X: on DSC)
   MS_Global   = 3<<4,     // HCS12 Global addresses
   // Fast memory access for HCS08/HCS12 (stopped target, regs. are modified
   MS_FAST     = 1<<7,
   // Masks for above
   MS_SIZE     = 0x7<<0,   // Size
   MS_SPACE    = 0x7<<4,   // Memory space

   // For convenience (DSC)
   MS_PWord    = MS_Word+MS_Program,
   MS_PLong    = MS_Long+MS_Program,
   MS_XByte    = MS_Byte+MS_Data,
   MS_XWord    = MS_Word+MS_Data,
   MS_XLong    = MS_Long+MS_Data,
} MemorySpace_t;

//! regNo Parameter for USBDM_ReadReg() with HCS08 target
//!
typedef enum {
   HCS08_RegPC  = 0xB,  //!< PC  reg
   HCS08_RegSP  = 0xF,  //!< SP  reg
   HCS08_RegHX  = 0xC,  //!< HX  reg
   HCS08_RegA   = 8,    //!< A   reg
   HCS08_RegCCR = 9,    //!< CCR reg
} HCS08_Registers_t;

//!  Reset mode as used by CMD_USBDM_TARGET_RESET
//!
typedef enum { /* type of reset action required */
   RESET_MODE_MASK   = (3<<0), //!< Mask for reset mode (SPECIAL/NORMAL)
   RESET_SPECIAL     = (0<<0), //!< - Special mode [BDM active, Target halted]
   RESET_NORMAL      = (1<<0), //!< - Normal mode [usual reset, Target executes]

   RESET_TYPE_MASK   = (7<<2), //!< Mask for reset type (Hardware/Software/Power)
   RESET_ALL         = (0<<2), //!< Use all reset strategies as appropriate
   RESET_HARDWARE    = (1<<2), //!< Use hardware RESET pin reset
   RESET_SOFTWARE    = (2<<2), //!< Use software (BDM commands) reset
   RESET_POWER       = (3<<2), //!< Cycle power

   // Legacy methods
//   SPECIAL_MODE = RESET_SPECIAL|RESET_ALL,  //!< - Special mode [BDM active, Target halted]
//   NORMAL_MODE  = RESET_NORMAL|RESET_ALL,   //!< - Normal mode [usual reset, Target executes]

} TargetMode_t;

//--------------------------------------------------------------------+
// IMPLEMENTED FUNCTIONALITIES
//--------------------------------------------------------------------+
uint8_t _cmd_usbdm_set_target();
uint8_t _cmd_usbdm_set_vdd(uint8_t* command_buffer);
uint8_t _cmd_usbdm_get_bdm_status(uint8_t* command_buffer);
uint8_t _cmd_usbdm_get_capabilities(uint8_t* command_buffer);
uint8_t _cmd_usbdm_set_options(uint8_t* command_buffer);

uint8_t _cmd_usbdm_connect();
uint8_t _cmd_usbdm_set_speed(uint8_t* command_buffer);
uint8_t _cmd_usbdm_get_speed(uint8_t* command_buffer);

uint8_t _cmd_usbdm_read_status_reg(uint8_t* command_buffer);
uint8_t _cmd_usbdm_write_control_reg(uint8_t* command_buffer);

uint8_t _cmd_usbdm_reset(uint8_t* command_buffer);
uint8_t _cmd_usbdm_go(uint8_t* command_buffer);
uint8_t _cmd_usbdm_halt(uint8_t* command_buffer);

uint8_t _cmd_usbdm_write_reg(uint8_t* command_buffer);
uint8_t _cmd_usbdm_read_reg(uint8_t* command_buffer);

uint8_t _cmd_usbdm_write_mem(uint8_t* command_buffer);
uint8_t _cmd_usbdm_read_mem(uint8_t* command_buffer);

// Processes all commands received over USB
uint8_t command_exec(uint8_t* command_buffer);
void set_command_status(uint8_t status);
