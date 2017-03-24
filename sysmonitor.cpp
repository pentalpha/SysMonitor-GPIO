#include "sysmonitor.h"

using namespace std;

//Starts a watcher for the panic button
void SysMonitor::watchPanicButton(){
  function<void()> watcherFunc = [this](){
    panicButtonWatcher();
  };
  async(watcherFunc);
}

/*Watcher to be runned on a thread
Verifies the value of the panic button file
And may send kill to dangerousProc
*/
void SysMonitor::panicButtonWatcher(){
  cout << "Watching for panic button" << endl;
  while(panicMode){
    if(getButtonState()){
      cout << "Killing\t" << dangerousProc << endl;
      sendKillTo(dangerousProc);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds{500});
  }
}

bool SysMonitor::getButtonState(){
  return true;
}

void SysMonitor::doPanicSign(){
  function<void()> playerFunc = [this](){
    playPanicSign();
  };
  async(playerFunc);
}

void SysMonitor::playPanicSign(){
  while(panicMode){
    cout << "TURNOFF LED";
    std::this_thread::sleep_for(std::chrono::milliseconds{500});
    cout << "TURNON LED";
    std::this_thread::sleep_for(std::chrono::milliseconds{500});
  }
}

void SysMonitor::lightOnlyRedLED(){
  cout << "RED ALERT" << endl;
}

void SysMonitor::lightOnlyYellowLED(){
  cout << "YELLOW ALERT" << endl;
}

void SysMonitor::lightOnlyGreenLED(){
  cout << "GREEN ALERT" << endl;
}

/* Scans /proc to get the stats of the processes */
void SysMonitor::scanProcs(){
  while(true){
    int memEater, cpuEater;
    memEater = cpuEater = -1;
    double totalCPU = 0.0;
    float totalMem = 0.0;
    for(pair<int, ProcInfo*> element : procs){
      if(element.second->state == 'R' || element.second->state == 'S'){
        chrono::system_clock::time_point previousCheck = element.second->lastCheck;
        double previousTime = element.second->totalTime;
        updateInfo(element.second);
        double timeSpent = element.second->totalTime - previousTime;
        auto timeSince = std::chrono::duration_cast<std::chrono::seconds>(element.second->lastCheck-previousCheck);
        element.second->lastCPU = timeSpent / timeSince.count();
        totalCPU += element.second->lastCPU;
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
    //cout << "res: " << resource << endl;
    if(resource >= 0.75){
      panicMode = true;
      alertMode = NONE;
      cout << "resource usage:\t" << resource << endl;
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
    addNewProcs();
    std::this_thread::sleep_for(std::chrono::seconds{1});
  }
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
  ifstream stream = std::ifstream(path + "/stat", std::ios::in);

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
  stream = std::ifstream(path + "/statm", std::ios::in);
  stream >> temp;
  stream >> residentPages;
  stream >> sharedPages;
  info->totalMemory = (residentPages*pageSize)-(sharedPages*pageSize);

  info->lastCPU = 0.0;
}


void SysMonitor::sendKillTo(int proc){
  kill(proc, SIGTERM);
}
