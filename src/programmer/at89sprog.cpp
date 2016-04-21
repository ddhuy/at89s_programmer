/*******************************************************************
 *
 *      HEADERS
 *
 ******************************************************************/
#include <string.h>
#include <util/delay.h>

#include "Arduino.h"
#include "at89s52.h"
#include "crc.h"



/*******************************************************************
 *
 *      PREPROCESSORS
 *
 ******************************************************************/
#define  LED_01    10
#define  LED_02    9
#define  LED_03    8

#define  PIN_RST   49
#define  PIN_MISO  50
#define  PIN_MOSI  51
#define  PIN_SCK   52



#define  DEFAULT_BAUDRATE    9600


/*******************************************************************
 *
 *      PRIVATE FUNCTION DECLARATION
 *
 ******************************************************************/
/*
 *
 */
static void
start_reprogram ( void );


/*
 *
 */
static void
reset_mcu( void );



/*
 *
 */
static unsigned long
send_mcu_cmd ( unsigned long command );


/*
 *
 */
static unsigned long
send_byte_cmd ( unsigned char command );



/*
 *
 */
static void
erase_chip ( void );


/*
 *
 */
static AT89S_EID
process_message ( AT89S_Msg_t* atmsg_ptr );


/*
 *
 */
static AT89S_EID
read_device_signature ( Msg_Signature_t * signature_msg_ptr );

/*
 *
 */
static AT89S_EID
write_data ( Msg_Memmory_t * mem_msg_ptr );


/*
 *
 */
static AT89S_EID
read_data ( Msg_Memmory_t * mem_msg_ptr );

/*******************************************************************
 *
 *      GLOBAL VARIABLES
 *
 ******************************************************************/
/*
 * Global raw data message
 */
char      g_data_buf[MSG_SIZE] = { 0 };
uint16_t  g_data_len = 0;
uint8_t   g_data_crc = 0;

/*
 * Global AT89S Message
 */
AT89S_Msg_t  g_at89msg;


/*
 * Global Error ID
 */
AT89S_EID  g_eid = EID_OK;


/*******************************************************************
 *
 *      FUNCTION DEFINITION
 *
 ******************************************************************/
/*
 *
 */
static void
start_reprogram ( void )
{
    digitalWrite(PIN_RST, HIGH);
    digitalWrite(PIN_SCK, LOW);
    delayMicroseconds(T_RESET);

    send_mcu_cmd(PROGRAM_ENABLE);
}

/*
 *
 */
static void
reset_mcu( void )
{
    digitalWrite(PIN_RST, LOW);
}


/*
 *
 */
static void
erase_chip ( void )
{
    send_mcu_cmd(CHIP_ERASE);
    delayMicroseconds(T_ERASE);
}

/*
 *
 */
static unsigned long
send_mcu_cmd ( unsigned long command )
{
    unsigned long resp = 0;
    signed char i = 0;

    for (i = 31; i >= 0; --i)
    {
        digitalWrite(PIN_SCK, LOW);
        digitalWrite(PIN_MOSI, ((command >> i) & 0x01));
        digitalWrite(PIN_SCK, HIGH);
        resp |= ((digitalRead(PIN_MISO) & 0x01) << i);
    }
    digitalWrite(PIN_SCK, HIGH);

    return resp;
}


/*
 *
 */
static unsigned long
send_byte_cmd ( unsigned char command )
{
    unsigned long resp = 0;
    signed char i = 0;

    for (i = 7; i >= 0; --i)
    {
        digitalWrite(PIN_SCK, LOW);
        digitalWrite(PIN_MOSI, ((command >> i) & 0x01));
        digitalWrite(PIN_SCK, HIGH);
        resp |= ((digitalRead(PIN_MISO) & 0x01) << i);
    }
    digitalWrite(PIN_SCK, HIGH);

    return resp;
}


/*
 *
 */
static AT89S_EID
process_message ( AT89S_Msg_t* atmsg_ptr )
{
    AT89S_EID eid = EID_OK;

    if (atmsg_ptr == NULL)
    {
        eid = EID_ARG_NULL;
    }
    else
    {
        switch (atmsg_ptr->msgt)
        {
            case CMD_W_MEM:
                break;

            case CMD_R_MEM:
                break;

            case CMD_E_MEM:
                break;

            case CMD_R_SIG:
                eid = read_device_signature(&atmsg_ptr->data.msg_signature);
                break;

            case CMD_W_USIG:
                break;

            case CMD_R_USIG:
                break;

            default:
                break;
        }
    }

    return eid;
}

/*
 *
 */
static AT89S_EID
read_device_signature ( Msg_Signature_t * signature_msg_ptr )
{
    AT89S_EID eid = EID_OK;
    unsigned long res = 0;

    if (signature_msg_ptr->type == SIGN_DEV)
    {
        // start reprogram mode
        start_reprogram();
        // read device signature
        signature_msg_ptr->signature[0] = ((unsigned char *) &res)[0];
        res = send_mcu_cmd(READ_SIGNATURE_BYTE1);
        signature_msg_ptr->signature[1] = ((unsigned char *) &res)[0];
        res = send_mcu_cmd(READ_SIGNATURE_BYTE2);
        signature_msg_ptr->signature[2] = ((unsigned char *) &res)[0];
        res = send_mcu_cmd(READ_SIGNATURE_BYTE3);
        signature_msg_ptr->signature[3] = ((unsigned char *) &res)[0];
        // reset when done
        reset_mcu();
    }
    else
    {
        eid = EID_ARG_INVALID;
    }

    return eid;
}


/*
 *
 */
static AT89S_EID
write_data ( Msg_Memmory_t * mem_msg_ptr )
{
    AT89S_EID eid = EID_OK;
    uint32_t i = 0, j = 0;
    uint32_t mcu_cmd = 0;

    uint32_t resp;

    if (mem_msg_ptr)
    {
        // start reprogram mode
        start_reprogram();
        // erase flash first
        erase_chip();

//        if (mem_msg_ptr->mode == PAGE_MODE)
//        {
//            mcu_cmd = WRITE_PAGE_MEM;
//        }
//        else // BYTE_MODE
//        {
//            mcu_cmd = WRITE_BYTE_MEM;
//        }

        // send data;
        for (i = 0; i < mem_msg_ptr->size || i < 256; ++i)
        {
            send_byte_cmd(0x50);
            send_byte_cmd(0x00);
//          send_byte_cmd(i);
            resp = send_byte_cmd(mem_msg_ptr->data[i]);
            for (j = 0; j < 1000; ++j);
//            delayMicroseconds(T_SWC);
//            snprintf(resp, sizeof(resp), "0x%08lx", mcu_cmd);
            Serial.print(resp);
            Serial.print("  ");
        }

        // flash data done, reset target MCU
        reset_mcu();


    }
    else
    {
        eid = EID_ARG_INVALID;
    }


    return eid;
}

/*
 *
 */
static AT89S_EID
read_data ( Msg_Memmory_t * mem_msg_ptr )
{
    AT89S_EID eid = EID_OK;
    uint32_t i = 0, j = 0;
    uint32_t mcu_cmd = 0;

    uint32_t resp;

    if (mem_msg_ptr)
    {
        // start reprogram mode
        start_reprogram();
//        if (mem_msg_ptr->mode == PAGE_MODE)
//        {
//            mcu_cmd = WRITE_PAGE_MEM;
//        }
//        else // BYTE_MODE
//        {
//            mcu_cmd = WRITE_BYTE_MEM;
//        }

        // send data;
        for (i = 0; i < 256; ++i)
        {
            resp = send_byte_cmd(0x20);
            resp = send_byte_cmd(0x00);
            resp = send_byte_cmd(i);
            resp = send_byte_cmd(0);

            mem_msg_ptr->data[i] = (uint8_t) resp;

            for (j = 0; j < 10000; ++j);

//            delayMicroseconds(T_SWC);
//            snprintf(resp, sizeof(resp), "0x%08lx", mcu_cmd);
            Serial.print(resp);
            Serial.print("  ");
        }

        // flash data done, reset target MCU
        reset_mcu();
    }
    else
    {
        eid = EID_ARG_INVALID;
    }


    return eid;
}


/*******************************************************************
 *
 *      ARDUINO SKELETON
 *
 ******************************************************************/
/*
 * Description:
 *
 * Input:
 *
 * Output:
 *
 */
void
setup ( void )
{
    Serial.begin(DEFAULT_BAUDRATE);

    // setup AT89s pin
    pinMode(PIN_RST, OUTPUT);
    pinMode(PIN_SCK, OUTPUT);
    pinMode(PIN_MOSI, OUTPUT);
    pinMode(PIN_MISO, INPUT);

    // reset pin voltage to default
    digitalWrite(PIN_RST, LOW);
    digitalWrite(PIN_SCK, HIGH);
    digitalWrite(PIN_MOSI, HIGH);
}


/*
 * Description:
 *
 * Input:
 *
 * Output:
 *
 */
void
loop ( void )
{
//    // get data from commander
//    int nbytes = Serial.available();
//    while (g_data_len < MSG_SIZE && nbytes > 0)
//    {
//        nbytes--;
//        g_data_buf[g_data_len++] = Serial.read();
//    }
//
//    // decode message from commander
//    if (g_data_len >= MSG_SIZE)
//    {
//        g_eid = decode_msg(g_data_buf, g_data_len, &g_at89msg);
//        if (g_eid == EID_OK)
//        {
//            g_eid = process_message(&g_at89msg);
//            if (g_eid == EID_OK)
//            {}
//        }
//
//        // reset value for next message
//        g_data_len = 0;
//    }

    char cmd;
    char resp[256] = { 0 };
    AT89S_EID eid = EID_NOK;
    Msg_Signature_t signature_msg;
    Msg_Memory_t memory_msg;

    static unsigned char sample_data[] = { 0x75, 0xA0, 0xFF, 0x12,
                                           0x00, 0x0E, 0x75, 0xA0,
                                           0x00, 0x12, 0x00, 0x0E,
                                           0x80, 0xF2, 0x78, 0xC8,
                                           0x79, 0xD2, 0x00, 0xD9,
                                           0xFD, 0xD8, 0xF9, 0x22, };

    if (Serial.available())
    {
        cmd = Serial.read();

        switch(cmd)
        {
            case 's':
                signature_msg.type = SIGN_DEV;
                eid = read_device_signature(&signature_msg);
                if (eid == EID_OK)
                {
                    snprintf(resp, sizeof(resp),
                             "%02x %02x %02x %02x",
                             signature_msg.signature[0],
                             signature_msg.signature[1],
                             signature_msg.signature[2],
                             signature_msg.signature[3]);
                    Serial.println(resp);
                }
                break;

            case 'w':
                memory_msg.size    = 0x18;
                memory_msg.address = 0x0000;
                memory_msg.rectype = 0x00;
                memory_msg.crc     = 0xF9;
                memcpy(memory_msg.data, sample_data, memory_msg.size);

                eid = write_data(&memory_msg);
                snprintf(resp, sizeof(resp), "\nWrite data %d !", eid);
                Serial.println(resp);

                break;

            case 'r':
                eid = read_data(&memory_msg);
                snprintf(resp, sizeof(resp), "\nRead data %d !", eid);
                Serial.println(resp);
                break;

            default:
                break;
        }

    }
}
