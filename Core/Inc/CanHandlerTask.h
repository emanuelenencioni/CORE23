#include "cmsis_os.h"
#include "cmsis_os2.h"
#include "main.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

#ifndef CANHANDLERTASK_H_
#define CANHANDLERTASK_H_

// main code of the CanHandlerTask

#define CAN_QUEUE_SIZE 10
#define WAIT_FOR_PILOT_STATE 100


typedef struct{
	uint16_t	Lambda;
	float		MAP;
	uint8_t		ATS;
	uint16_t	RPM;
	uint16_t	TPS;
	float		FPS;
	float		OPS;
	float 		IGN;
	float 		WTS;
	float 		VCC;
	uint16_t speedFrontL;
	uint16_t speedFrontR;
	uint16_t speedBackL;
	uint16_t speedBackR;
} EngineCANBuffer;

typedef struct{
	uint8_t reqUpShift;
	uint8_t reqDownShift;
	uint8_t reqMode;
	uint8_t reqStart; // TODO add to CAN filter and so on
	uint8_t selectedMission;
	uint8_t missionStatus;
	uint8_t brakePressureFront;
	uint8_t brakePressureBack;
	uint64_t Heartbit;
	uint8_t forcedGear;
	uint8_t clutchRequest;
	uint8_t reqAcceleration;

}ASCANBuffer;

typedef struct {
    CAN_TxHeaderTypeDef header;
    uint8_t data[8];

} CANMessage;

/**
 * @brief Task that handle both CAN bus in the vehicle.
 */
void canHandlerThread(void *argument);

/**
 * @brief function that add a filter to the specified can bus
 * 
 * @param CAN_Filter the filter to apply
 * @param hcan the can bus
 * @param filterBank the index of the filter in the bank
 * @param filterID the can id 
 * @param fifo which fifo to save on, 0 = FIFO0, 1 = FIFO1
 */
void addFilterCAN(CAN_FilterTypeDef* CAN_Filter, CAN_HandleTypeDef* hcan, uint32_t filterBank, uint32_t filterID, uint8_t fifo);

void initEngineCAN();
void initASCAN();

void engineCanRxHandler();
void engineCanTxHandler();

void ASCanRxHandler();
void ASCanTxHandler();
// variables



// check for can already initialized


#endif