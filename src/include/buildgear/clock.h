#ifndef CLOCK_H
#define CLOCK_H

using namespace std;

class CClock
{
   public:
      void Start(void);
      void Stop(void);
      void ShowElapsedTime(void);
   private:
      double time_start;
      double time_stop;
};

#endif
