#include "ADCTask.h"

extern ADC_HandleTypeDef hadc1;
extern osMutexId_t ADCSemHandle;
// Define the ADC buffer for DMA
volatile uint16_t adcBuffer[ADC_BUFFER_SIZE];
extern 
// Define the ADC buffer in critical section
uint16_t adcReadings[ADC_BUFFER_SIZE];

// ADC task function
void ADCThread(void* argument) {
    
    TickType_t xLastWakeTime;
	const TickType_t xFrequency = 100;

    HAL_ADC_Start_DMA(&hadc1, (uint16_t*)adcBuffer, sizeof(adcBuffer)/sizeof(adcBuffer[0]));


    xLastWakeTime = xTaskGetTickCount();
    while (1) {
        vTaskDelayUntil( &xLastWakeTime, xFrequency);
        /* Infinite loop */

            if(xSemaphoreTake(ADCSemHandle, (TickType_t) 0) == pdTRUE) {
                for(int i=0;i<12;i++) {
                    adcReadings[i] = adcBuffer[i];
                }
                xSemaphoreGive(ADCSemHandle);
        }
    }
}

// https://electronics.stackexchange.com/questions/581003/stm32h7-adc-with-dma-reading-only-zeros-using-hal-and-freertos