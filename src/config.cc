/*
 * This file is part of Build Gear.
 *
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
#include <sstream>
#include <string>
#include <stdexcept>
#include <list>
#include <stdio.h>
#include <errno.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "buildgear/config.h"

CConfig::CConfig()
{
   name = "";

   // Default commands
   download = false;
   build = false;
   clean = false;
   show = false;
   init = false;

   // Default download options
   download_timeout = 20;
   download_retry = 3;
   download_mirror = "";
   download_mirror_first = "no";
   download_connections = 4;

   // Default build options
   keep_work = "no";
   update_checksum = "no";
   update_footprint = "no";
   no_strip = "no";
   no_fakeroot = false;

   // Default show options
   build_order = false;
   download_order = false;
   dependency_circle = false;
   readme = false;
   show_version = false;
   log = false;
   log_tail = false;

   // Misc defaults
   all = false;

   default_name_prefix = "cross/";
   source_dir = SOURCE_DIR;

   parallel_builds = 1;
}

void CConfig::CorrectName()
{
   // Prepend default name prefix if none is provided
   if ((CConfig::name.find("cross/") != 0) &&
      (CConfig::name.find("native/") != 0))
      CConfig::name = CConfig::default_name_prefix + CConfig::name;
}

void CConfig::CorrectSourceDir(void)
{
   // Replace "~" with $HOME value
   if (CConfig::source_dir.find("~/") == 0)
   {
      FILE *fp;
      char line_buffer[PATH_MAX];
      string command = "echo $HOME";
      string home;
      
      fp = popen(command.c_str(), "r");
      if (fp == NULL)
         throw std::runtime_error(strerror(errno));
      
      while (fgets(line_buffer, PATH_MAX, fp) != NULL)
      {
         home = line_buffer;
         stripChar(home, '\n');
      }
      
      pclose(fp);

      if (home != "")
      {
         CConfig::source_dir.erase(0,1);
         CConfig::source_dir = home + CConfig::source_dir;
      }
   }
}

void CConfig::GuessBuildSystem(void)
{
      FILE *fp;
      char line_buffer[PATH_MAX];
      
      fp = popen(CONFIG_GUESS_SCRIPT, "r");
      if (fp == NULL)
         throw std::runtime_error(strerror(errno));
      
      while (fgets(line_buffer, PATH_MAX, fp) != NULL)
      {
         build_system = line_buffer;
         stripChar(build_system, '\n');
      }
      
      pclose(fp);
}
