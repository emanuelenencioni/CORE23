#include "CheckModeTask.h"
#include "cmsis_os.h"
#include "main.h"
#include "task.h"
#include "semphr.h"
#include "CanHandlerTask.h"

extern osMessageQueueId_t canTxEngineQueue;
extern osMutexId_t ASCanSemHandle;
extern ASCANBuffer AutCanBuffer;


enum state {Manual = 0, Autonomous = 1, NotSelected = 2};

void checkModeThread(void* argument){

    TickType_t xLastWakeTime;
	const TickType_t xFrequency = 20;

    enum state actualState = NotSelected;
    enum state reqState = Manual;

    while(1){
        // Reading the state from  PILOT23
        if(xSemaphoreTake(ASCanSemHandle, (TickType_t) WAIT_FOR_PILOT_STATE) == pdTRUE){
            //ReadingModeData
            actualState = AutCanBuffer.reqState % 3;
            xSemaphoreGive(ASCanSemHandle);
        }

        if(reqState == Autonomous) {
            
        }

    }

    vTaskDelayUntil(xLastWakeTime, xFrequency);
}
