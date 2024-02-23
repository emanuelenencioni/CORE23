#include "ASBCheckTask.h"

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


//in critical section
uint8_t checkedASB;
extern osMutexId_t ASBCheckSemHandle;

extern osThreadId_t ErrHandASTaskHandle;

void ASBCheckThread(void* argument) {
    CAN_TxHeaderTypeDef header;
    CAN_TxHeaderTypeDef headerPC;
    CANMessage msg;
    CANMessage pcMsg;

    uint16_t rpm = 0;
    float EBSAir1 = 0;
    float EBSAir2 = 0;
    uint8_t BrakePressureFront = 0;
    uint8_t BrakePressureBack = 0;
    header.StdId = 400;
    header.ExtId = 0;
    header.IDE = 0;
    header.RTR = 0;
    header.DLC = 1;
    msg.header = header;
    msg.data[0] = 14;

    headerPC.StdId = 350;
    headerPC.ExtId = 0;
    headerPC.IDE = 0;
    headerPC.RTR = 0;
    headerPC.DLC = 1;
    pcMsg.header = header;
    pcMsg.data[0] = 0;
    
    while(HAL_GPIO_ReadPin(SHUTDOWN_SENSE_GPIO_Port, SHUTDOWN_SENSE_Pin) == RESET){
        vTaskDelay(200);
    }
    
    if(xSemaphoreTake(ADCSemHandle, portMAX_DELAY)) {
        //ReadEBSAirPressure
        EBSAir1 = adcReadings.EBSAir1;
        EBSAir2 = adcReadings.EBSAir2;
        xSemaphoreGive(ADCSemHandle);
    }

    if(xSemaphoreTake(ASCanSemHandle, portMAX_DELAY)){
        //ReadBrakePressure
        BrakePressureFront = AutCanBuffer.brakePressureFront;
        BrakePressureBack = AutCanBuffer.brakePressureBack;
        xSemaphoreGive(ASCanSemHandle);
    }

    if(EBSAir1 < 7.0 || EBSAir2 < 7.0 || BrakePressureFront < 0 || BrakePressureFront > 3 || BrakePressureBack < 0 || BrakePressureBack > 3){
        HAL_GPIO_WritePin(SHUTDOWN_CMD_GPIO_Port, SHUTDOWN_CMD_Pin, RESET);
        msg.data[1] = 0;
        xQueueSend(canTxASQueue, &msg, 0);
        vTaskSuspend(NULL);
    }
    do{
        if(xSemaphoreTake(EngCanSemHandle, portMAX_DELAY)){
            rpm = EngCANBuffer.RPM;
        }
        xSemaphoreGive(EngCanSemHandle);
        vTaskDelay(100);
    }while(rpm <= MIN_RPM_ENG_ON);

    
    //TurnOffEBSValves
    HAL_GPIO_WritePin(QM_TRIGGER_GPIO_Port, QM_TRIGGER_Pin, SET);
    vTaskDelay(50);

    if(xSemaphoreTake(ASCanSemHandle, portMAX_DELAY)){
        //ReadBrakePressure
        BrakePressureFront = AutCanBuffer.brakePressureFront;
        BrakePressureBack = AutCanBuffer.brakePressureBack;
        xSemaphoreGive(ASCanSemHandle);
    }

    if(!(BrakePressureFront > 10 && BrakePressureBack > 10 && BrakePressureFront < 40  && BrakePressureBack < 40)){ // TODO find EBS pressure
        HAL_GPIO_WritePin(SHUTDOWN_CMD_GPIO_Port, SHUTDOWN_CMD_Pin, RESET);
        msg.data[1] = 0;
        xQueueSend(canTxASQueue, &msg, 0);
        vTaskSuspend(NULL);
    }
    
    //TurnOnEBSValves   
    HAL_GPIO_WritePin(QM_TRIGGER_GPIO_Port, QM_TRIGGER_Pin, SET); //deactivate EBS valves
    vTaskDelay(200);

    if(xSemaphoreTake(ADCSemHandle, portMAX_DELAY)) {
        //ReadEBSAirPressure
        EBSAir1 = adcReadings.EBSAir1;
        EBSAir2 = adcReadings.EBSAir2;
        xSemaphoreGive(ADCSemHandle);
    }

    if(EBSAir1 < 7.0 || EBSAir2 < 7.0){
        HAL_GPIO_WritePin(SHUTDOWN_CMD_GPIO_Port, SHUTDOWN_CMD_Pin, RESET);
        msg.data[1] = 0;
        xQueueSend(canTxASQueue, &msg, 0);
        vTaskSuspend(NULL);
    }


    if(xSemaphoreTake(ASBCheckSemHandle, portMAX_DELAY)){
        checkedASB = 1;
        xSemaphoreGive(ASBCheckSemHandle);
    }
    vTaskResume(ErrHandASTaskHandle);
    xQueueSend(canTxASQueue, &pcMsg, 0);
    
    vTaskSuspend(NULL);
}