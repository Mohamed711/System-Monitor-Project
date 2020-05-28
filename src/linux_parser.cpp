#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);

  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }

  return kernel;
}

// Read the process Ids from integer folder names
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Read the system memory utilization
float LinuxParser::MemoryUtilization() {
  string line;
  string key;
  int value, MemoryTotal, MemoryFree;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      // Line Format is "key: value  kB"
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "MemTotal:") {
        MemoryTotal = value;
      }
      else if (key == "MemFree:"){
        MemoryFree = value;
        break;
      }

    }
  }

  return (MemoryTotal - MemoryFree)/(float)MemoryTotal;
}

// Read the system uptime
long LinuxParser::UpTime() {
  string line;
  long uptime = 0;

  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }

  return uptime;
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  return ActiveJiffies() + IdleJiffies();
}

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  vector<string> processStat;
  string value;
  string line;

  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    while (linestream >> value)
    {
      processStat.push_back(value);
    }
  }

  int totalTime = std::stoi(processStat[13])
                + std::stoi(processStat[14])
                + std::stoi(processStat[15])
                + std::stoi(processStat[16]);

  return totalTime;
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  std::vector<long> CpuUtil;
  CpuUtilization(CpuUtil);  
  return
      CpuUtil[0] + CpuUtil[1]
    + CpuUtil[2] + CpuUtil[5]
    + CpuUtil[6] + CpuUtil[7]
    + CpuUtil[8] + CpuUtil[9];
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  std::vector<long> CpuUtil;
  CpuUtilization(CpuUtil);
  return CpuUtil[3] + CpuUtil[4];
}

// Read CPU utilization
void LinuxParser::CpuUtilization(std::vector<long> & CpuValues) {
  string line;
  string key;
  string JiffiesVal;

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {

      // Read the first line for aggregate CPU Information
      std::istringstream linestream(line);
      linestream >> key; // Aggregate CPU

      for (int i = 0; i < 10; i++)
      {
        linestream >> JiffiesVal;
        CpuValues.push_back(std::stoi(JiffiesVal));
      }
    }
  }
}

// Read the total number of processes
int LinuxParser::TotalProcesses() {
  string line;
  string key;
  int TotalProcesses;

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "processes") {
        linestream >> TotalProcesses;
        break;
      }
    }
  }

  return TotalProcesses;
}

// Read the number of running processes
int LinuxParser::RunningProcesses() {
  string line;
  string key;
  int TotalProcesses;

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "procs_running") {
        linestream >> TotalProcesses;
        break;
      }
    }
  }

  return TotalProcesses;
}

// Read the command associated with a process
string LinuxParser::Command(int pid) {
  string CommandLine = "";

  std::ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()) {
     std::getline(filestream, CommandLine);
  }

  return CommandLine;
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string Ram = "";
  string line;
  string key;
  long RamSize;

  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "VmSize:") {
        linestream >> RamSize;
        break;
      }
    }
  }

  // Return the RAM usage in MB
  return to_string(RamSize/1000);
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string line;
  string key;
  int UidNo;

  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "Uid:") {
        linestream >> UidNo;
        break;
      }
    }
  }

  return to_string(UidNo);
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string line;
  string input_user, input_pass, input_uid;
  string uid = Uid(pid);

  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> input_user >> input_pass >> input_uid;
      if (input_uid == uid) {
        break;
      }
    }
  }

  return input_user;
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  const int UpTimeIndex = 22;
  int ParamIndex = 1;
  long UpTime;
  string ProcessVal;
  string line;

  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    while (ParamIndex != UpTimeIndex)
    {
      linestream >> ProcessVal;
      ParamIndex++;
    }

    // Read the UpTime stat
    linestream >> UpTime;
  }

  // Return the UpTime in seconds
  return UpTime/sysconf(_SC_CLK_TCK);
}