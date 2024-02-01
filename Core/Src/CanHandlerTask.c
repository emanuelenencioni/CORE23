#include "CanHandlerTask.h"
#include "cmsis_os.h"
#include "main.h"
#include "task.h"
#include "semphr.h"

// Engine CAN
extern CAN_HandleTypeDef hcan1;

extern osMutexId_t EngCanSemHandle;
osMessageQueueId_t canTxASQueue;


CAN_FilterTypeDef CANFilterEngine;

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

// Buffer in sezione critica
EngineCANBuffer EngCANBuffer;

// AS CAN
extern CAN_HandleTypeDef hcan2;

extern osMutexId_t ASCanSemHandle;
osMessageQueueId_t canTxEngineQueue;


CAN_FilterTypeDef CANFilterAS;

typedef struct{
	uint8_t reqUpShift;
	uint8_t reqDownShift;

}ASCANBuffer;


typedef struct {
    CAN_TxHeaderTypeDef header;
    uint8_t data[8];

} CANMessage;

CANMessage rxMsg, txMsg;
// Buffer for all the message incoming from the CAN connected to the engine.


canInitialized = 0;

uint16_t mailSize;

/**
 * @brief Task that handle both CAN bus in the vehicle.
 * 
 * 
 */
void canHandlerThread(void *argument){

	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 20;

    if (!canInitialized){
		initASCAN();
		initEngineCAN();
        canInitialized = 1;
	}
	
	uint32_t mailSize;

	xLastWakeTime = xTaskGetTickCount();
	while(1) {
		// Engine CAN

		xSemaphoreTake(EngCanSemHandle, (TickType_t) 0);
		engineCANRxhandler();
		xSemaphoreGive(EngCanSemHandle);

		


		//AS CAN
		xSemaphoreTake(ASCanSemHandle, (TickType_t) 0);
		ASCanRxHandler();
		xSemaphoreGive(ASCanSemHandle);


		engineCanTxHandler();
		ASCanRxHandler();

		vTaskDelayUntil(xLastWakeTime, xFrequency); //Periodic task
	}
}


// TODO riconfigurare
void initEngineCAN(){
    
	// Lambda + CutOffV (259)
    addFilterCAN(CANFilterEngine, hcan1, 0, 0x0103 << 5);

	// MAP + FPS + OPS (261)
    addFilterCAN(CANFilterEngine, &hcan1, 1, 0x0105 << 5);

	// WTS + MTS (262)
	addFilterCAN(CANFilterEngine, &hcan1, 2, 0x0106 << 5);

	// VCC (263)
	addFilterCAN(CANFilterEngine, &hcan1, 3, 0x0107 << 5);

	// RPM + TPS (266)
	addFilterCAN(CANFilterEngine, &hcan1, 4, 0x010A << 5);

	// Start CAN and enable the buffer notification
	HAL_CAN_Start(&hcan1);
	if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK) error_handler(3);
}

void initASCAN(){

	// Start CAN and enable the buffer notification

    addFilterCAN(CANFilterAS, &hcan2, 0, 300);

    addFilterCAN(CANFilterAS, &hcan2, 1, 301);

    addFilterCAN(CANFilterAS, &hcan2, 3, 303);
    addFilterCAN(CANFilterAS, &hcan2, 4, 304);

	HAL_CAN_Start(&hcan2);
	if (HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO1_MSG_PENDING) != HAL_OK) error_handler(3);
}

/**
 * @brief function that generate
 * 
 * @param filterBank 
 * @param filterID 
 */
void addFilterCAN(CAN_FilterTypeDef CAN_Filter, CAN_HandleTypeDef hcan, int filterBank, int filterID){
    CAN_Filter.FilterBank = filterBank;
	CAN_Filter.FilterMode = CAN_FILTERMODE_IDLIST;
	CAN_Filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
	CAN_Filter.FilterIdHigh = filterID;
	CAN_Filter.FilterIdLow  = 0;
	CAN_Filter.FilterScale = CAN_FILTERSCALE_32BIT;
	CAN_Filter.FilterActivation = ENABLE;

    HAL_CAN_ConfigFilter(&hcan, &CAN_Filter);
}

/**
 * @brief code for handling the incoming data from the engine CAN
 * 
 */
void engineCanRxhandler(){ // TODO vedere se gli id sono giusti e anche i relativi campi dato
	if(HAL_CAN_GetRxFifoFillLevel(&hcan1, CAN_RX_FIFO0) > 0) {
			if(HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &rxMsg.header, rxMsg.data) == HAL_OK) {
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
					uint16_t local_CAN_MTS = (data[3] << 8 | data[2])/10;
					EngCANBuffer.MTS = local_CAN_MTS;
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
			}
		}
	}
}

/**
 * @brief code for handling the message to the engine CAN
 * 
 */
void engineCanTxHandler(){
	mailSize = osMessageQueueGetCount(canTxEngineQueue);
	if(mailSize > 0)
	{
		if(osMessageQueueGet(canTxEngineQueue, &txMsg, NULL, 0) == osOK)
		{
			// Invia il messaggio CAN
			uint32_t TxMailbox;
			if(HAL_CAN_AddTxMessage(&hcan1, &txMsg.header, txMsg.data, &TxMailbox) != HAL_OK)
			{
				// TODO Gestisci errore di trasmissione
			}
		}
	}
}

void ASCanRxHandler(){
	if(HAL_CAN_GetRxFifoFillLevel(&hcan2, CAN_RX_FIFO0) > 0) {
			if(HAL_CAN_GetRxMessage(&hcan2, CAN_RX_FIFO0, &rxMsg.header, rxMsg.data) == HAL_OK) {
			uint32_t id =  rxMsg.header.StdId;
			uint8_t* data = rxMsg.data;
			switch (id){
				case 300:
					request_clutchFollow(data[1] << 8 | data[0]);
					break;
				case 301:
					//PADDLES:
					if(data[0] == 1) //upshift
						request_upShift();
					else if (data[0] == 2) //downshift
						request_downShift();
					break;
				case 302:
					//BUTTONS:
					if (data[0] == 1) 		//neutral
						neutral();
					else if (data[0] == 2) //drive
						drive();
					else if (data[0] == 3) //launch
						//clutch_release(slow);
						__NOP();
					else if (data[0] == 4) //tag
						__NOP(); //TODO: implementare messaggio
					break;
				case 303:
					//OTHER:
					break;
			}
		}
	}
}