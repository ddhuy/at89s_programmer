/*******************************************************************
 *
 *      HEADERS
 *
 ******************************************************************/
#include "at89s.h"




/*******************************************************************
 *
 *      PREPROCESSOR & CONSTANT
 *
 ******************************************************************/




/*******************************************************************
 *
 *      DATA TYPE
 *
 ******************************************************************/
typedef  AT89S_EID  (*Decoder) ( char* data_buf, 
                                 int data_len,
                                 AT89S_Msg_t* atmsg );
typedef struct MsgDecoder_t
{
    uint8_t msgtype;
    Decoder msgdec_fptr;

} MsgDecoder;



/*******************************************************************
 *
 *      PRIVATE FUNCTION DELCARATION
 *
 ******************************************************************/
static AT89S_EID
dec_c_rsign ( char* data_buf,
              int data_len,
              AT89S_Msg_t* atmsg );

static AT89S_EID
dec_r_rsign ( char* data_buf,
              int data_len,
              AT89S_Msg_t* atmsg );

static AT89S_EID
dec_c_wmem ( char* data_buf,
             int data_len,
             AT89S_Msg_t* atmsg );

static AT89S_EID
dec_r_wmem ( char* data_buf,
             int data_len,
             AT89S_Msg_t* atmsg );

/*******************************************************************
 *
 *      FILE VARIABLE
 *
 ******************************************************************/
MsgDecoder  msg_decoders[] = 
{
    { CMD_W_MEM, dec_c_wmem },
    { CMD_R_MEM, NULL },
    { CMD_E_MEM, NULL },
    { CMD_R_SIG, dec_c_rsign },
    { CMD_R_LBIT, NULL },
    { CMD_W_LBIT, NULL },

    { RES_W_MEM, dec_r_wmem },
    { RES_R_MEM, NULL },
    { RES_E_MEM, NULL },
    { RES_R_SIG, dec_r_rsign },
    { RES_R_LBIT, NULL },
    { RES_W_LBIT, NULL },

    { CMD_NULL, NULL },
};





/*******************************************************************
 *
 *      FUNCTION DEFINITION
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
AT89S_EID
decode_msg ( char* data_buf,
             int   data_len,
             AT89S_Msg_t* atmsg )
{
    MsgDecoder* msgdec_ptr = NULL;
    AT89S_EID eid = EID_NOK;
    int len = 0;

    if (atmsg == NULL || data_buf == NULL)
        return EID_ARG_NULL;

    if (data_len < 2)
        return EID_CMD_LEN;

    // find message decoder
    for (msgdec_ptr = &msg_decoders[0];
         msgdec_ptr->msgtype != CMD_NULL;
         msgdec_ptr++)
    {
        if (msgdec_ptr->msgtype == data_buf[0])
            break;
    }

    if (msgdec_ptr->msgtype == CMD_NULL)
    {
        eid = EID_DEC_NOT_FOUND;
    }
    else if (msgdec_ptr->msgdec_fptr != NULL)
    {
        // decode message header
        atmsg->msgt = data_buf[len++];
        // decode message content
        eid = msgdec_ptr->msgdec_fptr(data_buf, len, atmsg);
    }

    return eid;
}


/*
 * Description:
 *
 * Input:
 *
 * Output:
 *
 */
static AT89S_EID
dec_c_rsign ( char* data_buf,
              int data_len,
              AT89S_Msg_t* atmsg )
{
    if (data_buf == NULL || atmsg == NULL)
        return EID_ARG_NULL;

    atmsg->data.msg_signature.type = data_buf[data_len++];

    return EID_OK;
}


/*
 * Description:
 *
 * Input:
 *
 * Output:
 *
 */
static AT89S_EID
dec_r_rsign ( char* data_buf,
              int data_len,
              AT89S_Msg_t* atmsg )
{
    if (data_buf == NULL || atmsg == NULL)
        return EID_ARG_NULL;

    atmsg->data.msg_signature.type         = data_buf[data_len++];
    atmsg->data.msg_signature.signature[0] = data_buf[data_len++];
    atmsg->data.msg_signature.signature[1] = data_buf[data_len++];
    atmsg->data.msg_signature.signature[2] = data_buf[data_len++];

    return EID_OK;
}

/*
 * Description:
 *
 * Input:
 *
 * Output:
 *
 */
static AT89S_EID
dec_c_wmem ( char* data_buf,
             int data_len,
             AT89S_Msg_t* atmsg )
{
    int i = 0;

    if (data_buf == NULL || atmsg == NULL)
        return EID_ARG_NULL;

    atmsg->data.msg_memory.address   = ((data_buf[data_len++] & 0x00FF) << 8);
    atmsg->data.msg_memory.address  |= (data_buf[data_len++] & 0x00FF);
    atmsg->data.msg_memory.mode       = data_buf[data_len++];
    atmsg->data.msg_memory.memtype   = data_buf[data_len++];
    atmsg->data.msg_memory.rectype   = data_buf[data_len++];
    atmsg->data.msg_memory.crc       = data_buf[data_len++];
    atmsg->data.msg_memory.size      = data_buf[data_len++];

    for (i = 0; i < atmsg->data.msg_memory.size; ++i)
        atmsg->data.msg_memory.data[i] = data_buf[i + data_len];
    data_len += atmsg->data.msg_memory.size;

    return EID_OK;
}

/*
 * Description:
 *
 * Input:
 *
 * Output:
 *
 */
static AT89S_EID
dec_r_wmem ( char* data_buf,
             int data_len,
             AT89S_Msg_t* atmsg )
{
    if (data_buf == NULL || atmsg == NULL)
        return EID_ARG_NULL;

    if (data_len < 0)
        return EID_CMD_LEN;

    return EID_OK;
}
