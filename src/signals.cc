#include "config.h"
#include <iostream>
#include <stdlib.h>
#include <signal.h>
#include "buildgear/config.h"
#include "buildgear/signals.h"

struct sigaction new_action, old_action;

void CSignals::sigIntHandler(int signum)
{
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
