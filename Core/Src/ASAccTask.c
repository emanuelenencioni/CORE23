#include "ASAccTask.h"


extern osMutexId_t EngCanSemHandle;
extern EngineCANBuffer EngCANBuffer;


extern osMutexId_t ASCanSemHandle;
extern ASCANBuffer AutCanBuffer;




void ASAccThread(void* argument) {
    uint8_t launch = 1;
    uint8_t reqAcceleration = 0;
    float speed;
    float accValue = 0;

    while(1) {
        if(xSemaphoreTake(EngCanSemHandle, portMAX_DELAY) == pdTRUE) {
            speed = getSpeed();
            xSemaphoreGive(EngCanSemHandle);
        }

        if(xSemaphoreTake(ASCanSemHandle, portMAX_DELAY) == pdTRUE) {
            //ReadAccCan
            reqAcceleration = AutCanBuffer.reqAcceleration;
            xSemaphoreGive(ASCanSemHandle);
        }
        if(launch) {
            if(reqAcceleration) { // Start the launch procedure if acceleration is requested from the PC
                launch = 0;
                LaunchRoutine();
            }
        }
        else {
            if(speed < 12.0){ //TODO parameter to tune, depend on the car
                slowRoutine();
                if(speed == 0.0){
                    //SetLaunch
                    launch = 1;
                }
            }
            else{
                //SendAccMotor
                accValue = map(reqAcceleration, 0, 100, 0, 5); // TODO capire se range sono ok
                DAC->DHR12R2 = accValue;
                __NOP();
            }
        }
    }
}


void LaunchRoutine() {
    __NOP();
}

void slowRoutine() {
    //TODO slow speed routine
    __NOP();
}

float getSpeed(){
    return ((float) EngCANBuffer.speedFrontL + EngCANBuffer.speedFrontR + EngCANBuffer.speedBackL + EngCANBuffer.speedBackR) / 4.0;
}