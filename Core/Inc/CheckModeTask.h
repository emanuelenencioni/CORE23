#ifndef CHECKMODETASK_H_
#define CHECKMODETASK_H_


#include "CanHandlerTask.h"


#define WAIT_FOR_PILOT_STATE 50 


enum Mode {NotSelected = 0, Manual = 1, Autonomous = 2};

void checkModeThread(void* argument);

/**
 * @brief Function that send the corrispective error code to the Pilot23 board.
 * 
 * @param errorCode Look at the CAN-ID table for reference
 */
void sendErrorToPilot(uint8_t errorCode);

void readModeData();

#endif