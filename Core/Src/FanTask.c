#include "FanTask.h"

//CAN variables
extern osMutexId_t ASCanSemHandle;
extern osMutexId_t EngCanSemHandle;

extern ASCANBuffer AutCanBuffer;
extern EngineCANBuffer EngCANBuffer;

extern QueueHandle_t canTxASQueue;

//Can message for sending state error to the pilot23
CAN_TxHeaderTypeDef header;
CANMessage msgFan;

extern TIM_HandleTypeDef htim2;

uint16_t waterTemp = 0;

void fanThread(void* argument) {
    
    TickType_t xLastWakeTime;
	const TickType_t xFrequency = 10;

    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3); // Start PWM on TIM2 channel 3 Radiator Fan

    while(1){

        if(xSemaphoreTake(EngCanSemHandle, (TickType_t) 0) == pdTRUE) {

            waterTemp = EngCANBuffer.WTS; // store the value of the water temperature sensor

            xSemaphoreGive(EngCanSemHandle);

            TIM2->CCR3 = waterTemp; // Set the PWM duty cycle to the value of the water temperature sensor
            
        }

    vTaskDelayUntil( &xLastWakeTime, xFrequency);
    }


}