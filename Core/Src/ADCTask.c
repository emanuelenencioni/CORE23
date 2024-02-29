#include "ADCTask.h"


/**
 * ADCBuffer sensors:
 * - 4: EBS Air 2
 * - 5: EBS Air 1
 * - 
 */

extern ADC_HandleTypeDef hadc1;
extern osMutexId_t ADCSemHandle;
// Define the ADC buffer for DMA

// Define the ADC buffer in critical section
ADCBuffer adcReadings;

//FOR TESTING
extern CAN_HandleTypeDef hcan2;
extern TIM_HandleTypeDef htim5;
extern osThreadId_t CanHandlerTaskHandle;

// ADC task function
void ADCThread(void* argument) {
    
    TickType_t xLastWakeTime;
	const TickType_t xFrequency = 100;
    volatile uint16_t adcBuffer[ADC_BUFFER_SIZE];
    HAL_ADC_Start_DMA(&hadc1, (uint16_t*)adcBuffer, sizeof(adcBuffer)/sizeof(adcBuffer[0]));
    
    osDelay(5000);
    HAL_TIM_Base_Start(&htim5);
    //Task deactivation for get times.
    vTaskSuspend(CanHandlerTaskHandle);


    uint32_t start_time = 0;
    uint32_t finish_time = 0;
    uint16_t count = 0;
    uint32_t avg = 0;
    xLastWakeTime = xTaskGetTickCount();
    while (1) {
        vTaskDelayUntil( &xLastWakeTime, xFrequency);
        /* Infinite loop */

            if(xSemaphoreTake(ADCSemHandle, (TickType_t) 0) == pdTRUE) {
                //ReadADCWriteADCBuffer
                start_time = TIM5->CNT;
                //TODO da checkare gli indirizzi del buffer
                adcReadings.EBSAir2 = map(adcBuffer[4], 0, 4096, 0, 100); // TODO vedere fondo scala sensori EBS
                adcReadings.EBSAir1 = map(adcBuffer[5], 0, 4096, 0, 100); //EBS_AIR2
                adcReadings.ADC_AUX1 = map(adcBuffer[6], 0, 4096, 0, 100); //TODO finire mapping valori ADC
                adcReadings.ADC_AUX2 = map(adcBuffer[7], 0, 4096, 0, 100);
                adcReadings.BPS = map(adcBuffer[8], 0, 4096, 0, 100); //TODO finire mapping valori ADC
                adcReadings.APPS1 = map(adcBuffer[9], 0, 4096, 0, 100); //TODO finire mapping valori ADC
                adcReadings.APPS2 = map(adcBuffer[10], 0, 4096, 0, 100);
                adcReadings.VPPMSense = map(adcBuffer[11], 0, 4096, 0, 100); //TODO finire mapping valori ADC
                adcReadings.GearUpAir = map(adcBuffer[12], 0, 4096, 0, 100); //TODO finire mapping valori ADC
                adcReadings.clutchOil = map(adcBuffer[13], 0, 4096, 0, 100); //TODO finire mapping valori ADC
                adcReadings.desmo1 = map(adcBuffer[14], 0, 4096, 0, 100); //TODO finire mapping valori ADC
                adcReadings.desmo2 = map(adcBuffer[15], 0, 4096, 0, 100);

                //TODO finire mapping valori ADC
                finish_time = TIM5->CNT;
                xSemaphoreGive(ADCSemHandle);
        }
        
        if(count < 10){
            avg += finish_time - start_time;
            count++;
        }
        else{
            uint32_t TxMailbox;
            avg /= 10;
            CANMessage msg;
            CAN_TxHeaderTypeDef header;
            // Settings for can message
            header.StdId = 400;
            header.ExtId = 0;
            header.IDE = 0;
            header.RTR = 0;
            header.DLC = 4;
            msg.header = header;
            msg.data[0] = (uint8_t) (avg >> 24);
            msg.data[1] = (uint8_t) (avg >> 16);
            msg.data[2] = (uint8_t) (avg >> 8);
            msg.data[3] = (uint8_t) avg;
            HAL_CAN_AddTxMessage(&hcan2, &msg.header, msg.data, &TxMailbox);
            vTaskSuspend(NULL);
        }
    }
}

// https://electronics.stackexchange.com/questions/581003/stm32h7-adc-with-dma-reading-only-zeros-using-hal-and-freertos

float map(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}