#include "pico/stdlib.h"

#define ACK_ENABLE	    ((uint8_t)0xD5)
#define ACK_DISABLED	((uint8_t)0xD6)
#define BACKGROUND	    ((uint8_t)0x90)
#define GO		        ((uint8_t)0x08)
#define TRACE1		    ((uint8_t)0x10)
#define TAGGO		    ((uint8_t)0x18)

//-------------------------------------------------------
#define READ_A		    ((uint8_t)0x68)
#define READ_CCR	    ((uint8_t)0x69)
#define READ_PC		    ((uint8_t)0x6B)
#define READ_HX		    ((uint8_t)0x6C)
#define READ_SP		    ((uint8_t)0x6F)
#define READ_NEXT	    ((uint8_t)0x70)
#define READ_NEXT_WS	((uint8_t)0x71)

//--------------------------------------------------
#define WRITE_A 	    ((uint8_t)0x48)
#define WRITE_CCR  	    ((uint8_t)0x49)
#define WRITE_PC  	    ((uint8_t)0x4B)
#define WRITE_HX  	    ((uint8_t)0x4C)
#define WRITE_SP  	    ((uint8_t)0x4F)
#define WRITE_NEXT  	((uint8_t)0x50)
#define WRITE_NEXT_WS 	((uint8_t)0x51)

//--------------------------------------------------
#define READ_STATUS 	((uint8_t)0xE4)
#define WRITE_CONTROL 	((uint8_t)0xC4)
#define READ_BYTE 	    ((uint8_t)0xE0)
#define READ_BYTE_WS 	((uint8_t)0xE1)
#define READ_LAST  	    ((uint8_t)0xE8)
#define WRITE_BYTE 	    ((uint8_t)0xC0)
#define WRITE_BYTE_WS   ((uint8_t)0xC1)
#define READ_BKPT 	    ((uint8_t)0xE2)
#define WRITE_BKPT 	    ((uint8_t)0xC2)
//=====================================================================================
#define BYTE    8   // bits
#define MAX_BDM_COMMAND_SIZE 6

enum{
    TX_BYTE_COUNT = 0,
    RX_BYTE_COUNT = 1,
    COMMAND = 2,
    FIRST_PARAMETER = 3,
    SECOND_PARAMETER = 4,
    THIRD_PARAMETER = 5
};


uint bdm_command_exec(void);

//=====================================================================================
// BDM commands
//=====================================================================================
void bdm_cmd_sync(void);
uint16_t bdm_cmd_get_sync_length(void);

void bdm_cmd_read_status(uint8_t *command_buffer);
void bdm_cmd_write_control(uint8_t *command_buffer);

void bdm_cmd_reset(void);
void bdm_cmd_go(void);
void bdm_cmd_halt(void);

void bdm_cmd_read_a(uint8_t *command_buffer);
void bdm_cmd_read_ccr(uint8_t *command_buffer);
void bdm_cmd_read_pc(uint8_t *command_buffer);
void bdm_cmd_read_hx(uint8_t *command_buffer);
void bdm_cmd_read_sp(uint8_t *command_buffer);

void bdm_cmd_write_a(uint8_t *command_buffer);
void bdm_cmd_write_ccr(uint8_t *command_buffer);
void bdm_cmd_write_pc(uint8_t *command_buffer);
void bdm_cmd_write_hx(uint8_t *command_buffer);
void bdm_cmd_write_sp(uint8_t *command_buffer);

void bdm_cmd_write_byte(uint8_t addr_h, uint8_t addr_l, uint8_t data);
void bdm_cmd_write_next(uint8_t data);
void bdm_cmd_read_byte(uint8_t addr_h, uint8_t addr_l, uint8_t *data_ptr);
void bdm_cmd_read_next(uint8_t *data_ptr);