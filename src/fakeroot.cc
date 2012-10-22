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
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include "buildgear/config.h"
#include "buildgear/fakeroot.h"

void CFakeroot::Respawn(int argc, char *argv[])
{
   FILE *fp;
   char cmd_result[5];
   int status;

   // Check for installed fakeroot utility
   status = system("type fakeroot 2>&1 > /dev/null");
   if (status != 0)
   {
      cout << "\nFakeroot is not found - please install.\n\n";
      exit(EXIT_FAILURE);
   }

   // Check user is "root"
   fp = popen("whoami", "r");
   if (fp == NULL)
   {
      cout << "Error" << endl;
      exit(EXIT_FAILURE);
   }

   if (fgets(cmd_result, 5, fp) == NULL)
   {
      cout << "fgets error" << endl;
      exit(EXIT_FAILURE);
   }
   
   pclose(fp);
   string result(cmd_result);

   // If not "root" user then respawn into fakeroot session
   if (result != "root")
   {
      int i=0;
      char **shifted_argv = (char **) malloc(sizeof(argv[0]) * (argc+2));

      // Create shifted argv pointer array
      for (i=0; i<argc; i++)
         shifted_argv[i+1]=argv[i];

      shifted_argv[0] = (char *) "fakeroot";

      // End pointer array
      shifted_argv[argc+1] = NULL;

      execvp("fakeroot", shifted_argv);
      perror("execvp() failure!\n\n");
      exit(EXIT_FAILURE);
   }
}
