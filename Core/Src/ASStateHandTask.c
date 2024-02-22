#include "ASSTateHandTask.h"



enum MissionStatus {Starting = 0, ReadyToDrive = 1, Finished = 2, Error = 3};
enum Mission {Manual = 0, Acceleration = 1, Skidpad = 2, Autocross = 3, Trackdrive = 4, Inspection = 5, EBSTest = 6}; 

extern osMutexId_t ASCanSemHandle;
extern osMutexId_t EngCanSemHandle;

extern ASCANBuffer AutCanBuffer;
extern EngineCANBuffer EngCANBuffer;

extern TIM_HandleTypeDef htim1;



uint8_t checkedASB = 1; // maybe


//For ASEmergency state
uint8_t firstTimeASEmergency = 1;
TickType_t emergencyStartTicks = 0;
TickType_t emergencyEndTicks = 0;

//For ASReady state, we need to wait 5 seconds before activate GO signal from RES
uint8_t firstTimeASReady = 1;
TickType_t readyStartTicks = 0;
TickType_t readyEndTicks = 0;

extern QueueHandle_t canTxASQueue;



void ASStateHandlerThread(void* argument){
    TickType_t xLastWakeTime;
	const TickType_t xFrequency =  pdMS_TO_TICKS(200);

    //init AS state data
    uint8_t shutdownSense = 0;
    uint8_t statusASMS = 0;
    uint8_t RESSignalGo = 0;
    uint16_t rpm = 0;
    uint8_t neutral = 1; //for indicating the state of the gear
    uint8_t readyToDrive = 0;
    uint8_t brakePressure = 0;
    enum MissionStatus missionStatus = Starting;
    enum Mission mission = Manual; 

    CANMessage msg;

    if(HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1) !=  HAL_OK){
        // TODO error generating PWM
    }
    TIM1->CCR1 = 0;
    
    CAN_TxHeaderTypeDef header;

    header.StdId = 350;
    header.ExtId = 0;
    header.IDE = 0;
    header.RTR = 0;
    header.DLC = 1;
    msg.header = header;

    xLastWakeTime = xTaskGetTickCount();
    while(1){
        

        vTaskDelayUntil( &xLastWakeTime, xFrequency);
        
        if(xSemaphoreTake(EngCanSemHandle, (TickType_t) 0) == pdTRUE){
            // ReadRPM
            rpm = EngCANBuffer.RPM;
            xSemaphoreGive(EngCanSemHandle);
        }
        if(xSemaphoreTake(ASCanSemHandle, (TickType_t) 0) == pdTRUE){
            // ReadASData
            missionStatus = AutCanBuffer.missionStatus;
            mission = AutCanBuffer.selectedMission;
            brakePressure = AutCanBuffer.brakePressure;
			xSemaphoreGive(ASCanSemHandle);
		}

        // ReadGPIO
        statusASMS = HAL_GPIO_ReadPin(ASMS_STATUS_GPIO_Port, ASMS_STATUS_Pin);
        RESSignalGo = HAL_GPIO_ReadPin(RES_IN1_GPIO_Port, RES_IN1_Pin); 

        //Check for anything other than CORE23 that open the SDC
        shutdownSense = HAL_GPIO_ReadPin(SHUTDOWN_SENSE_GPIO_Port, SHUTDOWN_SENSE_Pin);
        //Check if CORE23 opened SDC
        
        

        // Computing State, check FS_RULES 2024 v 1.1
        if(!shutdownSense){
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
            if(mission && statusASMS  && checkedASB && rpm >= MIN_RPM_ENG_ON){
                if(!neutral){ //R2D -> only if TS on and gear is engaged
                    setASDriving();
                }
                else {
                    if(brakePressure >= AS_INIT_BRAKE_PRESSURE){
                        setASReady();
                        readyEndTicks = xTaskGetTickCount();

                        if((readyEndTicks - readyStartTicks) > 5000 && RESSignalGo){ // 5 seconds to wait before accepting the "GO" signal from RES
                            // TODO activate steering system
                            msg.data[0] = 1;
                            if(xQueueSend(canTxASQueue, &msg, 0) != pdTRUE){// TODO send msg to PC , check for R2D -> ts on and gear engaged
                                // TODO comm. error
                            }
                            // TODO il pc deve mettere la prima metti la prima premi la frizione
                        }
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

    //Activate siren for 9s
    if(firstTimeASEmergency) {
        emergencyStartTicks = xTaskGetTickCount();
        firstTimeASEmergency = 0;
    }
    emergencyEndTicks = xTaskGetTickCount();
    if((emergencyEndTicks - emergencyStartTicks) < 9000)
        actSiren();
    else
        deactSiren();
}

void setASFinished(){
    HAL_GPIO_WritePin(ASSI_LED_Y_GPIO_Port, ASSI_LED_Y_Pin, RESET);
    HAL_GPIO_WritePin(ASSI_LED_B_GPIO_Port, ASSI_LED_B_Pin, SET);
    deactSiren();
    
}

void setASOff(){
    HAL_GPIO_WritePin(ASSI_LED_Y_GPIO_Port, ASSI_LED_Y_Pin, RESET);
    HAL_GPIO_WritePin(ASSI_LED_B_GPIO_Port, ASSI_LED_B_Pin, RESET);
    deactSiren();
    firstTimeASReady = 1;
    firstTimeASEmergency = 1;
}

void setASReady(){
    if(firstTimeASReady){
        readyStartTicks = xTaskGetTickCount();
        firstTimeASReady = 0;
    }

    HAL_GPIO_WritePin(ASSI_LED_Y_GPIO_Port, ASSI_LED_Y_Pin, SET);
    HAL_GPIO_WritePin(ASSI_LED_B_GPIO_Port, ASSI_LED_B_Pin, RESET);
    deactSiren();
}

void setASDriving(){
    HAL_GPIO_WritePin(ASSI_LED_B_GPIO_Port, ASSI_LED_B_Pin, RESET);
    HAL_GPIO_TogglePin(ASSI_LED_Y_GPIO_Port, ASSI_LED_Y_Pin);
    deactSiren();
}




void actSiren(){
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    TIM1->CCR1 = 50;
    
}

void deactSiren(){
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
    TIM1->CCR1 = 0;
}
