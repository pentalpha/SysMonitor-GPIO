/*
Pitágoras Alves, March 2017, UFRN.
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

struct ProcInfo{
  int pid;
  char state;
  float lastCPU;
  //time the processor dedicated to the process until now in seconds
  double totalTime;
  std::chrono::system_clock::time_point lastCheck;
  //in KB
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
  std::atomic_bool memMode;
  std::atomic_bool cpuMode;
  //std::atomic_bool panicButton;
  std::atomic_int dangerousProc;

  double hertzPerSecond = sysconf(_SC_CLK_TCK);
  long pageSize = sysconf(_SC_PAGE_SIZE)/1024;
  double totalSysMemory = sysconf(_SC_PHYS_PAGES) * (sysconf(_SC_PAGE_SIZE)/1024);
  double nCPUs = sysconf(_SC_NPROCESSORS_ONLN);
  /*
  * Builds and starts the SysMonitor
  * mode  true for CPU mode, false for memMode
  */
  SysMonitor(bool mode = true)
  : panicMode(false), dangerousProc(-1), cpuMode(mode), memMode(!mode), alertMode(NONE)
  {
    scanProcs();
  }

  ~SysMonitor(){

  }

  void updateInfo(ProcInfo* info, const char* pathC);
  void updateInfo(ProcInfo* info, std::string path);
  void updateInfo(ProcInfo* info);
protected:
  //void updateUptime();

  std::map<int, ProcInfo*> procs;

  //Starts a watcher for the panic button
  void watchPanicButton();
  /*Watcher to be runned on a thread
  Verifies the value of the panic button file
  And may send kill to dangerousProc
  */
  void panicButtonWatcher();
  void doPanicSign();
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
};

#endif
