#ifndef ASACCTASK_H_
#define ASACCTASK_H_

#include "CanHandlerTask.h"

void ASAccThread(void* argument);

void launchRoutine();
void slowRoutine();

float getSpeed();

#endif