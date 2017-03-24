all:
	g++ -static-libgcc -static-libstdc++ -pthread --std=c++11 -o sysmonitor run.cpp gpio.cpp sysmonitor.cpp main.cpp
	g++ -o more50mb more50mb.cpp
arm:
	arm-linux-gnueabihf-g++ -static-libgcc -static-libstdc++ -pthread --std=c++11 -o sysmonitor_arm run.cpp gpio.cpp sysmonitor.cpp main.cpp
	arm-linux-gnueabihf-g++ -o more50mb_arm more50mb.cpp
