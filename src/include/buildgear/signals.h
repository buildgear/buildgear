#ifndef SIGNALS_H
#define SIGNALS_H

#include "buildgear/config.h"
#include <string>

using namespace std;

class CSignals
{
   public:
      void Install(void);
      static void sigIntHandler(int signum);
   private:
};

#endif
