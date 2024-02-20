#ifndef ASSTATEHANDTASK_H_
#define ASSTATEHANDTASK_H_


#include "CanHandlerTask.h"




void ASStateHandThread(void* argument);

// Functions to update the current AS state (Change the ASSI too)
void setASEmergency();
void setASFinished();
void setASOff();
void setASReady();
void setASDriving();

#endif