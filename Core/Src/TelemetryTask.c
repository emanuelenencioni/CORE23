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

    // Engine CAN Variables
    uint16_t	Lambda = 0;
    uint32_t	MAP = 0;
    uint8_t		ATS = 0;
    uint16_t	RPM = 0;
    uint16_t	TPS = 0;
    uint32_t	FPS = 0;
    uint32_t	OPS = 0;
    uint32_t	IGN = 0;
    uint32_t	WTS = 0;
    uint32_t	VCC = 0;

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

        // Let's get the Engine CAN values
        if(xSemaphoreTake(EngCanSemHandle, (TickType_t) 0) == pdTRUE) {

            Lambda = EngCANBuffer.Lambda;
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

        //INVIO DESMO 1 E DESMO2, APPS1 E APPS2 //////////////////////////////////////////
        header.StdId = 450; // Gear position
        //header.DLC = 8;
        msg.header = header;

        msg.data[0] = (uint8_t)(desmo1 >> 8); // Byte più significativo di desmo1
        msg.data[1] = (uint8_t)desmo1; // Byte meno significativo di desmo1
        msg.data[2] = (uint8_t)(desmo2 >> 8); // Byte più significativo di desmo2
        msg.data[3] = (uint8_t)desmo2; // Byte meno significativo di desmo2
        msg.data[4] = (uint8_t)(APPS1 >> 8); // Byte più significativo di APPS1
        msg.data[5] = (uint8_t)APPS1; // Byte meno significativo di APPS1
        msg.data[6] = (uint8_t)(APPS2 >> 8); // Byte più significativo di APPS2
        msg.data[7] = (uint8_t)APPS2; // Byte meno significativo di APPS2

        xQueueSend(canTxASQueue, &msg, 0);
        /////////////////////////////////////////////////////////////////////////

        //INVIO CLUTCH OIL E GEAR UP AIR, VPPM SENSE E BPS //////////////////////////////////////////
        header.StdId = 451; // Gear position
        //header.DLC = 8;
        msg.header = header;

        msg.data[0] = (uint8_t)(clutchOil >> 8); // Byte più significativo di clutchOil
        msg.data[1] = (uint8_t)clutchOil; // Byte meno significativo di clutchOil
        msg.data[2] = (uint8_t)(GearUpAir >> 8); // Byte più significativo di GearUpAir
        msg.data[3] = (uint8_t)GearUpAir; // Byte meno significativo di GearUpAir
        msg.data[4] = (uint8_t)(VPPMSense >> 8); // Byte più significativo di VPPMSense
        msg.data[5] = (uint8_t)VPPMSense; // Byte meno significativo di VPPMSense
        msg.data[6] = (uint8_t)(BPS >> 8); // Byte più significativo di BPS
        msg.data[7] = (uint8_t)BPS; // Byte meno significativo di BPS


        xQueueSend(canTxASQueue, &msg, 0);
        /////////////////////////////////////////////////////////////////////////

        // INVIO VPPM SENSE E BPS //////////////////////////////////////////
        header.StdId = 452; // Gear position
        //header.DLC = 8;
        msg.header = header;

        msg.data[0] = (uint8_t)(EBSAir1 >> 8); // Byte più significativo di EBSAir1
        msg.data[1] = (uint8_t)EBSAir1; // Byte meno significativo di EBSAir1
        msg.data[2] = (uint8_t)(EBSAir2 >> 8); // Byte più significativo di EBSAir2
        msg.data[3] = (uint8_t)EBSAir2; // Byte meno significativo di EBSAir2
        msg.data[4] = (uint8_t)(ADC_AUX1 >> 8); // Byte più significativo di ADC_AUX1
        msg.data[5] = (uint8_t)ADC_AUX1; // Byte meno significativo di ADC_AUX1
        msg.data[6] = (uint8_t)(ADC_AUX2 >> 8); // Byte più significativo di ADC_AUX2
        msg.data[7] = (uint8_t)ADC_AUX2; // Byte meno significativo di ADC_AUX2

        xQueueSend(canTxASQueue, &msg, 0); // TODO: check the queue
        /////////////////////////////////////////////////////////////////////////

        vTaskDelayUntil( &xLastWakeTime, xFrequency);
    }

}