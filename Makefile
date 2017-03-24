all:
	g++ -pthread --std=c++11 -o sysmonitor run.cpp gpio.cpp sysmonitor.cpp main.cpp
