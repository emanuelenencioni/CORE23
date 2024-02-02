#include "cmsis_os.h"
#include "main.h"
#include "task.h"
#include "semphr.h"



#ifndef CANHANDLERTASK_H_
#define CANHANDLERTASK_H_

// main code of the CanHandlerTask

#define CAN_QUEUE_SIZE 10

typedef struct{
uint16_t	Lambda;
float		CutoffV;
float		MAP;
uint16_t	MTS;
uint16_t	RPM;
uint16_t	TPS;
float		FPS;
float		OPS;
float 		IGN;
float 		WTS;
float 		VCC;
} EngineCANBuffer;

typedef struct{
	uint8_t reqUpShift;
	uint8_t reqDownShift;
	uint8_t reqState;
	uint8_t selectedMission;

}ASCANBuffer;

void canHandlerThread(void *argument);

void initEngineCAN();
void initASCAN();

void engineCanRxhandler();
void engineCanTxHandler();

void ASCanRxHandler();
void ASCanTxHandler();

// variables



// check for can already initialized


#endif