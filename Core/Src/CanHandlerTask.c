#include "CanHandlerTask.h"
#include "cmsis_os.h"
#include "main.h"



extern CAN_HandleTypeDef hcan1;
extern CAN_HandleTypeDef hcan2;
extern SPI_HandleTypeDef hspi3;

osMessageQueueId_t canRxQueue;
osMessageQueueId_t canTxQueue;

typedef struct {
    CAN_TxHeaderTypeDef header;
    uint8_t data[8];

} CANMessage;
CAN_FilterTypeDef CANFilterEngine;
CAN_FilterTypeDef CANFilterAS;

canInitialized = 0;

/**
 * @brief Task that handle both CAN bus in the vehicle.
 * 
 * 
 */
void canHandlerThread(void *argument){

    if (!canInitialized){
		initASCAN();
		initEngineCAN();
        canInitialized = 1;
	}

    while(1){
        
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