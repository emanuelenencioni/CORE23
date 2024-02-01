#include "FreeRTOS.h"
#include "task.h"
#include "stm32f7xx_hal.h"

#define ADC_BUFFER_SIZE 12

// Define the ADC buffer
uint16_t adcBuffer[ADC_BUFFER_SIZE];
// Define the ADC buffer
uint16_t adcReadings[ADC_BUFFER_SIZE];

// ADC task function
void ADCTask(void *pvParameters)
{
    while (1)
    {
        void vAnalogRead(void *argument)
        {
        /* Infinite loop */

            xSemaphoreTake(xsemaph_adc, portMAX_DELAY);
            for(int i=0;i<12;i++)
            {
                adcReadings[i] = adcBuffer[i];
            }
            xSemaphoreGive(xsemaph_adc);
        }
        /* USER CODE END StartLeAnalog */
        
        // Delay for 50ms
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

// Create the ADC task
void CreateADCTask()
{
    xTaskCreate(ADCTask, "ADCTask", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
}

https://electronics.stackexchange.com/questions/581003/stm32h7-adc-with-dma-reading-only-zeros-using-hal-and-freertos