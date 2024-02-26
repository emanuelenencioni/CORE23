#include "cmsis_os.h"
#include "cmsis_os2.h"
#include "main.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

#ifndef ADCTASK_H_
#define ADCTASK_H_

#define ADC_BUFFER_SIZE 12




typedef struct{
    uint16_t desmo1;
    uint16_t desmo2;
    float clutchOil;
    float GearUpAir;
    float APPS1;
    float APPS2;
    float VPPMSense;
    float BPS;
    float EBSAir1;
    float EBSAir2;
    uint16_t ADC_AUX1;
    uint16_t ADC_AUX2;
} ADCBuffer;
float map(float x, float in_min, float in_max, float out_min, float out_max);
void ADCThread(void* argument);
#endif