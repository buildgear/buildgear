/*
 * This file is part of Build Gear.
 *
 * Copyright (c) 2011-2013  Martin Lund
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
#include <fstream>
#include <string>
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
   char cmd_result[20];
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

   if (fgets(cmd_result, 20, fp) == NULL)
   {
      cout << "fgets error" << endl;
      exit(EXIT_FAILURE);
   }

   pclose(fp);
   string result(cmd_result);

   if (result.find("bash") == string::npos)
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
   else if (Config.mismatch)
      ShowLogMismatch(log_file);
   else
   {
      if (!FileSystem.Cat(log_file))
      {
         cout << endl << "Error: Could not show log from " << log_file << endl;
         exit(EXIT_FAILURE);
      }
   }
}

void CBuildSystem::ShowLogMismatch(string log_file)
{
   ifstream inFile(log_file);
   string current_line, next_line;
   bool print = false;
   bool first = true;

   if (!inFile)
   {
      cout << "Error: Could not open " << log_file << endl;
      exit(EXIT_FAILURE);
   }

   while (getline(inFile, current_line))
   {
      if (print)
      {
         if (current_line.find("======>") == 0)
            print = false;
         else
            cout << current_line << endl;
      } else
      {
         if ( (current_line.find("======> Footprint") == 0) ||
               current_line.find("======> Checksum") == 0)
         {
            if (!getline(inFile, next_line))
            {
               cout << "Error: Log terminated abruptly" << endl;
               exit(EXIT_FAILURE);
            }

            // If the following line contains mismatch
            if (next_line.find("mismatch found") != string::npos)
            {
               print = true;

               if (first)
                  first = false;
               else
                  cout << endl;

               cout << current_line << endl << next_line << endl;
            }
         }
      }
   }
}
