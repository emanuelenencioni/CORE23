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
    uint16_t clutchOil;
    uint16_t GearUpAir;
    uint8_t APPS1;
    uint8_t APPS2;
    uint8_t APPS3;
    uint16_t VPPMSense;
    uint16_t BPSFront;
    uint16_t BPSRear;
    uint16_t EBSAir1;
    uint16_t EBSAir2;
    uint16_t ADC_AUX1;
    uint16_t ADC_AUX2;
} ADCBuffer;
uint32_t map(uint32_t x, uint32_t in_min, uint32_t in_max, uint32_t out_min, uint32_t out_max);
void ADCThread(void* argument);
#endif