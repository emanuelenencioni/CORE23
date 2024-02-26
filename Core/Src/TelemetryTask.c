#include "TelemetryTask.h"

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

void telemetryThread(void* argument) {

    CAN_TxHeaderTypeDef header;
    CANMessage msg;

    TickType_t xLastWakeTime;
	const TickType_t xFrequency = pdMS_TO_TICKS(100);

    // Settings for can message
    header.StdId = 450;
    header.ExtId = 0;
    header.IDE = 0;
    header.RTR = 0;
    header.DLC = 8;
    msg.header = header;

    // ADC Variables
    uint16_t desmo1 = 0;
    uint16_t desmo2 = 0;
    uint16_t clutchOil = 0;
    uint16_t GearUpAir = 0;
    uint16_t APPS1 = 0;
    uint16_t APPS2 = 0;
    uint16_t VPPMSense = 0;
    uint16_t BPS = 0;
    uint16_t EBSAir1 = 0;
    uint16_t EBSAir2 = 0;
    uint16_t ADC_AUX1 = 0;
    uint16_t ADC_AUX2 = 0;


    xLastWakeTime = xTaskGetTickCount(); // rate of execution

    while(1){

        // Let's get the ADC values
        if(xSemaphoreTake(ADCSemHandle, (TickType_t) 0) == pdTRUE) {

            desmo1 = adcReadings.desmo1;
            desmo2 = adcReadings.desmo2;
            clutchOil = adcReadings.clutchOil;
            GearUpAir = adcReadings.GearUpAir;
            APPS1 = adcReadings.APPS1;
            APPS2 = adcReadings.APPS2;
            VPPMSense = adcReadings.VPPMSense;
            BPS = adcReadings.BPS;
            EBSAir1 = adcReadings.EBSAir1;
            EBSAir2 = adcReadings.EBSAir2;
            ADC_AUX1 = adcReadings.ADC_AUX1;
            ADC_AUX2 = adcReadings.ADC_AUX2;

            xSemaphoreGive(ADCSemHandle);
        }


        // INVIO DESMO 1 E DESMO2, APPS1 E APPS2 //////////////////////////////////////////
        sendCANInt16(&msg, desmo1, desmo2, APPS1, APPS2, 450, 0, 0, 0, 8);

        // INVIO CLUTCH OIL E GEAR UP AIR, VPPM SENSE E BPS //////////////////////////////////////////
        sendCANInt16(&msg, clutchOil, GearUpAir, VPPMSense, BPS, 451, 0, 0, 0, 8);

        // INVIO EBS AIR1 E EBS AIR2, ADC AUX1 E ADC AUX2 //////////////////////////////////////////
        sendCANInt16(&msg, EBSAir1, EBSAir2, ADC_AUX1, ADC_AUX2, 452, 0, 0, 0, 8);

        //TODO send to LoRa
        __NOP();

        vTaskDelayUntil( &xLastWakeTime, xFrequency);
    }

}

void sendCANInt16(CANMessage* msg, uint16_t data1, uint16_t data2, uint16_t data3, uint16_t data4, uint16_t stdId, uint32_t extId, uint8_t ide, uint8_t rtr, uint8_t dlc) {
    msg->header.StdId = stdId;
    msg->header.ExtId = extId;
    msg->header.IDE = ide;
    msg->header.RTR = rtr;
    msg->header.DLC = dlc;

    msg->data[0] = (uint8_t)(data1 >> 8);
    msg->data[1] = (uint8_t)data1;
    msg->data[2] = (uint8_t)(data2 >> 8);
    msg->data[3] = (uint8_t)data2;
    msg->data[4] = (uint8_t)(data3 >> 8);
    msg->data[5] = (uint8_t)data3;
    msg->data[6] = (uint8_t)(data4 >> 8);
    msg->data[7] = (uint8_t)data4;
}