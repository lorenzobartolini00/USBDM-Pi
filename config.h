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