/*
 * Copyright (C) 2011-2012  Martin Lund
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "config.h"
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
