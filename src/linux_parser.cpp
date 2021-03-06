#include <dirent.h>
#include <math.h>
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

// BONUS: Update this to use std::filesystem
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

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string line;
  
  float memTotal;
  float memFree;

  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    string key;
    string value;
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal:") {
          memTotal = std::stof(value);
        } else if (key == "MemFree:") {
          memFree = std::stof(value);
        }
      }
    }
  }

  return (memTotal - memFree)/memTotal;
}

long LinuxParser::UpTime() {
  double uptime = 0;
  FILE* file = fopen(&(kProcDirectory + kUptimeFilename)[0], "r");
  if (file) {
    int n = fscanf(file, "%64lf", &uptime);
    fclose(file);
    if (n <= 0) { return 0; } // Didnt find a match
  }
  return (long) floor(uptime);
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  long total;

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    string line;
    string key;
    int i;

    std::getline(filestream, line);
    std::istringstream linestream(line);
    while (linestream >> key) {
      // skip the cpu key and guest jiffies
      // guest jiffies are included in user/nice jiffie
      if (key != "cpu" && i < 8) {
        total += std::stol(key);
      }
      i++;
    }
  }

  return total;
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  string line;
  vector<string> stats;

  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    string value;

    std::getline(filestream, line);
    std::istringstream linestream(line);
    while (linestream >> value) {
      stats.emplace_back(value);
    }

    long utime = std::stol(stats[13]);
    long stime = std::stol(stats[14]);
    long cutime = std::stol(stats[15]);
    long cstime = std::stol(stats[16]);

    return utime + stime + cutime + cstime;
  }
  return 0;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  vector<long> jiffies;

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    string line;
    string key;

    std::getline(filestream, line);
    std::istringstream linestream(line);
    while (linestream >> key) {
      if (key != "cpu") {
        jiffies.emplace_back(std::stol(key));
      }
    }
  }

  return jiffies[0] + jiffies[1] + jiffies[2];
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<long> jiffies;

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    string line;
    string key;

    std::getline(filestream, line);
    std::istringstream linestream(line);
    while (linestream >> key) {
      if (key != "cpu") {
        jiffies.emplace_back(std::stol(key));
      }
    }
  }

  return jiffies[3] + jiffies[4];
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  string line;
  vector<string> jiffies;

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    string key;

    std::getline(filestream, line);
    std::istringstream linestream(line);
    while (linestream >> key) {
      if (key != "cpu") {
        jiffies.emplace_back(key);
      }
    }
  }

  return jiffies;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    string line;
    string key;
    string value;

    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") {
          return std::stoi(value);
        }
      }
    }
  }

  return 0;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    string line;
    string key;
    string value;

    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          return std::stoi(value);
        }
      }
    }
  }

  return 0;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  string line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
  }
  return line;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  string value;

  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    string line;
    string key;

    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmSize:") {
          return std::to_string(std::stoi(value) / 1024);
        }
      }
    }
  }
  return value;
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  string line;
  string value;

  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    string key;
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid:") {
          return value;
        }
      }
    }
  }
  return value;
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(string _uid) {
  string line;
  string name;

  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    string junk;
    string uid;

    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> name >> junk >> uid) {
        if (_uid == uid) {
          return name;
        }
      }
    }
  }
  return name;
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  string line;
  vector<string> stats;

  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    string value;

    std::getline(filestream, line);
    std::istringstream linestream(line);
    while (linestream >> value) {
      stats.emplace_back(value);
    }

    return std::stol(stats[21]);
  }
  return 0;
}