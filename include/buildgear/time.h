#ifndef TIME_H
#define TIME_H

using namespace std;

class CTime
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
