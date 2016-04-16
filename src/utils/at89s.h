#ifndef _AT89S_TYPES_H_
#define _AT89S_TYPES_H_

/*******************************************************************
 *
 *      HEADERS
 *
 ******************************************************************/
#include <stdio.h>
#include <stdint.h>




/*******************************************************************
 *
 *      PREPROCESSOR & CONSTANT
 *
 ******************************************************************/
/*
 * Protocol between Programmer & Commander
 */
#define  CMD_NULL      0x00
#define  CMD_W_MEM     0x01
#define  CMD_R_MEM     0x02
#define  CMD_E_MEM     0x03
#define  CMD_R_SIG     0x04
#define  CMD_R_USIG    0x05
#define  CMD_W_USIG    0x06

#define  RES_W_MEM     0x10
#define  RES_R_MEM     0x20
#define  RES_E_MEM     0x30
#define  RES_R_SIG     0x40
#define  RES_R_USIG    0x50
#define  RES_W_USIG    0x60

#define  PADDING_BYTE  0x21

#define  MEM_FLS       0x00
#define  MEM_ROM       0x01
#define  MEM_RAM       0x02

#define  SIGN_DEV      0x00
#define  SIGN_USR      0x01


/*
 * Maximum packet size
 */
#define  MSG_SIZE    256
#define  DATA_SIZE   (MSG_SIZE - 1)


/*******************************************************************
 *
 *      DATA TYPE
 *
 ******************************************************************/
/*
 * Error ID
 */
typedef enum AT89S_EID
{
    EID_OK = 0,
    EID_NOK,

    EID_ARG_NULL,
    EID_ARG_INVALID,

    EID_MEM_ALLOC,

    EID_SERIAL_OPEN,
    EID_SERIAL_CLOSE,
    EID_SERIAL_SEND,
    EID_SERIAL_RECV,
    EID_SERIAL_ATTR_SET,
    EID_SERIAL_ATTR_GET,
    EID_SERIAL_IOSPEED_SET,
    EID_SERIAL_BAUDRATE_INVALID,
    EID_SERIAL_CONFIG_INVALID,

    EID_HEX_OPEN,
    EID_HEX_CLOSE,
    EID_HEX_READ,
    EID_HEX_WRITE,
    EID_HEX_EOF,

    EID_REC_CRC,
    EID_REC_FMT,
    EID_REC_TYPE,
    EID_REC_CORRUPT,
    EID_REC_EMPTY,
    EID_REC_LEAK,

    EID_CMD_LEN,
    EID_CMD_BAD,
    EID_CMD_CRC,
    EID_CMD_DATA,

    EID_ENC_NOT_FOUND,

    EID_DEC_NOT_FOUND,

} AT89S_EID;

/*
 * Command Read Signature
 */
typedef struct Cmd_ReadSign_t
{
    uint8_t signature_type;

} Cmd_ReadSign_t;

/*
 * Command Write Signature
 */
typedef struct Cmd_WriteSign_t
{
    uint8_t signature[3];

} Cmd_WriteSign_t;

/*
 * Command Read Memory
 */
typedef struct Cmd_ReadMem_t
{
    uint8_t   memtype;
    uint16_t  address;
    uint8_t   size;

} Cmd_ReadMem_t;

/*
 * Command Write Memory
 */
typedef struct Cmd_WriteMem_t
{
    uint8_t   memtype;
    uint16_t  address;
    uint8_t   size;
    uint16_t  crc;
    uint8_t   data[DATA_SIZE - 6];

} Cmd_WriteMem_t;

/*
 * Command Erase Memory
 */
typedef struct Cmd_EraseMem_t
{
    uint8_t   memtype;
    uint16_t  address;
    uint8_t   size;

} Cmd_EraseMem_t;


/*
 * Response Read Memory
 */
typedef struct Res_ReadMem_t
{
    uint8_t  size;
    uint16_t crc;
    uint8_t  data[DATA_SIZE - 3];

} Res_ReadMem_t;

/*
 * Response Read Signature
 */
typedef struct Res_ReadSign_t
{
    uint8_t  signature[3];

} Res_ReadSign_t;

/*
 * Message Data
 */
typedef union
{
    Cmd_ReadMem_t   cmd_read_mem;
    Cmd_WriteMem_t  cmd_write_mem;
    Cmd_EraseMem_t  cmd_erase_mem;
    Cmd_ReadSign_t  cmd_read_sign;
    Cmd_WriteSign_t cmd_write_sign;
    Res_ReadSign_t  res_read_sign;
    Res_ReadMem_t   res_read_mem;
} MsgData_t;

/*
 * AT89S Command Message
 */
typedef struct AT89S_Msg_t
{
    uint8_t msgt;
    MsgData_t data;

} AT89S_Msg_t;

/*******************************************************************
 *
 *      API DECLARATION
 *
 ******************************************************************/
AT89S_EID msg_decode ( char* data_buf,
                       int   data_len,
                       AT89S_Msg_t* atmsg );

AT89S_EID msg_encode ( AT89S_Msg_t* atmsg,
                       char* data_buf,
                       int* data_len );


#endif /* _AT89S_TYPES_H_ */
