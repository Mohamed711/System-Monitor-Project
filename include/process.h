#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(int pid) : pid_(pid) {};
  int Pid() const;                             
  std::string User();                    
  std::string Command();                 
  float CpuUtilization() const;                
  std::string Ram();                     
  long int UpTime();                     
  float getCpuUtilValue();
  void ComputeCpuUtil();
  bool operator<(Process const& a) const;

 private:
    int pid_;
    float CpuUtilValue;
    float PrevProcessTotalTime{0};
    float PrevSeconds{0};
};

#endif