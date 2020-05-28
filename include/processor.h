#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();

 private:
  long PrevActiveJiffies{0};
  long PrevTotalJiffies{0};
};

#endif