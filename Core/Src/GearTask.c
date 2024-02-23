#include "GearTask.h"

//CAN variables
extern osMutexId_t ASCanSemHandle;
extern osMutexId_t EngCanSemHandle;

extern ASCANBuffer AutCanBuffer;
extern EngineCANBuffer EngCANBuffer;

extern QueueHandle_t canTxASQueue;

extern osMutexId_t ADCSemHandle;
//Can message for sending state error to the pilot23

//adc buffer from dma
extern ADCBuffer adcReadings;

uint16_t desmo1;
uint16_t desmo2;
uint16_t rpm;
uint16_t clutchRequest;
uint16_t VPPMSense;

CANMessage msgGear;

uint16_t gear_pos[] = {3260, 3620, 200, 875, 1570, 2250}; // Gear position values

void gearThread(void* argument) {
    CAN_TxHeaderTypeDef header;

    TickType_t xLastWakeTime;
	const TickType_t xFrequency = 10;

    // Gear task function
    uint8_t reqDownShift = 0;
    uint8_t reqUpShift = 0;
    uint8_t force_mode = 0;

    // Settings for can message
    header.StdId = 804; // Gear position
    header.ExtId = 0;
    header.IDE = 0;
    header.RTR = 0;
    header.DLC = 1;
    msgGear.header = header;

    desmo1 = 0;
    desmo2 = 0;

    xLastWakeTime = xTaskGetTickCount(); // rate of execution

    while(1){

        if(xSemaphoreTake(ADCSemHandle, (TickType_t) 0) == pdTRUE) {

            desmo1 = adcReadings.desmo1;
            desmo2 = adcReadings.desmo2;
            VPPMSense = adcReadings.VPPMSense;

            xSemaphoreGive(ADCSemHandle);
        }

        msgGear.data[0] = desmo1; // Gear position
        xQueueSend(canTxASQueue, &msgGear, 0); // TODO: check the queue

        if(xSemaphoreTake(ASCanSemHandle, (TickType_t) 0) == pdTRUE) {

            reqDownShift = AutCanBuffer.reqDownShift;
            reqUpShift = AutCanBuffer.reqUpShift;
            clutchRequest = AutCanBuffer.clutchRequest;

            xSemaphoreGive(ASCanSemHandle);
        }

        if(xSemaphoreTake(EngCanSemHandle, (TickType_t) 0) == pdTRUE) {

            rpm = EngCANBuffer.RPM;

            xSemaphoreGive(EngCanSemHandle);
        }


    }

    if(reqUpShift == 1){
        if(rpm > RPM_THRESHOLD){
            upShiftCutOff();
        }
        else{
            upShift();
        }
    }
    else if(reqDownShift == 1){
        if(rpm > RPM_THRESHOLD){
            downShiftCutOff();
        }
        else{
            downShift();
        }
    }

    vTaskDelayUntil( &xLastWakeTime, xFrequency);
    
}

void upShiftCutOff() {
    // Gear upshift with cut-off
    HAL_GPIO_WritePin(CUTOFF_GPIO_Port, CUTOFF_Pin, GPIO_PIN_SET);
    vTaskDelay(CUTOFF_TIME); // Cut-off Duration
    HAL_GPIO_WritePin(VUVG_UP_GPIO_Port, VUVG_UP_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(CUTOFF_GPIO_Port, CUTOFF_Pin, GPIO_PIN_RESET);
    vTaskDelay(SHIFT_HOLD); // Upshift Duration
    HAL_GPIO_WritePin(VUVG_UP_GPIO_Port, VUVG_UP_Pin, GPIO_PIN_RESET);
}

void downShiftCutOff() {
    // Gear downshift with cut-off
    HAL_GPIO_WritePin(CUTOFF_GPIO_Port, CUTOFF_Pin, GPIO_PIN_SET);
    vTaskDelay(CUTOFF_TIME); // Cut-off Duration
    HAL_GPIO_WritePin(VUVG_DOWN_GPIO_Port, VUVG_UP_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(CUTOFF_GPIO_Port, CUTOFF_Pin, GPIO_PIN_RESET);
    vTaskDelay(SHIFT_HOLD); // Upshift Duration
    HAL_GPIO_WritePin(VUVG_DOWN_GPIO_Port, VUVG_UP_Pin, GPIO_PIN_RESET);
}

void upShift() {
    // Gear upshift without cut-off
    HAL_GPIO_WritePin(VUVG_UP_GPIO_Port, VUVG_UP_Pin, GPIO_PIN_SET);
    vTaskDelay(SHIFT_HOLD); // Upshift Duration
    HAL_GPIO_WritePin(VUVG_UP_GPIO_Port, VUVG_UP_Pin, GPIO_PIN_RESET);
}

void downShift() {
    // Gear downshift without cut-off
    HAL_GPIO_WritePin(VUVG_DOWN_GPIO_Port, VUVG_UP_Pin, GPIO_PIN_SET);
    vTaskDelay(SHIFT_HOLD); // Upshift Duration
    HAL_GPIO_WritePin(VUVG_DOWN_GPIO_Port, VUVG_UP_Pin, GPIO_PIN_RESET);
}

uint8_t gear_getCurrent(){
	//find minimum error to guess the current gear
	uint16_t error = 4095;
	uint8_t  guessed_gear;

	for (uint8_t tmp_gear = 0; tmp_gear <= 5; tmp_gear++){
		if (abs(gear_pos[tmp_gear]- desmo1) <= error){
			error = abs(gear_pos[tmp_gear]- desmo1);
			guessed_gear = tmp_gear;
		}

	}
	return guessed_gear;
}
