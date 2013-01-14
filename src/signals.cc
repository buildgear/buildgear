/*
 * Copyright (C) 2011-2013  Martin Lund
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include "config.h"
#include <iostream>
#include <stdlib.h>
#include <signal.h>
#include "buildgear/config.h"
#include "buildgear/signals.h"
#include "buildgear/cursor.h"
#include "buildgear/buildmanager.h"

struct sigaction new_action, old_action;

void CSignals::sigIntHandler(int signum)
{
   pthread_mutex_lock(&cout_mutex);
   Cursor.clear_below();
   cout << "\n\nInterrupt signal received - stopped!\n";

   exit(EXIT_SUCCESS);
}

void CSignals::Install(void)
{
   /* Set up the structure to specify the new action */
   new_action.sa_handler = sigIntHandler;
   sigemptyset(&new_action.sa_mask);
   new_action.sa_flags = 0;

   /* Install new action handler */
   sigaction (SIGINT, NULL, &old_action);
   if (old_action.sa_handler != SIG_IGN)
      sigaction (SIGINT, &new_action, NULL);
   else
      cout << "Cannot handle SIGINT!" << endl;
}
