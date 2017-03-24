#include "sysmonitor.h"

using namespace std;

void setValueRed(bool value){
  setGpioValue(redAlertGPIO, value);
}

void setValueYellow(bool value){
  setGpioValue(yellowAlertGPIO, value);
}

void setValueGreen(bool value){
  setGpioValue(greenAlertGPIO, value);
}

bool getPanicButton(){
  return getGpioValue(panicButtonGPIO);
}

//Starts a watcher for the panic button
void SysMonitor::watchPanicButton(){
  //cout << "Watching for panic button outside " << endl;
  thread t(&SysMonitor::panicButtonWatcher, this);
  //cout << "Going to detach\n" << endl;
  t.detach();
}

void SysMonitor::quitWatcher(){
  char c;
  do{
    cin >> c;
  }while(c != 'q');
  quitFlag = true;
}

/*Watcher to be runned on a thread
Verifies the value of the panic button file
And may send kill to dangerousProc
*/
void SysMonitor::panicButtonWatcher(){
  //cout << "Watching for panic button inside" << endl;
  do{
    if(getGpioValue(panicButtonGPIO)){
      cout << "Killing\t" << dangerousProc << endl;
      sendKillTo(dangerousProc);
      panicMode = false;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds{500});
  }while(panicMode);
}

bool SysMonitor::getButtonState(){
  return getPanicButton();
}

void SysMonitor::doPanicSign(){
  thread t(&SysMonitor::playPanicSign, this);
  //cout << "Going to detach\n" << endl;
  t.detach();
}

void SysMonitor::playPanicSign(){
  while(panicMode){
    //cout << "TURNOFF LED\n";
    setValueGreen(0);
    setValueRed(0);
    setValueYellow(0);
    std::this_thread::sleep_for(std::chrono::milliseconds{500});
    //cout << "TURNON LED\n";
    setValueGreen(1);
    setValueRed(1);
    setValueYellow(1);
    std::this_thread::sleep_for(std::chrono::milliseconds{500});
  }
  setValueGreen(0);
  setValueRed(0);
  setValueYellow(0);
}

void SysMonitor::lightOnlyRedLED(){
  setValueGreen(0);
  setValueRed(1);
  setValueYellow(0);
  cout << "RED ALERT" << endl;
}

void SysMonitor::lightOnlyYellowLED(){
  setValueGreen(0);
  setValueRed(0);
  setValueYellow(1);
  cout << "YELLOW ALERT" << endl;
}

void SysMonitor::lightOnlyGreenLED(){
  setValueGreen(1);
  setValueRed(0);
  setValueYellow(0);
  cout << "GREEN ALERT" << endl;
}

void SysMonitor::lightNoLEDs(){
  setValueGreen(0);
  setValueRed(0);
  setValueYellow(0);
  cout << "NO ALERT" << endl;
}

/* Scans /proc to get the stats of the processes */
void SysMonitor::scanProcs(){
  while(!quitFlag){
    int memEater, cpuEater;
    memEater = cpuEater = -1;
    double totalCPU = 0.0;
    //double totalTimeSpent = 0.0;
    float totalMem = 0.0;
    for(pair<int, ProcInfo*> element : procs){
      if(element.second->state == 'R' || element.second->state == 'S'){
        chrono::system_clock::time_point previousCheck = element.second->lastCheck;
        double previousTime = element.second->totalTime;
        updateInfo(element.second);
        double timeSpent = element.second->totalTime - previousTime;
        auto timeSince = std::chrono::duration_cast<std::chrono::seconds>(element.second->lastCheck-previousCheck);
        element.second->lastCPU = timeSpent / timeSince.count();
        if(element.second->state == 'R') {
          totalCPU += element.second->lastCPU;
        }
        //totalTimeSpent+=
        totalMem += element.second->totalMemory;
        if(memEater == -1 || cpuEater == -1){
          memEater = cpuEater = element.first;
        }else{
          if(element.second->totalMemory > procs[memEater]->totalMemory){
            memEater = element.first;
          }
          if(element.second->lastCPU > procs[cpuEater]->lastCPU){
            cpuEater = element.first;
          }
        }
      }
    }

    double resource = totalCPU / nCPUs;
    if(memMode){
      resource = totalMem / totalSysMemory;
    }
    //cout << "resource usage: \t" << resource*100 << "%" << endl;
    if(panicMode){
      if(resource < 0.75){
        panicMode = false;
      }else{

      }
    }else{
      if(resource >= 0.75){
        panicMode = true;
        alertMode = NONE;
        //cout << "resource usage:\t" << resource << endl;
        if(memMode==false){
          dangerousProc = cpuEater;
        }else{
          dangerousProc = memEater;
        }
        watchPanicButton();
        doPanicSign();
      }else{
        panicMode = false;
        if(resource >= 0.50){
          redAlert(resource);
        }else if(resource >= 0.25){
          yellowAlert(resource);
        }else{
          //cout << "alert?\n";
          greenAlert(resource);
        }
      }
    }
    addNewProcs();
    std::this_thread::sleep_for(std::chrono::seconds{1});
  }
  lightNoLEDs();
}

void SysMonitor::redAlert(float resource){
  if(alertMode != RED){
    lightOnlyRedLED();
    alertMode = RED;
    //cout << "res: " << resource << endl;
  }
}

void SysMonitor::yellowAlert(float resource){
  if(alertMode != YELLOW){
    lightOnlyYellowLED();
    alertMode = YELLOW;
    //cout << "res: " << resource << endl;
  }
}

void SysMonitor::greenAlert(float resource){
  if(alertMode != GREEN){
    lightOnlyGreenLED();
    alertMode = GREEN;
    //cout << "res: " << resource << endl;
  }
}

void SysMonitor::addNewProcs(){
  tinydir_dir dir;
  tinydir_open(&dir, "/proc");

  while(dir.has_next){
    tinydir_file file;
    tinydir_readfile(&dir, &file);
    if(file.is_dir){
      string fileName = file.name;
      try{
        int idFound = stoi(fileName);
        if(procs.find(idFound) == procs.end()){
          ProcInfo* info = new ProcInfo;
          info->pid = idFound;
          updateInfo(info, file.path);
          procs[idFound] = info;
        }
      }catch(...){

      }
    }
    tinydir_next(&dir);
  }

  tinydir_close(&dir);
}

void SysMonitor::updateInfo(ProcInfo* info){
  updateInfo(info, "/proc/" + to_string(info->pid));
}

void SysMonitor::updateInfo(ProcInfo* info, const char* pathC){
  string a(pathC);
  updateInfo(info, a);
}

void SysMonitor::updateInfo(ProcInfo* info, string path){
  string pathWithStat = path + "/stat";
  auto stream = std::fstream(pathWithStat);

  string temp;
  int utime, stime, cutime, cstime, residentPages, sharedPages;
  stream >> temp;
  stream >> temp;
  stream >> info->state;
  stream >> temp;
  stream >> temp;
  stream >> temp;
  stream >> temp;
  stream >> temp;
  stream >> temp;
  stream >> temp;
  stream >> temp;
  stream >> temp;
  stream >> temp;
  stream >> utime;
  stream >> stime;
  info->totalTime = ((utime+stime)/hertzPerSecond);
  info->lastCheck = chrono::system_clock::now();
  string pathWithMem = path + "/statm";
  stream = std::fstream(pathWithMem);
  stream >> temp;
  stream >> residentPages;
  stream >> sharedPages;
  info->totalMemory = (residentPages*pageSize)-(sharedPages*pageSize);

  info->lastCPU = 0.0;
}


void SysMonitor::sendKillTo(int proc){
  kill(proc, SIGTERM);
}
