#include <iostream>
#include <string>
#include "sysmonitor.h"

using namespace std;

int main(int argc, char const *argv[]) {
  if(argc == 2){
    std::string resource(argv[1]);
    if(resource == "memory"){
      SysMonitor monitor(false);
    }else if(resource == "cpu"){
      SysMonitor monitor(true);
    }else{
      cout << "Unknown resource: " << argv[1] << "\n";
    }
  }else{
    cout << "Usage:\n"
          <<"\tsysmonitor memory\n"
          <<"\tsysmonitor cpu\n";
  }
  return 0;
}
