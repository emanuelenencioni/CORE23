#include "CanHandlerTask.h"

// Engine CAN
extern CAN_HandleTypeDef hcan1;
extern osMutexId_t EngCanSemHandle;


QueueHandle_t canTxEngQueue;
uint32_t TxMailboxEng;


CAN_FilterTypeDef CANFilterEngine;

// Buffer for all the message incoming from the CAN connected to the engine. In critical section.
EngineCANBuffer EngCANBuffer;


// AS CAN
extern CAN_HandleTypeDef hcan2;

extern osMutexId_t ASCanSemHandle;

QueueHandle_t canTxASQueue;
uint32_t TxMailboxAS;

CAN_FilterTypeDef CANFilterAS;

// Buffer for all the message incoming from the CAN connected to the AS. In critical section.
ASCANBuffer AutCanBuffer;

CANMessage rxMsg, txMsg;

//For segnaling errors
extern osMutexId_t CanErrSemHandle;

uint16_t mailSize;

uint8_t canInitialized = 0;
uint8_t counter;


// For error handling if CAN goes down or input/output error
uint8_t shutdownCMD = 0;

void canHandlerThread(void *argument){

	TickType_t xLastWakeTime;
	const TickType_t xFrequency =  20;

    if (!canInitialized){
		counter = 0;
		initEngineCAN();
		initASCAN();
        canInitialized = 1;
	}
	
	uint32_t mailSize;

	xLastWakeTime = xTaskGetTickCount();
	while(1) {

		vTaskDelayUntil( &xLastWakeTime, xFrequency); //Periodic task
		// Engine CAN
		if(xSemaphoreTake(EngCanSemHandle, (TickType_t) 0) == pdTRUE){
			engineCanRxHandler();
			xSemaphoreGive(EngCanSemHandle);
		}
		//AS CAN
		if(xSemaphoreTake(ASCanSemHandle, (TickType_t) 0) == pdTRUE){
			ASCanRxHandler();
			xSemaphoreGive(ASCanSemHandle);
		}

		engineCanTxHandler();
		ASCanTxHandler();
	}
}


void initEngineCAN(){
	//TODO update the filters to new ids
	canTxEngQueue = xQueueCreate(10, sizeof(CANMessage));
	// Lambda + CutOffV (259)
    addFilterCAN(&CANFilterEngine, &hcan1, counter++, 0x0103 << 5, 0);

	// MAP + FPS + OPS (261)
    addFilterCAN(&CANFilterEngine, &hcan1, counter++, 0x0105 << 5, 0);

	// WTS + ATS (262)
	addFilterCAN(&CANFilterEngine, &hcan1, counter++, 0x0106 << 5, 0);

	// VCC (263)
	addFilterCAN(&CANFilterEngine, &hcan1, counter++, 0x0107 << 5, 0);

	// RPM + TPS (266)
	addFilterCAN(&CANFilterEngine, &hcan1, counter++, 0x010A << 5, 0);
	
	//UPDATED FILTERS
	
	// Speed all 4 tyres, (513)
	addFilterCAN(&CANFilterEngine, &hcan1, counter++, 0x0209 << 5, 0);

	// // Start CAN
	HAL_CAN_Start(&hcan1);
	
}

void initASCAN(){

	// Start CAN
	canTxASQueue = xQueueCreate(10, sizeof(CANMessage));
	if(xSemaphoreTake(ASCanSemHandle, (TickType_t) WAIT_FOR_PILOT_STATE) == pdTRUE){
        // TODO init value of the buffer
		AutCanBuffer.reqMode = 0; // NotSelected
		xSemaphoreGive(ASCanSemHandle);
    }
	
    addFilterCAN(&CANFilterAS, &hcan2, counter++, 0x012C << 5, 1);
	
	// Mission status from PC
	addFilterCAN(&CANFilterAS, &hcan2, counter++, 0x015F << 5, 1);
	// Actual brake pressure
	addFilterCAN(&CANFilterAS, &hcan2, counter++, 0x0065 << 5, 1);
	addFilterCAN(&CANFilterAS, &hcan2, counter++, 0x0122 << 5, 1);
	addFilterCAN(&CANFilterAS, &hcan2, counter++, 0x012D << 5, 1);
	addFilterCAN(&CANFilterAS, &hcan2, counter++, 0x012E << 5, 1);
    addFilterCAN(&CANFilterAS, &hcan2, counter++, 0x012F << 5, 1);


	HAL_CAN_Start(&hcan2);
	
}

void addFilterCAN(CAN_FilterTypeDef* CAN_Filter, CAN_HandleTypeDef* hcan, uint32_t filterBank, uint32_t filterID, uint8_t fifo){
	if(fifo)
		CAN_Filter->FilterFIFOAssignment = CAN_FILTER_FIFO1;
	else
		CAN_Filter->FilterFIFOAssignment = CAN_FILTER_FIFO0;

    CAN_Filter->FilterBank = filterBank;
	CAN_Filter->FilterMode = CAN_FILTERMODE_IDLIST;
	CAN_Filter->FilterIdHigh = filterID;
	CAN_Filter->FilterIdLow  = 0;
	CAN_Filter->FilterActivation = ENABLE;
	CAN_Filter->FilterScale = CAN_FILTERSCALE_16BIT;
	HAL_CAN_ConfigFilter(hcan, CAN_Filter);
}


void engineCanRxHandler(){ // TODO vedere se gli id sono giusti e anche i relativi campi dato
		while(HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &rxMsg.header, rxMsg.data) == HAL_OK) {
			uint32_t id = rxMsg.header.StdId;
			uint8_t* data = rxMsg.data;
			switch (id){
				case 259:
					EngCANBuffer.Lambda = (data[1] << 8 | data[0])/1000;
					EngCANBuffer.CutoffV = (data[3] << 8 | data[2])/816.f;
					break;

				case 261:
					EngCANBuffer.MAP =  (float)(data[1] << 8 | data[0]);
					EngCANBuffer.FPS =  (float)((data[7] << 8 | data[6])/100);
					EngCANBuffer.OPS =  (float)(data[5] << 8 | data[4]);
					break;

				case 262:
					uint16_t local_CAN_WTS =  (float)(data[1] << 8 | data[0])/10;
					uint16_t local_CAN_ATS = (data[3] << 8 | data[2])/10;
					EngCANBuffer.WTS = local_CAN_WTS;
					EngCANBuffer.ATS = local_CAN_ATS;
					break;

				case 263:
					EngCANBuffer.VCC = (float)((data[3] << 8 | data[2])/1000);
					break;
				case 266:
					EngCANBuffer.RPM = (data[1] << 8 | data[0]);
					EngCANBuffer.TPS = (data[3] << 8 | data[2])/10;

					// TODO spostare questo in Gear Task 
					//useCutoff = EngCANBuffer.CAN_RPM > 6000 ? 1 : 0; 	//Disable cutoff if rpm < 6000
					break;

				case 270:	

					EngCANBuffer.IGN = (data[3] << 8 | data[2])/16.f;
					break;
				case 521: //Speed big endian o little endian?
					EngCANBuffer.speedFrontL = (data[1] << 8 | data[0]);
					EngCANBuffer.speedFrontR = (data[3] << 8 | data[2]);
					EngCANBuffer.speedBackL = (data[5] << 8 | data[4]);
					EngCANBuffer.speedBackR = (data[7] << 8 | data[6]);
					break;

			}
		}
}


void engineCanTxHandler(){
	if(uxQueueMessagesWaiting(canTxEngQueue) > 0) {
		while(xQueueReceive(canTxEngQueue, &txMsg, 0) == pdTRUE) {// TODO capire criticità del while
			if(HAL_CAN_AddTxMessage(&hcan2, &txMsg.header, txMsg.data, &TxMailboxAS) == HAL_ERROR) {
				if(xSemaphoreTake(CanErrSemHandle, (TickType_t) 0)){
						//TODO add check in ErrorHandManTask
					xSemaphoreGive(CanErrSemHandle);
				}
			}
		}
	}
}

void ASCanRxHandler(){
		while(HAL_CAN_GetRxMessage(&hcan2, CAN_RX_FIFO1, &rxMsg.header, rxMsg.data) == HAL_OK) {
			uint32_t id =  rxMsg.header.StdId;
			uint8_t* data = rxMsg.data;
			switch (id){
				case 101:
					AutCanBuffer.brakePressureFront = data[0];
					AutCanBuffer.brakePressureBack = data[1];
					break;
				case 290:
					AutCanBuffer.reqMode = data[0];
					AutCanBuffer.selectedMission = data[1];

					break;
				case 300: // 
					AutCanBuffer.clutchRequest = data[0];
					break;
				case 301:
					//PADDLES:
					if(data[0] == 1) //upshift
					
						//request_upShift();
						AutCanBuffer.reqUpShift = 1;
					else if (data[0] == 2) //downshift
						//request_downShift();
						AutCanBuffer.reqDownShift = 1;
					break;
				case 302:
					//BUTTONS:
					if (data[0] == 1) 		//neutral
						__NOP();//neutral();
					else if (data[0] == 2) //drive
						__NOP(); //drive();
					else if (data[0] == 3) //launch
						//clutch_release(slow);
						__NOP();
					else if (data[0] == 4) //tag
						__NOP(); //TODO: implementare messaggio
					break;
				case 303:
					if(data[0]==1 || data[0]==0)
						AutCanBuffer.forcedGear == data[0];
					break;

				case 351:
					AutCanBuffer.missionStatus = data[0];
					break;
			}
		}
}

void ASCanTxHandler(){
	if(uxQueueMessagesWaiting(canTxASQueue) > 0) {
		while(xQueueReceive(canTxASQueue, &txMsg, 0) == pdTRUE) {// TODO capire criticità del while
			if(HAL_CAN_AddTxMessage(&hcan2, &txMsg.header, txMsg.data, &TxMailboxAS) != HAL_OK){
				if(xSemaphoreTake(CanErrSemHandle, (TickType_t) 0)){
						//TODO add check in ErrorHandManTask
					xSemaphoreGive(CanErrSemHandle);
				}
			}
		}
	}
}
