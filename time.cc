#include <iostream>
#include <time.h>
#include "buildgear/time.h"

void CTime::Start(void)
{
   struct timespec start;
   
   /* Get start clock time */
   if( clock_gettime( CLOCK_MONOTONIC, &start) == -1 )
      cerr << "clock_gettime error" << endl;

   CTime::time_start = start.tv_sec + 
                      start.tv_nsec * 0.000000001;
}

void CTime::Stop(void)
{
   struct timespec stop;
   
   /* Get stop clock time */
   if( clock_gettime( CLOCK_MONOTONIC, &stop) == -1 )
      cerr << "clock_gettime error" << endl;
      
   CTime::time_stop = stop.tv_sec +
                      stop.tv_nsec * 0.000000001;

}

void CTime::ShowElapsedTime(void)
{
   cout << "Elapsed time: " 
        << (CTime::time_stop - CTime::time_start)
        << " seconds"
        << endl;
}
