#include "CheckModeTask.h"


//CAN variables
extern osMutexId_t ASCanSemHandle;
extern osMutexId_t EngCanSemHandle;

extern ASCANBuffer AutCanBuffer;
extern EngineCANBuffer EngCANBuffer;

extern osMessageQueueId_t canTxASQueue;

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
    header.StdId = 400;
    header.DLC = 8;
    msg.header = header;
    
    actualMode = NotSelected;
    reqMode = NotSelected;

    HAL_GPIO_WritePin(BRAKE_LIGHT_GPIO_Port, BRAKE_LIGHT_Pin, GPIO_PIN_SET);
    vTaskDelay(1000 /  portTICK_PERIOD_MS);
    msg.data[0] = 10;

    int AS_PIN = 0;

    xLastWakeTime = xTaskGetTickCount();
    while(1){
        vTaskDelayUntil( &xLastWakeTime, xFrequency );
        // Reading the state from  PILOT23
        if(actualMode == NotSelected){
            if(xSemaphoreTake(ASCanSemHandle, (TickType_t) WAIT_FOR_PILOT_STATE) == pdTRUE){
                //Critical section
                readModeData();
                xSemaphoreGive(ASCanSemHandle);
            }
            if(reqMode != NotSelected) {
                actualMode = reqMode;
            }
            else{
                if(xSemaphoreTake(EngCanSemHandle, (TickType_t) 0) == pdTRUE){
                    //ReadingModeData
                    if(EngCANBuffer.RPM > MIN_RPM_ENG_ON){ // TS active while no Mode selected
                        sendErrorToPilot(12);
                    }
                    xSemaphoreGive(EngCanSemHandle);
                }

                if(HAL_GPIO_ReadPin(ASMS_STATUS_GPIO_Port, ASMS_STATUS_Pin)){ // ASMS on and no mode selected
                    sendErrorToPilot(10);
                    HAL_GPIO_WritePin(BRAKE_LIGHT_GPIO_Port, BRAKE_LIGHT_Pin, GPIO_PIN_SET);
                }
                else    
                    HAL_GPIO_WritePin(BRAKE_LIGHT_GPIO_Port, BRAKE_LIGHT_Pin, GPIO_PIN_RESET);
            }
        }
        else{ 
            
            // Misison already selected, to change mission is necessary reset the board.
            if(actualMode == Manual){
                if(HAL_GPIO_ReadPin(ASMS_STATUS_GPIO_Port, ASMS_STATUS_Pin) == GPIO_PIN_RESET){ //ASMS on in manual mode
                    sendErrorToPilot(10);
                }
            }

            if(actualMode == Autonomous){
                if (HAL_GPIO_ReadPin(ASMS_STATUS_GPIO_Port, ASMS_STATUS_Pin) == GPIO_PIN_SET){
                    //TODO activation of autonomous tasks
                    HAL_GPIO_WritePin(LED_B_GPIO_Port, LED_B_Pin, SET);
                }
                else
                if(xSemaphoreTake(EngCanSemHandle, (TickType_t) 0) == pdTRUE){
                    //ReadingModeData
                    if(EngCANBuffer.RPM > MIN_RPM_ENG_ON){ // TODO TS active while and ASMS OFF
                        sendErrorToPilot(11);
                    }
                    xSemaphoreGive(EngCanSemHandle);
                }

            }
        }
        
    }
}

void readModeData() {
    reqMode = AutCanBuffer.reqMode % 3;
    selectedMission = AutCanBuffer.selectedMission;
}

void sendErrorToPilot(uint8_t errorCode) {
    msg.data[0] = errorCode;
    if(osMessageQueuePut(canTxASQueue, &msg, NULL, 0) != HAL_OK){
        // TODO comm. error
    }
}

int readASMSPin(){
    HAL_GPIO_ReadPin (GPIOA, GPIO_PIN_9);
}