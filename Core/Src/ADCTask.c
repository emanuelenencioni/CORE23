#include "ADCTask.h"


/**
 * ADCBuffer sensors (same order as IOC):
 * - 0: Desmo1
 * - 1: Cluch oil
 * - 2: Gear up air
 * - 3: VPPM sense
 * - 4: EBS Air 2
 * - 5: EBS Air 1
 * - 6: APPS2
 * - 7: BPS
 * - 8: AUX2
 * - 9: Desmo2
 * - 10: APPS1, 11 seems connected too.
 * - 11: AUX1
 */

extern ADC_HandleTypeDef hadc1;
extern osMutexId_t ADCSemHandle;
// Define the ADC buffer for DMA

// Define the ADC buffer in critical section
ADCBuffer adcReadings;

// ADC task function
void ADCThread(void* argument) {
    
    TickType_t xLastWakeTime;
	const TickType_t xFrequency = pdMS_TO_TICKS(100);
    volatile uint16_t adcBuffer[ADC_BUFFER_SIZE];
    HAL_ADC_Start_DMA(&hadc1, (uint16_t*)adcBuffer, sizeof(adcBuffer)/sizeof(adcBuffer[0]));
    
    xLastWakeTime = xTaskGetTickCount();
    while (1) {
        vTaskDelayUntil( &xLastWakeTime, xFrequency);
        /* Infinite loop */

            if(xSemaphoreTake(ADCSemHandle, (TickType_t) 0) == pdTRUE) {
                //ReadADCWriteADCBuffer
                adcReadings.EBSAir2 = map(adcBuffer[4], 0, 4096, -161, 19232) < 0 ? 0 : map(adcBuffer[4], 0, 4096, -161, 19232) / 100; // dBar 
                adcReadings.EBSAir2 = map(adcBuffer[4], 0, 4096, -161, 19232) < 0 ? 0 : map(adcBuffer[4], 0, 4096, -161, 19232) / 100; // dBar
                adcReadings.ADC_AUX1 = map(adcBuffer[11], 0, 4096, 0, 100); //TODO finire mapping valori ADC
                adcReadings.ADC_AUX2 = map(adcBuffer[8], 0, 4096, 0, 100);
                
                //BOSCH APPS 0-5 V linear operating
                adcReadings.APPS1 = map(adcBuffer[10], 0, 4096, 0, 100);
                adcReadings.APPS2 = map(adcBuffer[6], 0, 4096, 0, 100);

                //Bourns APPS
                adcReadings.APPS3 = map(adcBuffer[10], 0, 4096, 0, 100); //TODO add apps3 in mx, find range testing it.

                adcReadings.VPPMSense = map(adcBuffer[3], 0, 4096, 0, 100); //TODO finire mapping valori ADC
                adcReadings.GearUpAir = map(adcBuffer[2], 0, 4096, 0, 100); //TODO finire mapping valori ADC
                adcReadings.clutchOil = map(adcBuffer[1], 0, 4096, 0, 100); //TODO finire mapping valori ADC
                adcReadings.desmo1 = map(adcBuffer[0], 0, 4096, 0, 100); //TODO finire mapping valori ADC
                adcReadings.desmo2 = map(adcBuffer[9], 0, 4096, 0, 100);

                xSemaphoreGive(ADCSemHandle);
        }
        
        
    }
}

// https://electronics.stackexchange.com/questions/581003/stm32h7-adc-with-dma-reading-only-zeros-using-hal-and-freertos

uint32_t map(uint32_t x, uint32_t in_min, uint32_t in_max, uint32_t out_min, uint32_t out_max) { // TODO to test
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}