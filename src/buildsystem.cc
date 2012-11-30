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
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include "buildgear/config.h"
#include "buildgear/buildsystem.h"
#include "buildgear/buildfile.h"
#include "buildgear/buildfiles.h"
#include "buildgear/filesystem.h"

extern CFileSystem FileSystem;

void CBuildSystem::Check(void)
{
   FILE *fp;
   char cmd_result[5];
   int status, i=0;
   string tool[] = {      "bash",
                     "sha256sum",
                           "sed",
                           "awk",
                           "tar",
                          "gzip",
                         "bzip2",
                            "xz",
                         "unzip",
                          "lzma",
                          "diff",
                          "sort",
                          "find",
                          "grep",
                          "file",
                           "cat",
                            "mv",
                            "rm",
                            "cp",
                         "xargs",
                       "dirname",
                      "basename",
                       "unalias",
                      "readlink",
                          "tail",
                              "" };

   while (tool[i] != "")
   {
      string command = "type " + tool[i] + " &> /dev/null";
      
      status = system(command.c_str());
      
      if (status != 0)
      {
         cout << "Failed ('" << tool[i] << "' is not found)\n";
         cout << "\nPlease install missing tool.\n\n";
         exit(EXIT_FAILURE);
      }
      
      i++;
   }

   // Make sure that /bin/sh points to bash!
   fp = popen("readlink /bin/sh", "r");
   if (fp == NULL)
   {
      cout << "Error popen" << endl;
      exit(EXIT_FAILURE);
   }

   if (fgets(cmd_result, 5, fp) == NULL)
   {
      cout << "fgets error" << endl;
      exit(EXIT_FAILURE);
   }

   pclose(fp);
   string result(cmd_result);

   if (result != "bash")
   {
      cout << "\n\nError: Default shell is not bash.\n\n";
      cout << "Please make sure that /bin/sh links to bash.\n\n";
      exit(EXIT_FAILURE);
   }
}

void CBuildSystem::CallCheck(list<CBuildFile*> *buildfiles)
{
   int result;
   string command;
   list<CBuildFile*>::iterator it;
   
   /* Traverse through all buildfiles */
   for (it=buildfiles->begin(); it!=buildfiles->end(); it++)
   {
      if ((*it)->check_function == "yes")
      {
         command = "bash --norc --noprofile -O extglob -c 'source " BUILD_FILES_CONFIG "; source " + (*it)->filename + "; check '";
         result = system(command.c_str());
         if (result != 0)
         {
            cout << endl << "Build system check failed for " << (*it)->filename << endl << endl;
            cout << "Please install missing tools or libraries." << endl << endl;
            exit(EXIT_FAILURE);
         }
      }
   }
}

void CBuildSystem::ShowLog(void)
{
   string log_file = Config.root + "/" + string(BUILD_LOG);

   if (Config.log_tail)
      FileSystem.Tail(log_file);
   else
      FileSystem.Cat(log_file);
}
