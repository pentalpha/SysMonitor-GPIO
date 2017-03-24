all:
	g++ -static-libgcc -static-libstdc++ -pthread --std=c++11 -o sysmonitor run.cpp gpio.cpp sysmonitor.cpp main.cpp
arm:
	arm-linux-gnueabihf-g++ -static-libgcc -static-libstdc++ -pthread --std=c++11 -o sysmonitor_arm run.cpp gpio.cpp sysmonitor.cpp main.cpp
