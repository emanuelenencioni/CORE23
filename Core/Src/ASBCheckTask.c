#include "AccTask.h"

// ENG CAN
extern EngineCANBuffer EngCANBuffer;
extern osMutexId_t EngCanSemHandle;
//AS CAN
extern ASCANBuffer AutCanBuffer;
extern osMutexId_t ASCanSemHandle;
extern QueueHandle_t canTxASQueue;


//ADC buffer
extern osMutexId_t ADCSemHandle;
extern ADCBuffer adcReadings;

extern osMutexId_t ASBCheckSemHandle;
//in critical section
uint8_t checkedASB;

void ASBCheckThread(void* argument) {
    CAN_TxHeaderTypeDef header;
    CANMessage msg;
    uint16_t rpm = 0;
    float EBSAir1 = 0;
    float EBSAir2 = 0;
    uint8_t BrakePressure = 0;
    header.StdId = 400;
    header.ExtId = 0;
    header.IDE = 0;
    header.RTR = 0;
    header.DLC = 1;
    msg.header = header;
    msg.data[0] = 14;
    while(HAL_GPIO_ReadPin(SHUTDOWN_SENSE_GPIO_Port, SHUTDOWN_SENSE_Pin) == RESET){
        vTaskDelay(200);
    }
    
    if(xSemaphoreTake(ADCSemHandle, (TickType_t) 0)) {
        //ReadEBSPressure
        EBSAir1 = adcReadings.EBSAir1;
        EBSAir2 = adcReadings.EBSAir2;
        xSemaphoreGive(ADCSemHandle);
    }

    if(xSemaphoreTake(ASCanSemHandle, (TickType_t) 0)){
        BrakePressure = AutCanBuffer.brakePressure;
        xSemaphoreGive(ASCanSemHandle);
    }

    if(EBSAir1 < 7.0 || EBSAir2 < 7.0 || BrakePressure < 0 || BrakePressure > 3){
        HAL_GPIO_WritePin(SHUTDOWN_CMD_GPIO_Port, SHUTDOWN_CMD_Pin, RESET);
        msg.data[1] = 0;
        xQueueSend(canTxASQueue, &msg, 0);
    }
    
    do{
        if(xSemaphoreTake(EngCanSemHandle, (TickType_t) 0)){
            rpm = EngCANBuffer.RPM;
        }
        xSemaphoreGive(EngCanSemHandle);
        vTaskDelay(100);
    }while(rpm <= MIN_RPM_ENG_ON);

    
    //TODO problemi con EBS try


    if(xSemaphoreTake(ASBCheckSemHandle, (TickType_t) 0)){
        checkedASB = 1;
        xSemaphoreGive(ASBCheckSemHandle);
    }
    vTaskSuspend(NULL);
}