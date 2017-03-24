#include "gpio.h"

using namespace std;

bool getGpioValue(int id){
	string result = run("cat "+pastaGpio+"/gpio"+to_string(id)+"/value");
	return result == "1";
}

string getGpioDirection(int id){
	string result = run("cat "+pastaGpio+"/gpio"+to_string(id)+"/direction");
	return result;
}

void setGpioDirection(int id, string val){
	if(val == "in" || val == "out"){
		run("echo " + val + " > " + pastaGpio + "/gpio"+to_string(id)+"/direction");
	}
}

void setGpioValue(int id, bool val){
	setGpioDirection(id, "out");
	string newVal;
	if(val) {
		newVal = "1";
	}
	else{
		newVal = "0";
	}
	cout << "run(" << "echo " << newVal << " > " << pastaGpio << "/gpio" << to_string(id) << "/value)" << endl;
	run("echo " + newVal + " > " + pastaGpio + "/gpio"+to_string(id)+"/value");
}

void exportGpio(int id){
	run("echo "+to_string(id)+" > "+pastaGpio+"/export");
}