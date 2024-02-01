#include "FreeRTOS.h"
#include "task.h"
#include "stm32f7xx_hal.h"

#define ADC_BUFFER_SIZE 10

// Define the ADC buffer
uint16_t adcBuffer[ADC_BUFFER_SIZE];

// ADC task function
void ADCTask(void *pvParameters)
{
    while (1)
    {
        if(xSemaphoreTake(ASCanSemHandle, (TickType_t) WAIT_FOR_PILOT_STATE) == pdTRUE){

            // Read all ADC channels
            for (uint8_t channel = 0; channel < ADC_BUFFER_SIZE; channel++)
            {
                // Start ADC conversion for the current channel
                HAL_ADC_Start(&hadc1);

                yield();
                
                // Wait for ADC conversion to complete
                HAL_ADC_PollForConversion(&hadc1, 1);
                
                // Read the ADC value for the current channel
                adcBuffer[channel] = HAL_ADC_GetValue(&hadc1);

                HAL_ADC_Stop(&hadc1);
            }
        }
        // Pass the ADC values to other tasks
        
        // Delay for 50ms
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

// Create the ADC task
void CreateADCTask()
{
    xTaskCreate(ADCTask, "ADCTask", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
}
