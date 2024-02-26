#ifndef ASACCTASK_H_
#define ASACCTASK_H_

#include "CanHandlerTask.h"

void ASAccThread(void* argument);

void LaunchRoutine();
void slowRoutine();

float getSpeed();

#endif