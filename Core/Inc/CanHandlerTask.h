#ifdef CANHANDLERTASK_H_
#define CANHANDLERTASK_H_

// main code of the CanHandlerTask

#define CAN_QUEUE_SIZE = 10

void canHandlerThread(void *argument);

void initEngineCAN();
void initASCAN();

void engineCanRxhandler();
void engineCanTxHandler();

void ASCanRxHandler();
void ASCanTxHandler();

// variables



// check for can already initialized
bool canInitialized;

#endif