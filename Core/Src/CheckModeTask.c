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
	const TickType_t xFrequency = 20;

    CAN_TxHeaderTypeDef header;
    header.StdId = 400;
    header.DLC = 8;
    msg.header = header;
    
    actualMode = NotSelected;
    reqMode = NotSelected;


    msg.data[0] = 10;

    int AS_PIN = 0;

    while(1){
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
                }

                // if( __NOP()){ // TODO ASMS on and no mode selected
                //     sendErrorToPilot(10);
                // }
            }
        }
        else{ // Misison already selected, to change mission is necessary reset the board.
            // if(actualMode == Manual){
            //     if(__NOP()){  // TODO ASMS on in manual mode
            //         sendErrorToPilot(10);
            //     }
            // }

            if(actualMode == Autonomous){
                if(xSemaphoreTake(EngCanSemHandle, (TickType_t) 0) == pdTRUE){
                    //ReadingModeData
                    if(EngCANBuffer.RPM > MIN_RPM_ENG_ON){ // TODO TS active while no ASMS OFF
                        sendErrorToPilot(11);
                    }
                }
            }
        }
    }
    vTaskDelayUntil(xLastWakeTime, xFrequency);
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