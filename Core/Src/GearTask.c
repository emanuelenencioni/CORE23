#include "GearTask.h"

//CAN variables
extern osMutexId_t ASCanSemHandle;
extern osMutexId_t EngCanSemHandle;

extern ASCANBuffer AutCanBuffer;
extern EngineCANBuffer EngCANBuffer;

extern QueueHandle_t canTxASQueue;

//Can message for sending state error to the pilot23
CAN_TxHeaderTypeDef header;
CANMessage msgGear;

void gearThread(void* argument) {
    
    TickType_t xLastWakeTime;
	const TickType_t xFrequency = 10;

    // Gear task function
    uint8_t request_upshift = 0;
    uint8_t request_downshift = 0;

    xLastWakeTime = xTaskGetTickCount(); // rate of execution

    while(1){

        if(xSemaphoreTake(ASCanSemHandle, (TickType_t) 0) == pdTRUE) {

        request_downshift = AutCanBuffer.reqDownShift;
        request_upshift = AutCanBuffer.reqUpShift;

        xSemaphoreGive(ASCanSemHandle);

        } 

    }

}

void upShift() {
    // Set the gear to the next gear
    HAL_GPIO_WritePin(CUTOFF_GPIO_Port, CUTOFF_Pin, GPIO_PIN_SET);
    vTaskDelay(100); // Cut-off Duration
    HAL_GPIO_WritePin(VUVG_UP_GPIO_Port, VUVG_UP_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(CUTOFF_GPIO_Port, CUTOFF_Pin, GPIO_PIN_RESET);
    vTaskDelay(100); // Upshift Duration
    HAL_GPIO_WritePin(VUVG_UP_GPIO_Port, VUVG_UP_Pin, GPIO_PIN_RESET);
}