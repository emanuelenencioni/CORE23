#include "CheckModeTask.h"


//CAN variables
extern osMutexId_t ASCanSemHandle;
extern osMutexId_t EngCanSemHandle;

extern ASCANBuffer AutCanBuffer;
extern EngineCANBuffer EngCANBuffer;

extern QueueHandle_t canTxASQueue;

//Can message for sending state error to the pilot23
CANMessage msg;

uint8_t selectedMission;

enum Mode {NotSelected = 0, Manual = 1, Autonomous = 2};

enum  Mode actualMode;
enum Mode reqMode; 



void checkModeThread(void* argument){

    TickType_t xLastWakeTime;
    const TickType_t xFrequency = 100;


    HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, RESET);
    CAN_TxHeaderTypeDef header;
    // Settings for can message
    header.StdId = 400;
    header.ExtId = 0;
    header.IDE = 0;
    header.DLC = 1;
    msg.header = header;

    uint8_t autTaskActivated = 0;
    uint16_t rpm = 0;

    actualMode = NotSelected;
    reqMode = NotSelected;

    vTaskDelay(1000 /  portTICK_PERIOD_MS);
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
                    // updateMode
                    actualMode = reqMode;
                else{ 
                    if(xSemaphoreTake(EngCanSemHandle, (TickType_t) 0) == pdTRUE){
                        // readRPM
                        rpm = EngCANBuffer.RPM;
                        xSemaphoreGive(EngCanSemHandle);
                    }
                    // checkForPlausability
                    if(rpm > MIN_RPM_ENG_ON) // TS active while no Mode selected
                        sendErrorToPilot(12);
                    else 
                        if(HAL_GPIO_ReadPin(ASMS_STATUS_GPIO_Port, ASMS_STATUS_Pin)) // ASMS on and no mode selected
                            sendErrorToPilot(10);
                }
                break;


            case Autonomous:
                if (HAL_GPIO_ReadPin(ASMS_STATUS_GPIO_Port, ASMS_STATUS_Pin) == GPIO_PIN_SET){
                    if(!autTaskActivated){
                        //TODO Activation of autonomous tasks
                        //TODO set and send mission
                    }
                }
                else{ 
                    if(xSemaphoreTake(EngCanSemHandle, (TickType_t) 0) == pdTRUE){
                        // ReadingModeData
                        rpm = EngCANBuffer.RPM;
                        xSemaphoreGive(EngCanSemHandle);
                    }
                    
                    if(rpm > MIN_RPM_ENG_ON) // TS active while and ASMS OFF
                        sendErrorToPilot(11);
                }

                break;


            case Manual:
                // checkASMSOff
                if(HAL_GPIO_ReadPin(ASMS_STATUS_GPIO_Port, ASMS_STATUS_Pin) == GPIO_PIN_SET)// ASMS on in manual mode
                    sendErrorToPilot(10);
                
                break;
        }
    }
}

void sendErrorToPilot(uint8_t errorCode) {
    msg.data[0] = errorCode;
    if(xQueueSend(canTxASQueue, &msg, 0) != pdTRUE){
        // TODO comm. error
    }
}


void readModeData() {
    reqMode = AutCanBuffer.reqMode;
    selectedMission = AutCanBuffer.selectedMission;
}