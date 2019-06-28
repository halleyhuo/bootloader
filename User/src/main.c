/**********************************************************
 *
 * File :
 *		main.c
 *
 * Description:
 *		
 *
 */


#include "stm32f1xx_hal.h"
#include "usart.h"
#include "iap.h"

static void Error_Handler(void)
{
}

static void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    /** Initializes the CPU, AHB and APB busses clocks 
    */
    RCC_OscInitStruct.OscillatorType    = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState          = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue    = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState          = RCC_HSI_ON;
    RCC_OscInitStruct.LSIState          = RCC_LSI_ON;
    RCC_OscInitStruct.PLL.PLLState      = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource     = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL        = RCC_PLL_MUL6;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }
    
    /** Initializes the CPU, AHB and APB busses clocks 
    */
    RCC_ClkInitStruct.ClockType         = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                            |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource      = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider     = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider    = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider    = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }
    
    PeriphClkInit.PeriphClockSelection  = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_ADC;
    PeriphClkInit.RTCClockSelection     = RCC_RTCCLKSOURCE_LSI;
    PeriphClkInit.AdcClockSelection     = RCC_ADCPCLK2_DIV6;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
        Error_Handler();
    }
}


int main(void)
{

    IapFlagType             flag;
    IapRetVal               ret;

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();

    /* Init Usart2 with baudrate */
    USART_Init(USART_BAUDRATE);

    printf("Hello Boot Loader!\n\r");

    while(1)
    {
        flag = IAP_ReadFlag();

        if(flag == IAP_FLAG_RUN_APP)
        {
            printf("\r\n Run to app.\r\n");

            ret = IAP_RunApp();

            if(ret == IAP_ERROR)
            {
                printf("\r\n Run to app error.\r\n");
            }
        }
        else
        {
            printf("\r\n Update APP...\r\n");
            
            ret = IAP_UpdateApp();

            if(ret == IAP_OK)
            {
                IAP_WriteFlag(IAP_FLAG_RUN_APP);
            }
        }
    }
}



