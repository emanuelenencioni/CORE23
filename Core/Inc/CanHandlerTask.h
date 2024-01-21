#ifdef CANHANDLERTASK_H_
#define CANHANDLERTASK_H_

// main code of the CanHandlerTask

#define CAN_QUEUE_SIZE = 10

void canHandlerThread(void *argument);

void initEngineCAN();
void initASCAN();


// variables



// check for can already initialized
bool canInitialized;

#endif