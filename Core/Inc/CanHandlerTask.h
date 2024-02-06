#include "cmsis_os.h"
#include "cmsis_os2.h"
#include "main.h"
#include "task.h"
#include "semphr.h"


#ifndef CANHANDLERTASK_H_
#define CANHANDLERTASK_H_

// main code of the CanHandlerTask

#define CAN_QUEUE_SIZE 10
#define WAIT_FOR_PILOT_STATE 100

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
	uint8_t reqMode;
	uint8_t selectedMission;

}ASCANBuffer;

typedef struct {
    CAN_TxHeaderTypeDef header;
    uint8_t data[8];

} CANMessage;

/**
 * @brief Task that handle both CAN bus in the vehicle.
 * 
 * 
 */
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