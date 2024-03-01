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

//FOR TESTING
extern CAN_HandleTypeDef hcan2;
extern TIM_HandleTypeDef htim5;

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

    //FOR TESTING
    HAL_TIM_Base_Start(&htim5);
    uint32_t start_time = 0;
    uint32_t finish_time = 0;
    uint16_t count = 0;
    uint32_t avg = 0;
    uint8_t avg_count = 100;

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
        if(count < avg_count){
            avg += finish_time - start_time;
            count++;
        }
        else{
            uint32_t TxMailbox = 0;
            avg /= avg_count;
            CANMessage msg;
            CAN_TxHeaderTypeDef header_test;
            // Settings for can message
            header_test.StdId = 400;
            header_test.ExtId = 0;
            header_test.IDE = 0;
            header_test.RTR = 0;
            header_test.DLC = 4;
            msg.header = header_test;
            msg.data[0] = (uint8_t) (avg >> 24);
            msg.data[1] = (uint8_t) (avg >> 16);
            msg.data[2] = (uint8_t) (avg >> 8);
            msg.data[3] = (uint8_t) avg;
            HAL_CAN_AddTxMessage(&hcan2, &msg.header, msg.data, &TxMailbox);
            vTaskSuspend(NULL);
        }

    }
    while(HAL_GPIO_ReadPin(SHUTDOWN_SENSE_GPIO_Port, SHUTDOWN_SENSE_Pin));

    
    //STOP TASK
}