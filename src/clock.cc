#include <iostream>
#include <iomanip>
#include <time.h>
#include "buildgear/clock.h"

void CClock::Start(void)
{
   struct timespec start;
   
   /* Get start clock time */
   if( clock_gettime( CLOCK_MONOTONIC, &start) == -1 )
      cerr << "clock_gettime error" << endl;

   time_start = start.tv_sec + start.tv_nsec * 0.000000001;
}

void CClock::Stop(void)
{
   struct timespec stop;
   
   /* Get stop clock time */
   if( clock_gettime( CLOCK_MONOTONIC, &stop) == -1 )
      cerr << "clock_gettime error" << endl;
      
   time_stop = stop.tv_sec + stop.tv_nsec * 0.000000001;
}

void CClock::ShowElapsedTime(void)
{
   double seconds = time_stop - time_start;
   int hours = seconds / 3600;
   int minutes = (seconds - hours * 3600) / 60;
   seconds = seconds - hours * 3600 - minutes * 60;
   
   cout << "Elapsed time: "
        << hours   << "h "
        << minutes << "m "
        << fixed
        << setprecision(0)
        << seconds << "s "
        << endl << endl;
}
