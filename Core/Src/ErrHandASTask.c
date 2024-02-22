#include "ErrHandASTask.h"

extern osMutexId_t ASCanSemHandle;
extern osMutexId_t EngCanSemHandle;

extern ASCANBuffer AutCanBuffer;
extern EngineCANBuffer EngCANBuffer;
extern osThreadId_t ASStateHandTaskHandle;
extern QueueHandle_t canTxASQueue;

extern osMutexId_t CanErrSemHandle;
extern uint8_t shutdownCMD;



errorHandlerASThread(void* argument){
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = pdMS_TO_TICKS(200);
    
    HAL_GPIO_WritePin(SHUTDOWN_CMD_GPIO_Port, SHUTDOWN_CMD_Pin, SET);
    uint64_t lastHeartbit;
    uint8_t heartbitCount;

    xLastWakeTime = xTaskGetTickCount();
    do{
        vTaskDelayUntil( &xLastWakeTime, xFrequency );

        if(xSemaphoreTake(CanErrSemHandle, (TickType_t) 0) == pdTRUE){
            // CheckCAN
            if(shutdownCMD){
                HAL_GPIO_WritePin(SHUTDOWN_CMD_GPIO_Port, SHUTDOWN_CMD_Pin, RESET);
            }
            xSemaphoreGive(CanErrSemHandle);
        }

        // TODO Check RES status ->  no CAN data from RES for now

        if(xSemaphoreTake(ASCanSemHandle, (TickType_t) 0) == pdTRUE){
            // ReadHeartBit
            if(lastHeartbit == AutCanBuffer.Heartbit){
                if(++heartbitCount == 2){
                    HAL_GPIO_WritePin(SHUTDOWN_CMD_GPIO_Port, SHUTDOWN_CMD_Pin, RESET);
                }
            }
            else{
                heartbitCount = 0;
            }
            lastHeartbit = AutCanBuffer.Heartbit;
            xSemaphoreGive(ASCanSemHandle);
        }

        // TODO get EBS Air pressure
    }
    while(HAL_GPIO_ReadPin(SHUTDOWN_SENSE_GPIO_Port, SHUTDOWN_SENSE_Pin));
}