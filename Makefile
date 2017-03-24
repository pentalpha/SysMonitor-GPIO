all:
	g++ -pthread --std=c++11 -o sysmonitor gpio.cpp sysmonitor.cpp main.cpp
