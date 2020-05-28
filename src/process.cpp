#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"

using std::string;
using std::to_string;
using std::vector;

// Return this process's ID
int Process::Pid() const{
    return pid_;
}

// Compute the process CPU utilization
void Process::ComputeCpuUtil()
{
    // Process total running time in seconds
    float CurrectProcessTotalTime = LinuxParser::ActiveJiffies(pid_)/(float)sysconf(_SC_CLK_TCK);
  
    // The proccess available time (seconds = uptime of processor - (proccess start time / Hertz)
    float CurrectSeconds = LinuxParser::UpTime() - (UpTime()/(float)sysconf(_SC_CLK_TCK));

    // Process CPU Utilization = proccess running time / total time available for the process
    CpuUtilValue = (CurrectProcessTotalTime - PrevProcessTotalTime)/
                        (float) (CurrectSeconds - PrevSeconds);

    // Update latest read values
    PrevProcessTotalTime = CurrectProcessTotalTime;
    PrevSeconds = CurrectSeconds;
}

// Return this process's CPU utilization
float Process::CpuUtilization() const{
    return CpuUtilValue;
}

// Return the command that generated this process
string Process::Command() {
    return LinuxParser::Command(pid_);
}

// Return this process's memory utilization
string Process::Ram() {
    return LinuxParser::Ram(pid_);
}

// Return the user (name) that generated this process
string Process::User() {
    return LinuxParser::User(pid_) ;
}

// Return the age of this process (in seconds)
long int Process::UpTime() {
    return LinuxParser::UpTime(pid_);
}

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {
    return CpuUtilValue < a.CpuUtilization() ? false : true;
}