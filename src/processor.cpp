#include "processor.h"

#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
    /**
     * cpuStats:
     *  0: user
     *  1: nice
     *  2: system
     *  3: idle
     *  4: iowait
     *  5: irq
     *  6: softirq
     *  7: steal
     *  8: guest
     *  9: guest_nice
     */
    std::vector<int> cpuStats;
    for (std::string stat : LinuxParser::CpuUtilization()) {
        cpuStats.emplace_back(std::stoi(stat));
    }

    int idleAllTime = cpuStats[3] + cpuStats[4];
    int systemAllTime = cpuStats[2] + cpuStats[5] + cpuStats[6];
    int totalTime = cpuStats[0] + cpuStats[1] + systemAllTime + idleAllTime + cpuStats[7];

    float totalDiff = totalTime - this->prevTotalTime_;
    float idleDiff = idleAllTime - this->prevIdleTime_;

    this->prevTotalTime_ = totalTime;
    this->prevIdleTime_ = idleAllTime;

    return (totalDiff - idleDiff)/totalDiff;
}