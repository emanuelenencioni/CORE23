#include "CheckModeTask.h"


//CAN variables

extern osMutexId_t EngCanSemHandle;
extern EngineCANBuffer EngCANBuffer;

extern osMutexId_t ASCanSemHandle;
extern ASCANBuffer AutCanBuffer;

extern QueueHandle_t canTxASQueue;


extern osThreadId_t ASStateHandTaskHandle;
extern osThreadId_t ASBCheckTaskHandle;
extern osThreadId_t PedalTaskHandle;
extern osThreadId_t ASAccTaskHandle;
extern osThreadId_t TelemetryTaskHandle;


//Can message for sending state error to the pilot23
CANMessage msg;

uint8_t selectedMission;



enum  Mode actualMode;
enum Mode reqMode; 

//FOR TESTING
extern CAN_HandleTypeDef hcan2;
extern TIM_HandleTypeDef htim5;

void checkModeThread(void* argument){

    TickType_t xLastWakeTime;
    const TickType_t xFrequency = pdMS_TO_TICKS(200);


    HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, RESET);
    CAN_TxHeaderTypeDef header;
    // Settings for can message
    header.StdId = 400;
    header.ExtId = 0;
    header.IDE = 0;
    header.RTR = 0;
    header.DLC = 1;
    msg.header = header;

    uint8_t autTaskActivated = 0;
    uint8_t telemTaskActivated = 0;
    uint16_t rpm = 0;

    actualMode = NotSelected;
    reqMode = NotSelected;

    // Default error message
    msg.data[0] = 10;

    xLastWakeTime = xTaskGetTickCount();
    while(1){
        vTaskDelayUntil( &xLastWakeTime, xFrequency );

        switch(actualMode){
            default:
                if(xSemaphoreTake(ASCanSemHandle, (TickType_t) WAIT_FOR_PILOT_STATE) == pdTRUE){
                    readModeData();
                    xSemaphoreGive(ASCanSemHandle);
                }
                if( reqMode == Autonomous || reqMode == Manual) 
                    // UpdateMode
                    
                    actualMode = reqMode;
                else{ 
                    if(xSemaphoreTake(EngCanSemHandle, (TickType_t) 0) == pdTRUE){
                        // ReadRPM
                        rpm = EngCANBuffer.RPM;
                        xSemaphoreGive(EngCanSemHandle);
                    }
                    // CheckForPlausability
                    if(rpm > MIN_RPM_ENG_ON) // TS active while no Mode selected
                        sendErrorToPilot(12);
                    else 
                        if(HAL_GPIO_ReadPin(ASMS_STATUS_GPIO_Port, ASMS_STATUS_Pin)) // ASMS on and no mode selected
                            sendErrorToPilot(10);
                }
                break;
            case Autonomous:
                if(!telemTaskActivated){
                    vTaskResume(TelemetryTaskHandle);
                    telemTaskActivated = 1;
                }
                if (HAL_GPIO_ReadPin(ASMS_STATUS_GPIO_Port, ASMS_STATUS_Pin) == GPIO_PIN_SET){
                    if(!autTaskActivated){
                        vTaskDelete(PedalTaskHandle);
                        vTaskResume(ASStateHandTaskHandle);
                        vTaskResume(ASBCheckTaskHandle);
                        vTaskResume(ASAccTaskHandle);
                        autTaskActivated = 1;
                    }
                }
                else{ 
                    if(xSemaphoreTake(EngCanSemHandle, (TickType_t) 0) == pdTRUE){
                        //ReadRPM
                        rpm = EngCANBuffer.RPM;
                        xSemaphoreGive(EngCanSemHandle);
                    }
                    //CheckForAutoPlausability
                    if(rpm > MIN_RPM_ENG_ON) // TS active while and ASMS OFF
                        sendErrorToPilot(11);
                }

                break;


            case Manual:
                // CheckASMSOff
                if(!telemTaskActivated){
                    vTaskResume(TelemetryTaskHandle);
                    telemTaskActivated = 1;
                }
                if(HAL_GPIO_ReadPin(ASMS_STATUS_GPIO_Port, ASMS_STATUS_Pin) == GPIO_PIN_SET)// ASMS on in manual mode
                    sendErrorToPilot(10);
                
                break;

        }
    }
}

void sendErrorToPilot(uint8_t errorCode) {
    msg.data[0] = errorCode;
    if(xQueueSend(canTxASQueue, &msg, 0) != pdTRUE){
        //TODO comm. error
    }
}


void readModeData() {
    if(AutCanBuffer.reqMode > 2 || AutCanBuffer.reqMode < 0)
        reqMode = 0;
    else
        reqMode = AutCanBuffer.reqMode;
}