#include <string>
#include <unistd.h>
#include "run.h"

using namespace std;

string pastaGpio = "/sys/class/gpio";
int greenID = 66;
int yellowID = 69;
int redID = 45;
int buttonID = 47;
/*bool getGpioValue(int id){
	string result = run("cat "+pastaGpio+"/gpio"+to_string(id)+"/value");
	return result == "1";
}
string getGpioDirection(int id){
	string result = run("cat "+pastaGpio+"/gpio"+to_string(id)+"/direction");
	return result;
}
*/
void setGpioDirection(int id, string val){
	run("echo " + val + " > " + pastaGpio + "/gpio"+to_string(id)+"/direction");
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
	cout << "str = " << "echo " << newVal << " > " << pastaGpio << "/gpio" << to_string(id) << "/value" << endl;
	run("echo " + newVal + " > " + pastaGpio + "/gpio"+to_string(id)+"/value");
}



void exportGpio(int id){
	run("echo "+to_string(id)+" > "+pastaGpio+"/export");
}

int main(){

	setGpioValue(yellowID, 1);
	setGpioValue(redID, 1);
	setGpioValue(greenID, 1);
	sleep(2);
	setGpioValue(yellowID, 0);
	setGpioValue(redID, 0);
	setGpioValue(greenID, 0);	
	
	return 0;
}
