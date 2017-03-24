#ifndef _GPIO_API_
#define _GPIO_API_

#include <string>
#include <unistd.h>
#include "run.h"

using namespace std;

string pastaGpio = "/sys/class/gpio";
int greenID = 66;
int yellowID = 69;
int redID = 45;
int buttonID = 47;

bool getGpioValue(int id);

string getGpioDirection(int id);

void setGpioDirection(int id, string val);

void setGpioValue(int id, bool val);

void exportGpio(int id);

/*int main(){

	setGpioValue(yellowID, 1);
	setGpioValue(redID, 1);
	setGpioValue(greenID, 1);
	sleep(2);
	setGpioValue(yellowID, 0);
	setGpioValue(redID, 0);
	setGpioValue(greenID, 0);

	return 0;
}*/

#endif
