Dangerous process monitor and killer designed to be used on a Beaglebone-Black, to display resource usage levels using LEDs and offer an input buttom to kill the process who is using the most resources.

To build for local arch:
- make

To cross-build for arm:
- make arm

We HIGHLY recommend using g++ >= 5.0.

------------------------------------------------
The main class is SysMonitor:

SysMonitor will monitor the resource (CPU or Memory) usage on the /proc
pseudo-file system.
Levels of usage:
0 ->  25% - Green
25% ->  50% - Yellow
50% ->  75% - Red
75% ->  100%  - Panic State

SysMonitor has 3 analogic outputs and one input:
- A green LED that will be on when the usage is Green, a yellow LED and a red LED who work the same way. When in panic state, the three LEDs will blink.
- While in the Panic State, the PanicButton will be enabled. This button is capable of sending a SIGTERM to the process who is using the most resources, when pressed down.

SysMonitor communicates with the LEDs and the button through gpio's. Their IDs are listed bellow:
const int panicButtonGPIO = 47;
const int redAlertGPIO = 45;
const int yellowAlertGPIO = 69;
const int greenAlertGPIO = 66;

After starting the instance, call SysMonitor.start() to start the monitor.
