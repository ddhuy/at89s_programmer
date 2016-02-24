#ifndef _HEX_IO_H_
#define _HEX_IO_H_

/*******************************************************************
 *
 *      HEADERS
 *
 ******************************************************************/

#include "at89s_types.h"



/*******************************************************************
 *
 *      API DECLARATION
 *
 ******************************************************************/
/*
 *
 */
AT89S_EID
hexio_readall ( char* filename,
                unsigned char** data_pptr,
                int* data_len_ptr );


#endif /* _HEX_IO_H_ */
