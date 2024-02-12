#include "CanHandlerTask.h"

// Engine CAN
extern CAN_HandleTypeDef hcan1;
extern osMutexId_t EngCanSemHandle;

osMessageQueueId_t canTxEngineQueue;
uint32_t TxMailboxEng;


CAN_FilterTypeDef CANFilterEngine;

// Buffer in sezione critica
EngineCANBuffer EngCANBuffer;


// AS CAN
extern CAN_HandleTypeDef hcan2;

extern osMutexId_t ASCanSemHandle;
osMessageQueueId_t canTxASQueue;
uint32_t TxMailboxAS;

CAN_FilterTypeDef CANFilterAS;

// Buffer in sezione critica
ASCANBuffer AutCanBuffer;

CANMessage rxMsg, txMsg;
// Buffer for all the message incoming from the CAN connected to the engine.

uint16_t mailSize;



uint8_t canInitialized = 0;
uint8_t counter;


void canHandlerThread(void *argument){

	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 500;

    if (!canInitialized){
		counter = 0;
		initASCAN();
		initEngineCAN();
        canInitialized = 1;
	}
	
	uint32_t mailSize;

	xLastWakeTime = xTaskGetTickCount();
	while(1) {

		vTaskDelayUntil( &xLastWakeTime, xFrequency); //Periodic task
		// Engine CAN

		if(xSemaphoreTake(EngCanSemHandle, (TickType_t) 0) == pdTRUE){
			engineCanRxhandler();
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


// TODO riconfigurare
void initEngineCAN(){
    
	// // Lambda + CutOffV (259)
    addFilterCAN(&CANFilterEngine, &hcan1, counter++, 0x0103 << 5, 0);

	// // // // MAP + FPS + OPS (261)
    addFilterCAN(&CANFilterEngine, &hcan1, counter++, 0x0105 << 5, 0);

	// // // // WTS + MTS (262)
	addFilterCAN(&CANFilterEngine, &hcan1, counter++, 0x0106 << 5, 0);

	// // // // VCC (263)
	addFilterCAN(&CANFilterEngine, &hcan1, counter++, 0x0107 << 5, 0);

	// // // // RPM + TPS (266)
	addFilterCAN(&CANFilterEngine, &hcan1, counter++, 0x010A << 5, 0);

	// HAL_CAN_ConfigFilter(&hcan2, &CANFilterEngine);
	// // Start CAN
	HAL_CAN_Start(&hcan1);
	if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK) __NOP(); //TODO Error Handler
}

void initASCAN(){

	// Start CAN

	if(xSemaphoreTake(ASCanSemHandle, (TickType_t) WAIT_FOR_PILOT_STATE) == pdTRUE){
        // TODO init value of the buffer
		AutCanBuffer.reqMode = 0; // NotSelected
		xSemaphoreGive(ASCanSemHandle);
    }
	
    addFilterCAN(&CANFilterAS, &hcan2, counter++, 0x012C << 5, 1);
	
	addFilterCAN(&CANFilterAS, &hcan2, counter++, 0x0122 << 5, 1);
	addFilterCAN(&CANFilterAS, &hcan2, counter++, 0x012D << 5, 1);
	addFilterCAN(&CANFilterAS, &hcan2, counter++, 0x012E << 5, 1);
    addFilterCAN(&CANFilterAS, &hcan2, counter++, 0x012F << 5, 1);


	HAL_CAN_Start(&hcan2);
	if (HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO1_MSG_PENDING) != HAL_OK) __NOP(); //error_handler(3);
}

/**
 * @brief function that generateW
 * 
 * @param filterBank 
 * @param filterID 
 */
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
	CAN_Filter->FilterScale = CAN_FILTERSCALE_32BIT;
	HAL_CAN_ConfigFilter(hcan, CAN_Filter);
}

/**
 * @brief code for handling the incoming data from the engine CAN
 * 
 */
void engineCanRxhandler(){ // TODO vedere se gli id sono giusti e anche i relativi campi dato
	if(HAL_CAN_GetRxFifoFillLevel(&hcan1, CAN_RX_FIFO0) > 0) {
			HAL_GPIO_WritePin(BRAKE_LIGHT_GPIO_Port, BRAKE_LIGHT_Pin, GPIO_PIN_SET);
			vTaskDelay(200 /  portTICK_PERIOD_MS);
			HAL_GPIO_WritePin(BRAKE_LIGHT_GPIO_Port, BRAKE_LIGHT_Pin, GPIO_PIN_RESET);


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

void ASCanTxHandler(){
	
	mailSize = osMessageQueueGetCount(canTxEngineQueue);
	if(mailSize > 0)
	{
		if(osMessageQueueGet(canTxEngineQueue, &txMsg, NULL, 0) == osOK)
		{
			// Invia il messaggio CAN
			if(HAL_CAN_AddTxMessage(&hcan2, &txMsg.header, txMsg.data, &TxMailbox) != HAL_OK)
			{
				// TODO Gestisci errore di trasmissione
			}
		}
	}
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan){
	
	HAL_CAN_GetRxMessage(hcan,CAN_RX_FIFO0, &rxMsg.header, rxMsg.data);

}

void ASCanRxHandler(){
		if(HAL_CAN_GetRxMessage(&hcan2, CAN_RX_FIFO1, &rxMsg.header, rxMsg.data) == HAL_OK) {
			uint32_t id =  rxMsg.header.StdId;
			uint8_t* data = rxMsg.data;
			HAL_GPIO_WritePin(BRAKE_LIGHT_GPIO_Port, BRAKE_LIGHT_Pin, GPIO_PIN_SET);
			vTaskDelay(1000 /  portTICK_PERIOD_MS);
			HAL_GPIO_WritePin(BRAKE_LIGHT_GPIO_Port, BRAKE_LIGHT_Pin, GPIO_PIN_RESET);
			switch (id){
				case 290:
					AutCanBuffer.reqMode = data[0];
					break;
				case 300: // 0x0300
					// TODO request_clutchFollow(data[1] << 8 | data[0]);
					HAL_GPIO_WritePin(BRAKE_LIGHT_GPIO_Port, BRAKE_LIGHT_Pin, GPIO_PIN_SET);
					vTaskDelay(1000 /  portTICK_PERIOD_MS);
					HAL_GPIO_WritePin(BRAKE_LIGHT_GPIO_Port, BRAKE_LIGHT_Pin, GPIO_PIN_RESET);
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
					//OTHER:
					break;
			}
		}
}