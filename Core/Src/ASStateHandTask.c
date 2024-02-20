#include "ASSTateHandTask.h"



enum MissionStatus {Starting = 0, ReadyToDrive = 1, Finished = 2};
enum Mission {Manual = 0, Acceleration = 1, Skidpad = 2, Autocross = 3, Trackdrive = 4, Inspection = 5, EBSTest = 6}; 

extern osMutexId_t ASCanSemHandle;
extern osMutexId_t EngCanSemHandle;

extern ASCANBuffer AutCanBuffer;
extern EngineCANBuffer EngCANBuffer;

uint8_t checkedASB = 0; // maybe

extern QueueHandle_t canTxASQueue;

void ASStateHandThread(void* argument){
    TickType_t xLastWakeTime;
	const TickType_t xFrequency = 500;

    //init AS state data
    uint8_t shutdownSense = 0;
    uint8_t statusASMS = 0;
    
    uint16_t rpm = 0;
    uint8_t readyToDrive = 0;
    uint8_t brakePressure = 0;
    enum MissionStatus missionStatus = Starting;
    enum Mission mission = Manual; 

    xLastWakeTime = xTaskGetTickCount();
    while(1){
        vTaskDelayUntil( &xLastWakeTime, xFrequency);
        
        if(xSemaphoreTake(ASCanSemHandle, (TickType_t) 0) == pdTRUE){
            missionStatus = AutCanBuffer.missionStatus;
            mission = AutCanBuffer.selectedMission;

			xSemaphoreGive(ASCanSemHandle);
		}

        if(xSemaphoreTake(EngCanSemHandle, (TickType_t) 0) == pdTRUE){
            rpm = EngCANBuffer.RPM;
            xSemaphoreGive(EngCanSemHandle);
        }

        if(shutdownSense){
            if(missionStatus == Finished){
                if(0){ // TODO RES activated
                    setASEmergency();
                }
                else{
                    setASFinished();
                }
            }
            else{
                setASEmergency();
            }
        }
        else{
            if(mission && statusASMS && checkedASB && rpm > MIN_RPM_ENG_ON){
                if(mission == ReadyToDrive){
                    setASDriving();
                }
                else {
                    if(brakePressure >= AS_INIT_BRAKE_PRESSURE){
                        setASReady();
                        // TODO send msg to PC , check for R2D -> ts on and gear engaged

                    }
                    else{
                        setASOff();
                    }
                }
            }
            else    
                setASOff();
        }
    }
}


void setASEmergency(){
    HAL_GPIO_TogglePin(ASSI_LED_B_GPIO_Port, ASSI_LED_B_Pin);
    HAL_GPIO_WritePin(ASSI_LED_Y_GPIO_Port, ASSI_LED_Y_Pin, RESET);
}

void setASFinished(){
    HAL_GPIO_WritePin(ASSI_LED_Y_GPIO_Port, ASSI_LED_Y_Pin, RESET);
    HAL_GPIO_WritePin(ASSI_LED_B_GPIO_Port, ASSI_LED_B_Pin, SET);
}

void setASOff(){
    HAL_GPIO_WritePin(ASSI_LED_Y_GPIO_Port, ASSI_LED_Y_Pin, RESET);
    HAL_GPIO_WritePin(ASSI_LED_B_GPIO_Port, ASSI_LED_B_Pin, RESET);
}

void setASReady(){
    HAL_GPIO_WritePin(ASSI_LED_Y_GPIO_Port, ASSI_LED_Y_Pin, SET);
    HAL_GPIO_WritePin(ASSI_LED_B_GPIO_Port, ASSI_LED_B_Pin, RESET);
    // TODO send ready msg to PC
}

void setASDriving(){
    HAL_GPIO_WritePin(ASSI_LED_B_GPIO_Port, ASSI_LED_B_Pin, RESET);
    HAL_GPIO_TogglePin(ASSI_LED_Y_GPIO_Port, ASSI_LED_Y_Pin);
}