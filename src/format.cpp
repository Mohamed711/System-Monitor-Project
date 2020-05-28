#include <string>

#include "format.h"

using std::string;
using std::to_string;

// Change seconds to HH:MM:SS format
string Format::ElapsedTime(long seconds) {
    long hours, minutes;

    minutes = seconds/60;
    hours = (minutes/60);

    minutes = (minutes) % 60; // exclude hour minutes
    seconds = seconds % 60;

    return to_string(hours) + ":" + to_string(minutes)
                            + ":" + to_string(seconds);
}