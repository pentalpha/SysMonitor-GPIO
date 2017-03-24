/*
Pitágoras Alves, March 2017, UFRN.

SysMonitor will monitor the resource (CPU or Memory) usage on the /proc
pseudo-file system.
Levels of usage:
0 ->  25% - Green
25% ->  50% - Yellow
50% ->  75% - Red
75% ->  100%  - Panic State

SysMonitor has 3 analogic outputs and one input
A green LED that will be on when the usage is Green, a yellow LED and a red LED
who work the same way. When in panic state, the three LEDs will blink.
While in the Panic State, the PanicButton will be enabled. This button is
capable of sending a SIGTERM to the process who is using the most resources,
when pressed down.
SysMonitor communicates with the LEDs and the button through gpio's. Their IDs
are listed bellow.

After starting the instance, call SysMonitor.start() to start the monitor.
*/
#ifndef _SYS_MONITOR_
#define _SYS_MONITOR_

#include <atomic>
#include <linux/unistd.h>
#include <linux/kernel.h>
#include <sys/sysinfo.h>
#include <errno.h>
#include <iostream>
#include <chrono>
#include <future>
#include <fstream>
#include <string>
#include <map>
#include <unistd.h>
#include <signal.h>
#include <thread>
#include "tinydir.h"
#include "gpio.h"

/*
Stores info about a specific process
*/
struct ProcInfo{
  int pid;
  char state;
  //the last percentage of cpu usage
  float lastCPU;
  //time the processor dedicated to the process until now in seconds
  double totalTime;
  //the last time these values where updated
  std::chrono::system_clock::time_point lastCheck;
  //RAM memory in KB
  double totalMemory;
};

enum AlertMode{
  NONE, GREEN, YELLOW, RED
};

const int panicButtonGPIO = 47;
const int redAlertGPIO = 45;
const int yellowAlertGPIO = 69;
const int greenAlertGPIO = 66;

void setValueRed(bool value);
void setValueYellow(bool value);
void setValueGreen(bool value);
bool getPanicButton();

class SysMonitor{
public:
  AlertMode alertMode;
  std::atomic_bool panicMode;
  //memMode is for monitoring memory usage
  std::atomic_bool memMode;
  //and cpuMode for CPU usage
  std::atomic_bool cpuMode;
  std::atomic_bool quitFlag;
  //the process that can be killed by the button
  std::atomic_int dangerousProc;

  //current system hardware info
  double hertzPerSecond = sysconf(_SC_CLK_TCK);
  long pageSize = sysconf(_SC_PAGE_SIZE)/1024;
  double totalSysMemory = sysconf(_SC_PHYS_PAGES) * (sysconf(_SC_PAGE_SIZE)/1024);
  double nCPUs = sysconf(_SC_NPROCESSORS_ONLN);
  /*
  * Builds and starts the SysMonitor
  * mode  true for CPU mode, false for memMode.
  * Also prints some info about the system to stdout.
  */
  SysMonitor(bool mode = true)
  : panicMode(false), dangerousProc(-1), cpuMode(mode), memMode(!mode),
  alertMode(NONE), quitFlag(false)
  {
    std::cout << "CPUs\t" << nCPUs
    << "\ttotalSysMemory\t" << totalSysMemory/(1024*1024)
    << "GB\thertzPerSecond\t" << hertzPerSecond
    << std::endl;
  }

  /*
  Starts monitoring.
  Press q + ENTER to stop
  */
  void inline start(){
    thread watchForQ(&SysMonitor::quitWatcher, this);
    watchForQ.detach();
    scanProcs();
  }

  ~SysMonitor(){

  }

  void updateInfo(ProcInfo* info, const char* pathC);
  void updateInfo(ProcInfo* info, std::string path);
  void updateInfo(ProcInfo* info);
protected:
  //all the data on the processes
  std::map<int, ProcInfo*> procs;

  //Starts a watcher for the panic button
  void watchPanicButton();
  /*Watcher to be runned on a thread
  Verifies the value of the panic button file
  And may send kill to dangerousProc
  */
  void panicButtonWatcher();
  //Starts doing the panic sign
  void doPanicSign();
  //Turns the LEDs on and off repeatedly
  void playPanicSign();
  /* Scans /proc to get the stats of the processes */
  void scanProcs();
  void addNewProcs();
  void sendKillTo(int proc);
  void redAlert(float resource);
  void yellowAlert(float resource);
  void greenAlert(float resource);
  static void lightOnlyRedLED();
  static void lightOnlyYellowLED();
  static void lightOnlyGreenLED();
  static void lightNoLEDs();
  bool getButtonState();
  /*
  Constantly verifies for a 'q\n' on stdin, to update quitFlag
  */
  void quitWatcher();
};

#endif
