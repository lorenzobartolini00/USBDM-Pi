//! Internal Target Voltage supply selection
//!
typedef enum  {
   BDM_TARGET_VDD_OFF       = 0,     //!< Target Vdd Off
   BDM_TARGET_VDD_3V3       = 1,     //!< Target Vdd internal 3.3V
   BDM_TARGET_VDD_5V        = 2,     //!< Target Vdd internal 5.0V
   BDM_TARGET_VDD_ENABLE    = 0x10,  //!< Target Vdd internal at last set level
   BDM_TARGET_VDD_DISABLE   = 0x11,  //!< Target Vdd Off but previously set level unchanged
} TargetVddSelect_t;


//! Target BDM Clock selection
//!
typedef enum {
   CS_DEFAULT           = 0xFF,  //!< - Use default clock selection (don't modify target's reset default)
   CS_ALT               =  0,    //!< - Force ALT clock (CLKSW = 0)
   CS_NORMAL            =  1,    //!< - Force Normal clock (CLKSW = 1)
} ClkSwValues_t;

//! Auto-reconnect options
//!
typedef enum  {
   AUTOCONNECT_NEVER   = 0,  //!< Only connect explicitly
   AUTOCONNECT_STATUS  = 1,  //!< Reconnect on USBDM_ReadStatusReg()
   AUTOCONNECT_ALWAYS  = 2,  //!< Reconnect before every command
} AutoConnect_t;

// HCS08 Registers addresses
//===============================
//#define HCS08_SRS            (0x1800) //!< HCS08 SRS address
#define HCS08_SBDFR_DEFAULT  (0x1801) //!< Default HCS08 SBDFR address

// HCS08 Register bit masks
//===============================
#define HCS_SBDFR_BDFR (0x01) //!< HCS08 SBDFR BDFR mask


//! Target interface options
typedef struct {
   uint8_t  cycleVddOnReset:1;     //!< Cycle target Power  when resetting
   uint8_t  cycleVddOnConnect:1;   //!< Cycle target Power if connection problems (when resetting?)
   uint8_t  leaveTargetPowered:1;  //!< Leave target power on exit
   uint8_t  guessSpeed:1;          //!< Guess speed for target w/o ACKN
   uint8_t  useResetSignal:1;      //!< Use RESET signal on BDM interface
   uint8_t  targetVdd;             //!< Target Vdd (off, 3.3V or 5V)
   uint8_t  useAltBDMClock;        //!< Use alternative BDM clock source in target (HCS08)
   uint8_t  autoReconnect;         //!< Automatically re-connect method (for speed change)
   uint16_t SBDFRaddress;          //!< Address of HCS08_SBDFR register
   uint8_t  reserved[3];
} BDM_Option_t;

//! Target Status bit masks for \ref CMD_USBDM_GET_BDM_STATUS\n
//! \verbatim
//!     9       8       7       6       5        4       3       2       1       0
//! +-------+-------+-------+-------+--------+-------+-------+-------+-------+-------+
//! |      VPP      |     Power     |  Halt  | Communication | Reset | ResDet| Ackn  |
//! +-------+-------+-------+-------+--------+-------+-------+-------+-------+-------+
//! \endverbatim
typedef enum  {
   S_ACKN            = (1<<0),  //!< - Target supports BDM ACK (HCS08/12/CFV1)
   
   S_RESET_DETECT    = (1<<1),  //!< - Target has been reset since status last polled
   
   S_RESET_STATE     = (1<<2),  //!< - Current state of target reset pin (RESET or RSTO) (active low!)
   
   S_NOT_CONNECTED   = (0<<3),  //!< - No connection with target
   S_SYNC_DONE       = (1<<3),  //!< - Target communication speed determined by BDM SYNC
   S_GUESS_DONE      = (2<<3),  //!< - Target communication speed guessed
   S_USER_DONE       = (3<<3),  //!< - Target communication speed specified by user
   S_COMM_MASK       = (3<<3),  //!< - Mask for communication state
   
   S_HALT            = (1<<5),  //!< - Indicates target is halted (CF V2, V3 & V4)
   
   S_POWER_NONE      = (0<<6),  //!< - Target power not present
   S_POWER_EXT       = (1<<6),  //!< - External target power present
   S_POWER_INT       = (2<<6),  //!< - Internal target power on
   S_POWER_ERR       = (3<<6),  //!< - Internal target power error - overcurrent or similar
   S_POWER_MASK      = (3<<6),  //!< - Mask for Power
   
   S_VPP_OFF         = (0<<8),  //!< - Vpp Off
   S_VPP_STANDBY     = (1<<8),  //!< - Vpp Standby (Inverter on)
   S_VPP_ON          = (2<<8),  //!< - Vpp On
   S_VPP_ERR         = (3<<8),  //!< - Vpp Error - not used
   S_VPP_MASK        = (3<<8),  //!< - Mask for Vpp
} StatusBitMasks_t;

//! Target speed selection
//!
typedef enum {
   SPEED_NO_INFO        = 0,   //!< Not connected
   SPEED_SYNC           = 1,   //!< Speed determined by SYNC
   SPEED_GUESSED        = 2,   //!< Speed determined by trial & error
   SPEED_USER_SUPPLIED  = 3    //!< User has specified the speed to use
} SpeedMode_t;

//!  Target microcontroller types
//!
typedef enum {
   T_HC12      = 0,       //!< HC12 or HCS12 target
   T_HCS12     = T_HC12,  //!< HC12 or HCS12 target
   T_HCS08     = 1,       //!< HCS08 target
   T_RS08      = 2,       //!< RS08 target
   T_CFV1      = 3,       //!< Coldfire Version 1 target
   T_CFVx      = 4,       //!< Coldfire Version 2,3,4 target
   T_JTAG      = 5,       //!< JTAG target - TAP is set to \b RUN-TEST/IDLE
   T_EZFLASH   = 6,       //!< EzPort Flash interface (SPI?)
   T_MC56F80xx = 7,       //!< JTAG target with MC56F80xx optimised subroutines
   T_ARM_JTAG  = 8,       //!< ARM target using JTAG
   T_ARM_SWD   = 9,       //!< ARM target using SWD
   T_ARM       = 10,      //!< ARM target using either SWD (preferred) or JTAG as supported
   T_HCS12Z    = 11,      //!< MC9S12ZVM target
   T_LAST      = T_HCS12Z,
   T_ILLEGAL   = 0xFE,  //!< - Used to indicate error in selecting target
   T_OFF       = 0xFF,    //!< Turn off interface (no target)
} TargetType_t;

//! Target supports ACKN or uses fixed delay {WAIT} instead
//!
typedef enum {
   WAIT  = 0,   //!< Use WAIT (delay) instead
   ACKN  = 1,   //!< Target supports ACKN feature and it is enabled
} AcknMode_t;

//! Target reset status values
//!
typedef enum {
   NO_RESET_ACTIVITY    = 0,   //!< No reset activity since last polled
   RESET_INACTIVE       = NO_RESET_ACTIVITY,
   RESET_DETECTED       = 1    //!< Reset since last polled
} ResetMode_t;

//! Target Voltage supply state
//!
typedef enum  {
   BDM_TARGET_VDD_NONE      = 0,   //!< Target Vdd not detected
   BDM_TARGET_VDD_EXT       = 1,   //!< Target Vdd external
   BDM_TARGET_VDD_INT       = 2,   //!< Target Vdd internal
   BDM_TARGET_VDD_ERR       = 3,   //!< Target Vdd error
} TargetVddState_t;

//! Internal Programming Voltage supply selection
//!
typedef enum  {
   BDM_TARGET_VPP_OFF       = 0,   //!< Target Vpp Off
   BDM_TARGET_VPP_STANDBY   = 1,   //!< Target Vpp Standby (Inverter on, Vpp off)
   BDM_TARGET_VPP_ON        = 2,   //!< Target Vpp On
   BDM_TARGET_VPP_ERROR     = 3,   //!< Target Vpp ??
} TargetVppSelect_t;

//! Target status
typedef struct {
   TargetType_t        target_type:8;  //!< Target type \ref TargetType_t
   AcknMode_t          ackn:8;         //!< Target supports ACKN see \ref AcknMode_t
   ResetMode_t         reset:8;        //!< Target has been reset, see \ref ResetMode_T
   SpeedMode_t         speed:8;        //!< Target speed determination method, see \ref SpeedMode_t
   TargetVddState_t    power:8;        //!< Target Vdd state
   TargetVppSelect_t   flashState:8;   //!< State of RS08 Flash programming,  see \ref FlashState_t
   uint16_t            sync_length;    //!< Length of the target SYNC pulse in 60MHz ticks
   uint16_t            wait150_cnt;    //!< Time for 150 BDM cycles in bus cycles of the MCU divided by N
   uint16_t            wait64_cnt;     //!< Time for 64 BDM cycles in bus cycles of the MCU divided by N
   uint8_t             bdmpprValue;    //!< BDMPPR value for HCS12
} CableStatus_t;