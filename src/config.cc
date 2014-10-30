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

   home_dir = getenv("HOME");

   // Default buildgear options
   bg_config_default[CONFIG_KEY_DOWNLOAD_TIMEOUT] = "20";
   bg_config_default[CONFIG_KEY_DOWNLOAD_RETRY] = "3";
   bg_config_default[CONFIG_KEY_DOWNLOAD_MIRROR_FIRST] = "no";
   bg_config_default[CONFIG_KEY_DOWNLOAD_CONNECTIONS] = "4";
   bg_config_default[CONFIG_KEY_DEFAULT_NAME_PREFIX] = "cross/";
   bg_config_default[CONFIG_KEY_SOURCE_DIR] = SOURCE_DIR;
   bg_config_default[CONFIG_KEY_PARALLEL_BUILDS] = "1";
   bg_config_default[CONFIG_KEY_CERTIFICATE_DIR] = "";
   bg_config_default[CONFIG_KEY_SSH_PUBLIC_KEYFILE] = home_dir + "/.ssh/id_rsa.pub";
   bg_config_default[CONFIG_KEY_SSH_PRIVATE_KEYFILE] = home_dir + "/.ssh/id_rsa";
   bg_config_default[CONFIG_KEY_LOG_ROTATION] = "5";

   // Default buildfiles options
   bf_config[CONFIG_KEY_DOWNLOAD_MIRROR] = "";

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
   manifest_plain_text = false;
   manifest_xml = false;
   manifest_html = false;

   // Default config options
   global = false;
   unset = false;
   blist = false;
   key = "";
   value = "";

   // Misc defaults
   all = false;

   pid = getpid();
   tmp_dir = "/tmp/buildgear." + to_string(pid) + "/";

   bg_config = bg_config_default;
}

void CConfig::CorrectName()
{
   // Prepend default name prefix if none is provided
   if ((CConfig::name.find("cross/") != 0) &&
      (CConfig::name.find("native/") != 0))
      CConfig::name = CConfig::bg_config[CONFIG_KEY_DEFAULT_NAME_PREFIX] + CConfig::name;
}

void CConfig::CorrectSourceDir(void)
{
   // Replace "~" with $HOME value
   if (CConfig::bg_config[CONFIG_KEY_SOURCE_DIR].find("~/") == 0)
   {
      if (home_dir != "")
      {
         CConfig::bg_config[CONFIG_KEY_SOURCE_DIR].erase(0,1);
         CConfig::bg_config[CONFIG_KEY_SOURCE_DIR] = home_dir + CConfig::bg_config[CONFIG_KEY_SOURCE_DIR];
      }
   }

   // Prepend build root directory if a relative path is configured
   if (CConfig::bg_config[CONFIG_KEY_SOURCE_DIR][0] != '/')
         CConfig::bg_config[CONFIG_KEY_SOURCE_DIR] = root + "/" +  CConfig::bg_config[CONFIG_KEY_SOURCE_DIR];
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
         bf_config[CONFIG_KEY_BUILD] = line_buffer;
         stripChar(bf_config[CONFIG_KEY_BUILD], '\n');
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
   list<CConfigOption*>::iterator it;
   uint max_len = 0;
   string line;
   ostringstream out;

   ConfigFile.Parse(Config.home_dir + GLOBAL_CONFIG_FILE);
   ConfigFile.Parse(LOCAL_CONFIG_FILE);

   cout << endl;

   // Find length of longest key=value string
   for (it = ConfigFile.options.begin(); it != ConfigFile.options.end(); it++)
   {
      line = (*it)->key + "=";
      line += Config.bg_config[(*it)->key];
      if (line.size() > max_len)
         max_len = line.size();
   }

   for (it = ConfigFile.options.begin(); it != ConfigFile.options.end(); it++)
   {
      out.str("");
      out << (*it)->key << "=";
      out << Config.bg_config[(*it)->key];
      cout << out.str() << setw(max_len - out.tellp()) << "";

      if (!bg_config_local[(*it)->key].empty())
         cout << " [local]" << endl;
      else if (!bg_config_global[(*it)->key].empty())
         cout << " [global]" << endl;
      else
         cout << " [default]" << endl;
   }

   cout << endl;
}
