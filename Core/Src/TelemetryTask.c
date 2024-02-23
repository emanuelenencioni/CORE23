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
	const TickType_t xFrequency = 10;

    // Settings for can message
    header.StdId = 804; // Gear position
    header.ExtId = 0;
    header.IDE = 0;
    header.RTR = 0;
    header.DLC = 1;
    msg.header = header;

    // ADC Variables
    uint16_t desmo1 = 0;
    uint16_t desmo2 = 0;
    float clutchOil = 0.0;
    float GearUpAir = 0.0;
    uint16_t APPS1 = 0;
    uint16_t APPS2 = 0;
    float VPPMSense = 0.0;
    float BPPS = 0.0;
    float EBSAir1 = 0.0;
    float EBSAir2 = 0.0;
    uint16_t ADC_AUX1 = 0;
    uint16_t ADC_AUX2 = 0;

    // Engine CAN Variables
    uint16_t	Lambda = 0;
    float		CutoffV = 0.0;
    float		MAP = 0.0;
    uint8_t		ATS = 0;
    uint16_t	RPM = 0;
    uint16_t	TPS = 0;
    float		FPS = 0.0;
    float		OPS = 0.0;
    float 		IGN = 0.0;
    float 		WTS = 0.0;
    float 		VCC = 0.0;

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
            BPPS = adcReadings.BPPS;
            EBSAir1 = adcReadings.EBSAir1;
            EBSAir2 = adcReadings.EBSAir2;
            ADC_AUX1 = adcReadings.ADC_AUX1;
            ADC_AUX2 = adcReadings.ADC_AUX2;

            xSemaphoreGive(ADCSemHandle);
        }

        // Let's get the Engine CAN values
        if(xSemaphoreTake(EngCanSemHandle, (TickType_t) 0) == pdTRUE) {

            Lambda = EngCANBuffer.Lambda;
            CutoffV = EngCANBuffer.CutoffV;
            MAP = EngCANBuffer.MAP;
            ATS = EngCANBuffer.ATS;
            RPM = EngCANBuffer.RPM;
            TPS = EngCANBuffer.TPS;
            FPS = EngCANBuffer.FPS;
            OPS = EngCANBuffer.OPS;
            IGN = EngCANBuffer.IGN;
            WTS = EngCANBuffer.WTS;
            VCC = EngCANBuffer.VCC;

            xSemaphoreGive(EngCanSemHandle);
        }

        header.StdId = 804; // Gear position
        msg.header = header;
        msg.data[0] = (uint8_t)(desmo1 >> 8); // Byte più significativo di desmo1
        msg.data[1] = (uint8_t)desmo1; // Byte meno significativo di desmo1
        xQueueSend(canTxASQueue, &msg, 0); // TODO: check the queue
        // Vedere se conviene utilizzare altri ID per la telemtria, per ridurre il numero di messaggi

        vTaskDelayUntil( &xLastWakeTime, xFrequency);
    }

}

void sendCANInt(CANMessage* msg, uint32_t data, uint16_t stdId, uint32_t extId, uint8_t ide, uint8_t rtr, uint8_t dlc){
    msg->header.StdId = stdId;
    msg->header.ExtId = extId;
    msg->header.IDE = ide;
    msg->header.RTR = rtr;
    msg->header.DLC = dlc;
    msg->data[0] = (uint8_t)(data >> 24); // Byte più significativo
    msg->data[1] = (uint8_t)(data >> 16);
    msg->data[2] = (uint8_t)(data >> 8);
    msg->data[3] = (uint8_t)data; // Byte meno significativo
    xQueueSend(canTxASQueue, msg, 0); // TODO: check the queue
}