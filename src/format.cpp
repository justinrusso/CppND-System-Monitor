#include <math.h>
#include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long totalSeconds) {
    int hour = totalSeconds / 3600;
    totalSeconds = totalSeconds % 3600;
    int minute = totalSeconds / 60;
    int second = totalSeconds % 60;

    return std::to_string(hour) + ":" + std::to_string(minute) + ":" + std::to_string(second);
}
