#include "ErrHandASTask.h"


//ENG CAN

extern EngineCANBuffer EngCANBuffer;
extern osMutexId_t EngCanSemHandle;
//AS CAN
extern ASCANBuffer AutCanBuffer;
extern osMutexId_t ASCanSemHandle;
extern QueueHandle_t canTxASQueue;
extern osMutexId_t CanErrSemHandle;
extern uint8_t shutdownCMD;

//ADC buffer
extern osMutexId_t ADCSemHandle;
extern ADCBuffer adcReadings;

errorHandlerASThread(void* argument){
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = pdMS_TO_TICKS(200);
    
    
    HAL_GPIO_WritePin(SHUTDOWN_CMD_GPIO_Port, SHUTDOWN_CMD_Pin, SET);
    uint64_t lastHeartbit;
    uint8_t heartbitCount;
    CANMessage msg;
    CAN_TxHeaderTypeDef header;
    header.StdId = 400;
    header.ExtId = 0;
    header.IDE = 0;
    header.RTR = 0;
    header.DLC = 1;
    msg.header = header;
    msg.data[0] = 15;

    xLastWakeTime = xTaskGetTickCount();
    do{
        vTaskDelayUntil( &xLastWakeTime, xFrequency );

        if(xSemaphoreTake(CanErrSemHandle, (TickType_t) 0) == pdTRUE){
            // CheckCAN
            if(shutdownCMD){
                HAL_GPIO_WritePin(SHUTDOWN_CMD_GPIO_Port, SHUTDOWN_CMD_Pin, RESET);
                msg.data[1] = 0;
                xQueueSend(canTxASQueue, &msg, 0);
            }
            xSemaphoreGive(CanErrSemHandle);
        }

        // TODO Check RES status ->  no CAN data from RES for now
        /* FOR RES error logging
            msg.data[1] = 3;
            xQueueSend(canTxASQueue, &msg, 0);
        */

        if(xSemaphoreTake(ASCanSemHandle, (TickType_t) 0) == pdTRUE){
            // ReadHeartBit
            if(lastHeartbit == AutCanBuffer.Heartbit){
                if(++heartbitCount == 2){
                    HAL_GPIO_WritePin(SHUTDOWN_CMD_GPIO_Port, SHUTDOWN_CMD_Pin, RESET);
                    msg.data[1] = 1;
                    xQueueSend(canTxASQueue, &msg, 0);
                }
            }
            else{
                heartbitCount = 0;
            }
            lastHeartbit = AutCanBuffer.Heartbit;
            xSemaphoreGive(ASCanSemHandle);
        }

    
        if(xSemaphoreTake(ADCSemHandle, (TickType_t) 0) == pdTRUE){
            //GetEBSAirPressure
            if(adcReadings.EBSAir1 < 7.0 || adcReadings.EBSAir2 < 7.0){
                HAL_GPIO_WritePin(SHUTDOWN_CMD_GPIO_Port, SHUTDOWN_CMD_Pin, RESET);
                msg.data[1] = 2;
                xQueueSend(canTxASQueue, &msg, 0);
            }
            xSemaphoreGive(ADCSemHandle);
        }
    }
    while(HAL_GPIO_ReadPin(SHUTDOWN_SENSE_GPIO_Port, SHUTDOWN_SENSE_Pin));
    //STOP TASK
}