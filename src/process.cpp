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

// TODO: Return this process's ID
int Process::Pid() { return this->pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() {
    int hz = sysconf(_SC_CLK_TCK);
    float totalTime = LinuxParser::ActiveJiffies(this->pid_) / hz;
    auto seconds = LinuxParser::UpTime() - (LinuxParser::UpTime(this->pid_) / hz);
    return totalTime / seconds;
}

// TODO: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(this->pid_); }

// TODO: Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(this->pid_); }

// TODO: Return the user (name) that generated this process
string Process::User() {
    return LinuxParser::User(uid_);
}

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() {
    int hz = sysconf(_SC_CLK_TCK);
    long seconds = LinuxParser::UpTime() - (LinuxParser::UpTime(this->pid_) / hz);
    return seconds;
}

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const {
    return cpuUtilization_ > a.cpuUtilization_;
}
