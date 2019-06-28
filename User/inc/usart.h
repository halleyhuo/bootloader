/**********************************************************
 *
 * File :
 *      usart.h
 *
 * Description:
 *      Header file. UART uitilty functions.
 *
 */

#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"

#define USART_PORT                  USART2

#define USART_BAUDRATE              115200

#define USART_MAX_TIMEOUT           0xFFFF
#define USART_TRANS_TIMEOUT         1000


/* Return value types for flash functions */
typedef int32_t                     UsartRetVal;

#define USART_OK                    0
#define USART_ERROR                 -1
#define USART_PARAMETERS_ERR        -2
#define USART_TIMEOUT               -3


UsartRetVal USART_Init(int baudrate);

UsartRetVal USART_SendData(uint8_t * data, uint32_t len, uint32_t timeout);

UsartRetVal USART_Receive(uint8_t *data, uint32_t dataLen, uint32_t timeout);


#ifdef __cplusplus
}
#endif

#endif

