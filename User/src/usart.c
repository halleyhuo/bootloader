/**********************************************************
 *
 * File :
 *      usart.c
 *
 * Description:
 *      UART uitilty functions.
 *
 */

#include "stm32f1xx_hal.h"
#include "usart.h"

static UART_HandleTypeDef UART_Hander;

UsartRetVal USART_Init(int baudrate)
{
    UsartRetVal                     ret = USART_ERROR;

    UART_Hander.Instance            = USART_PORT;
    UART_Hander.Init.BaudRate       = baudrate;
    UART_Hander.Init.WordLength     = UART_WORDLENGTH_8B;
    UART_Hander.Init.StopBits       = UART_STOPBITS_1;
    UART_Hander.Init.Parity         = UART_PARITY_NONE;
    UART_Hander.Init.Mode           = UART_MODE_TX_RX;
    UART_Hander.Init.HwFlowCtl      = UART_HWCONTROL_NONE;
    UART_Hander.Init.OverSampling   = UART_OVERSAMPLING_16;

    if (HAL_UART_Init(&UART_Hander) == HAL_OK)
    {
        ret = USART_OK;
    }

    return ret;
}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

    GPIO_InitTypeDef GPIO_InitStruct = {0};


    if(uartHandle->Instance == USART1)
    {
        /* USART1 clock enable */
        __HAL_RCC_USART1_CLK_ENABLE();

        __HAL_RCC_GPIOA_CLK_ENABLE();

        /*
         * USART1 GPIO Configuration
         * PA9     ------> USART1_TX
         * PA10     ------> USART1_RX 
         */
        GPIO_InitStruct.Pin = GPIO_PIN_9;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_10;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        HAL_NVIC_DisableIRQ(USART1_IRQn);
    }
    else if(uartHandle->Instance == USART2)
    {
        /* USART2 clock enable */
        __HAL_RCC_USART2_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();

        /*
         * USART2 GPIO Configuration
         * PA2     ------> USART2_TX
         * PA3     ------> USART2_RX 
         */
        GPIO_InitStruct.Pin     = GPIO_PIN_2;
        GPIO_InitStruct.Mode    = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed   = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        GPIO_InitStruct.Pin     = GPIO_PIN_3;
        GPIO_InitStruct.Mode    = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull    = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        HAL_NVIC_DisableIRQ(USART2_IRQn);               // Use usart2 with none-interrupt mode

    }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

    if(uartHandle->Instance == USART1)
    {
        /* Peripheral clock disable */
        __HAL_RCC_USART1_CLK_DISABLE();

        /*
         * USART1 GPIO Configuration
         * PA9     ------> USART1_TX
         * PA10     ------> USART1_RX 
         */
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);
    }
    else if(uartHandle->Instance == USART2)
    {
        /* Peripheral clock disable */
        __HAL_RCC_USART2_CLK_DISABLE();
      
        /*
         * USART2 GPIO Configuration
         * PA2     ------> USART2_TX
         * PA3     ------> USART2_RX 
         */
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3);
    }
}


UsartRetVal USART_SendData(uint8_t * data, uint32_t len, uint32_t timeout)
{
    UsartRetVal             ret;
    HAL_StatusTypeDef       status;

    status = HAL_UART_Transmit(&UART_Hander, data, len, timeout);

    switch(status)
    {
        case HAL_OK:
            ret = 0;
            break;

        case HAL_TIMEOUT:
            ret = USART_TIMEOUT;
            break;

        case HAL_ERROR:
        case HAL_BUSY:
        default:
            ret = USART_ERROR;
            break;
    }

    return ret;
}

UsartRetVal USART_Receive(uint8_t *data, uint32_t dataLen, uint32_t timeout)
{
    UsartRetVal             ret;
    HAL_StatusTypeDef       status;

    status = HAL_UART_Receive(&UART_Hander, data, dataLen, timeout);
    switch(status)
    {
        case HAL_OK:
            ret = USART_OK;
            break;

        case HAL_TIMEOUT:
            ret = USART_TIMEOUT;
            break;

        case HAL_ERROR:
        case HAL_BUSY:
        default:
            ret = USART_ERROR;
            break;
    }

    return ret;
}

/* re-write fputc() */
int fputc(int c, FILE *stream)
{
    HAL_UART_Transmit(&UART_Hander, (unsigned char *)&c, 1, 1000);
    return 1;
}


