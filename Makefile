all:
	g++ -pthread --std=c++11 -o sysmonitor sysmonitor.cpp main.cpp
