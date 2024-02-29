#include "FansContrTask.h"

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


void fansContrThread(void* argument) {
    
    TickType_t xLastWakeTime;
	const TickType_t xFrequency = 10;

    uint8_t waterTemp = 0;
    uint8_t airTemp = 0;

    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3); // Start PWM on TIM2 channel 3 Radiator Fan
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4); // Start PWM on TIM2 channel 4 Air Fan

    xLastWakeTime = xTaskGetTickCount(); // rate of execution

    while(1){

        if(xSemaphoreTake(EngCanSemHandle, (TickType_t) 0) == pdTRUE) {
            // ReadTempData
            waterTemp = EngCANBuffer.WTS; // store the value of the water temperature sensor
            airTemp = EngCANBuffer.ATS; // store the value of the air temperature senso

            xSemaphoreGive(EngCanSemHandle);

            //HandleFanSpeed
            if (waterTemp < 80){
                TIM2->CCR3 = 0; // Stop the Fan
            }
            else if (waterTemp >= 80){
                TIM2->CCR3 = (waterTemp-10); // Set the PWM duty cycle to the value of the water temperature sensor
            }

            if (airTemp < 50){
                TIM2->CCR4 = 0; // Stop the Fan
            }
            else if (airTemp >= 50){
                TIM2->CCR4 = (airTemp+10); // Set the PWM duty cycle to the value of the air temperature sensor
            }
        }

        vTaskDelayUntil( &xLastWakeTime, xFrequency);
    }
}