#ifndef _GPIO_API_
#define _GPIO_API_

#include <string>
#include <unistd.h>
#include "run.h"

using namespace std;

bool getGpioValue(int id);

string getGpioDirection(int id);

void setGpioDirection(int id, string val);

void setGpioValue(int id, bool val);

void exportGpio(int id);

#endif
