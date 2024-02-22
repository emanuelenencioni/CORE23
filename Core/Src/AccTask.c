#include "AccTask.h"

//adc buffer from dma
extern uint16_t adcReadings[ADC_BUFFER_SIZE];

//CAN variables
extern osMutexId_t ASCanSemHandle;
extern osMutexId_t EngCanSemHandle;

extern ASCANBuffer AutCanBuffer;
extern EngineCANBuffer EngCANBuffer;

extern QueueHandle_t canTxASQueue;

extern osMutexId_t ADCSemHandle;

//Can message for sending state error to the pilot23

CANMessage msgAcc;

// Acc task function
void accThread(void* argument) {
    CAN_TxHeaderTypeDef header;
    TickType_t xLastWakeTime;
	const TickType_t xFrequency = 100;
    uint32_t timer = 0;
    uint16_t apps1 = 0;
    uint16_t apps2 = 0;

    // Settings for can message
    header.StdId = 400;
    header.ExtId = 0;
    header.IDE = 0;
    header.RTR = 0;
    header.DLC = 1;
    msgAcc.header = header;
    

    xLastWakeTime = xTaskGetTickCount();
    while (1) {
        vTaskDelayUntil( &xLastWakeTime, xFrequency);
        /* Infinite loop */

            if(xSemaphoreTake(ADCSemHandle, (TickType_t) 0) == pdTRUE) {
                
                apps1 = adcReadings[APPS1];
                apps2 = adcReadings[APPS2];

                xSemaphoreGive(ADCSemHandle);
                
                DAC->DHR12R2 = (apps1+apps2)/2; // Set DAC channel 2 value to average of APPS1 and APPS2
                //DAC->DHR12R1: channel 1 12-bit right-aligned data holding register
                //DHR12R2: channel 2 12-bit right-aligned data holding register
                // Channel 1 is connected to VPPM and Channel 2 is connected to APPS
                
                
                // check plausibility APPS
                if (abs(apps1 - apps2) > MAX_DEVIATION) {
                    timer++;
                    if (timer > 10) { // 100ms implausibility
                        HAL_GPIO_WritePin(SHUTDOWN_CMD_GPIO_Port, SHUTDOWN_CMD_Pin, GPIO_PIN_RESET); // Shutdown OFF
                        msgAcc.data[0] = 13; // Error code
                        xQueueSend(canTxASQueue, &msgAcc, 0); // Send error message
                    }
                }
                else {
                    timer = 0;
                }

                //do something with adcReadings
                
        }
    }
}