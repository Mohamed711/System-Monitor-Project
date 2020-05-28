#include "processor.h"
#include "linux_parser.h"

// Return the aggregate CPU utilization
float Processor::Utilization() {
    long CurrectActiveJiffies = LinuxParser::ActiveJiffies();
    long CurrectTotalJiffies = LinuxParser::Jiffies();

    float AverageUtil = (CurrectActiveJiffies - PrevActiveJiffies) /
                        (float)(CurrectTotalJiffies - PrevTotalJiffies);

    // Update latest read values
    PrevActiveJiffies = CurrectActiveJiffies;
    PrevTotalJiffies = CurrectTotalJiffies;

    return AverageUtil;
}