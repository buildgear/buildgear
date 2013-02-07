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
#include <iomanip>
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
#include "buildgear/configfile.h"

CConfig::CConfig()
{
   name = "";

   // Default commands
   download = false;
   build = false;
   clean = false;
   show = false;
   init = false;
   config = false;

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
   mismatch = false;
   footprint = false;
   checksum = false;

   // Default config options
   global = false;
   unset = false;
   blist = false;
   key = "";
   value = "";

   // Misc defaults
   all = false;

   default_name_prefix = "cross/";
   source_dir = SOURCE_DIR;

   parallel_builds = 1;

   home_dir = getenv("HOME");

   pid = getpid();
   output_fifo = "/tmp/buildgear." + to_string(pid) + ".fifo";
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

void CConfig::SetConfig(void)
{
   list<CConfigOption*>::iterator it;
   bool key_valid = false;
   bool value_valid = false;

   for (it = ConfigFile.options.begin(); it != ConfigFile.options.end(); it++)
   {
      if ((*it)->key == Config.key)
      {
         key_valid = true;
         if ((*it)->check == NULL || (*it)->check(Config.value) || Config.unset)
         {
            value_valid = true;
         }
      }
   }

   if (!key_valid)
   {
      cout << endl << "Error: The key '" << Config.key << "' is not a valid option.\n";
      exit(EXIT_FAILURE);
   }

   if (!value_valid)
   {
      cout << endl << "Error: The value '" << Config.value << "' is not valid for option '";
      cout << Config.key << "'.\n";
      exit(EXIT_FAILURE);
   }

   if (Config.global)
      ConfigFile.Update(Config.home_dir + GLOBAL_CONFIG_FILE);
   else
      ConfigFile.Update(LOCAL_CONFIG_FILE);

}

void CConfig::List(void)
{
   CConfig defaults, global, local;
   list<CConfigOption*>::iterator it;
   uint max_len = 0;
   string line;
   ostringstream out;
   defaults = Config;

   ConfigFile.Parse(Config.home_dir + GLOBAL_CONFIG_FILE);

   global = Config;

   ConfigFile.Parse(LOCAL_CONFIG_FILE);

   local = Config;

   cout << endl;

   line = CONFIG_KEY_SOURCE_DIR  "=";
   line += Config.source_dir;
   if (line.size() > max_len)
      max_len = line.size();

   line = CONFIG_KEY_DOWNLOAD_TIMEOUT  "=";
   line += Config.download_timeout;
   if (line.size() > max_len)
      max_len = line.size();

   line = CONFIG_KEY_DOWNLOAD_RETRY  "=";
   line += Config.download_retry;
   if (line.size() > max_len)
      max_len = line.size();

   line = CONFIG_KEY_DOWNLOAD_MIRROR_FIRST  "=";
   line += Config.download_mirror_first;
   if (line.size() > max_len)
      max_len = line.size();

   line = CONFIG_KEY_DOWNLOAD_CONNECTIONS  "=";
   line += Config.download_connections;
   if (line.size() > max_len)
      max_len = line.size();

   line = CONFIG_KEY_PARALLEL_BUILDS  "=";
   line += Config.parallel_builds;
   if (line.size() > max_len)
      max_len = line.size();

   out << CONFIG_KEY_SOURCE_DIR << "=";
   out << Config.source_dir;
   cout << out.str() << setw(max_len - out.tellp()) << "";
   if (local.source_dir == defaults.source_dir)
      cout << " [default]" << endl;
   else if (local.source_dir == global.source_dir)
      cout << " [global]" << endl;
   else
      cout << " [local]" << endl;

   out.str("");
   out << CONFIG_KEY_DOWNLOAD_TIMEOUT << "=";
   out << Config.download_timeout;
   cout << out.str() << setw(max_len - out.tellp()) << "";
   if (local.download_timeout == defaults.download_timeout)
      cout << " [default]" << endl;
   else if (local.download_timeout == global.download_timeout)
      cout << " [global]" << endl;
   else
      cout << " [local]" << endl;

   out.str("");
   out << CONFIG_KEY_DOWNLOAD_RETRY << "=";
   out << Config.download_retry;
   cout << out.str() << setw(max_len - out.tellp()) << "";
   if (local.download_retry == defaults.download_retry)
      cout << " [default]" << endl;
   else if (local.download_retry == global.download_retry)
      cout << " [global]" << endl;
   else
      cout << " [local]" << endl;

   out.str("");
   out << CONFIG_KEY_DOWNLOAD_MIRROR_FIRST << "=";
   out << Config.download_mirror_first;
   cout << out.str() << setw(max_len - out.tellp()) << "";
   if (local.download_mirror_first == defaults.download_mirror_first)
      cout << " [default]" << endl;
   else if (local.download_mirror_first == global.download_mirror_first)
      cout << " [global]" << endl;
   else
      cout << " [local]" << endl;

   out.str("");
   out << CONFIG_KEY_DOWNLOAD_CONNECTIONS << "=";
   out << Config.download_connections;
   cout << out.str() << setw(max_len - out.tellp()) << "";
   if (local.download_connections == defaults.download_connections)
      cout << " [default]" << endl;
   else if (local.download_connections == global.download_connections)
      cout << " [global]" << endl;
   else
      cout << " [local]" << endl;

   out.str("");
   out << CONFIG_KEY_PARALLEL_BUILDS << "=";
   out << Config.parallel_builds;
   cout << out.str() << setw(max_len - out.tellp()) << "";
   if (local.parallel_builds == defaults.parallel_builds)
      cout << " [default]" << endl;
   else if (local.parallel_builds == global.parallel_builds)
      cout << " [global]" << endl;
   else
      cout << " [local]" << endl;

   cout << endl;
}
