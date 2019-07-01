/**********************************************************
 *
 * File :
 *		YModem.h
 *
 * Description:
 *		YModem protocal (use statemachine)
 *
 */


#ifndef __YMODEM_H__
#define __YMODEM_H__

#include "stm32f1xx_hal.h"

/*
 * Receive Frame return values
 */

typedef int32_t                     YmodemRetVal;

#define YMODEM_OK                   0       /* OK */
#define YMODEM_ERROR                -1      /* ERROR */
#define YMODEM_TIMEOUT              -2      /* Receive timeout */
#define YMODEM_PARAM_ERR            -3      /* Parameters error */
#define YMODEM_ABORT                -4      /* Remote abort */

YmodemRetVal YmodemReceive(uint32_t addrSaveData, uint32_t * savedSize);

#endif //__YMODEM_H__

