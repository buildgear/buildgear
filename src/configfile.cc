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
#include <sstream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <list>
#include <stdio.h>
#include <errno.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include "buildgear/config.h"
#include "buildgear/configfile.h"

bool ValidBool(string value)
{
   try {
      if (value == "yes" || value == "no")
         return true;
   } catch(...)
   {
      return false;
   }
   return false;
}

bool ValidInt(string value)
{
   try {
      stoi(value);
      return true;
   } catch(...)
   {
      return false;
   }
   return false;
}

CConfigOption::CConfigOption(string key, sanity_function check)
{
   this->key = key;
   this->check = check;
}

CConfigFile::CConfigFile(void)
{
   options.push_back(new CConfigOption(CONFIG_KEY_SOURCE_DIR,              NULL));
   options.push_back(new CConfigOption(CONFIG_KEY_DOWNLOAD_MIRROR_FIRST,   ValidBool));
   options.push_back(new CConfigOption(CONFIG_KEY_DOWNLOAD_TIMEOUT,        ValidInt));
   options.push_back(new CConfigOption(CONFIG_KEY_DOWNLOAD_RETRY,          ValidInt));
   options.push_back(new CConfigOption(CONFIG_KEY_DOWNLOAD_CONNECTIONS,    ValidInt));
   options.push_back(new CConfigOption(CONFIG_KEY_PARALLEL_BUILDS,         ValidInt));
   options.push_back(new CConfigOption(CONFIG_KEY_CERTIFICATE_DIR,         NULL));
}

void CConfigFile::Parse(string filename)
{
      FILE *fp;
      char line_buffer[PATH_MAX];
      string command =  "bash --norc --noprofile -O extglob -c 'source " + filename + " 2> /dev/null";

      if (filename != BUILD_FILES_CONFIG)
         command += "; echo source_dir=$source_dir \
                     ; echo download_mirror_first=$download_mirror_first \
                     ; echo download_timeout=$download_timeout \
                     ; echo download_retry=$download_retry \
                     ; echo download_connections=$download_connections \
                     ; echo required_version=$required_version \
                     ; echo default_name_prefix=$default_name_prefix \
                     ; echo parallel_builds=$parallel_builds \
                     ; echo certificate_dir=$certificate_dir \
                     '";
      else
         command += "; echo cross_depends=${CROSS_DEPENDS[@]} \
                     ; echo build=$BUILD \
                     ; echo host=$HOST \
                     ; echo download_mirror=$DOWNLOAD_MIRROR \
                     ; echo layers=${LAYERS[@]} \
                     '";

      fp = popen(command.c_str(), "r");
      if (fp == NULL)
         throw std::runtime_error(strerror(errno));

      while (fgets(line_buffer, PATH_MAX, fp) != NULL)
      {
         // Parse key=value pairs
         string line(line_buffer);
         string key, value;
         size_t pos = line.find_first_of('=');

         key=line.substr(0, pos);
         value=line.substr(pos+1);

         stripChar(value, '\n');

         if (value != "")
         {
            if (filename != BUILD_FILES_CONFIG)
            {
               // ~/.buildgearconfig, .buildgear/config :
               Config.bg_config[key] = value;
            }
            else
            {
               // buildfiles/config :
               Config.bf_config[key] = value;
            }
         }
      }
      pclose(fp);
}

void CConfigFile::Update(string filename)
{
   FILE *in_file;
   char line_buffer[LINE_MAX];
   string new_line;
   bool updated = false;
   ostringstream outbuffer;
   ofstream config_file;
   string key;
   size_t pos;

   // Parse the current file to store non-default settings
   Parse(filename);

   if (!FileSystem.FileExist(TEMPLATE_LOCAL_CONFIG))
   {
      cout << "\nError: Could not find template file. " << TEMPLATE_LOCAL_CONFIG;
      cout << "\n       Please reinstall buildgear.\n";
      exit(EXIT_FAILURE);
   }

   in_file = fopen(TEMPLATE_LOCAL_CONFIG, "r");

   if (!in_file)
   {
      cout << "\nError: Could not open " << filename << " for reading.\n";
      exit(EXIT_FAILURE);
   }

   while (fgets(line_buffer, LINE_MAX, in_file) != NULL)
   {
      pos = string(line_buffer).find("=");
      if (pos != string::npos)
      {
         // The line contains a setting, get the key
         key = string(line_buffer).substr(1,pos - 1);

         if (key != Config.key)
         {
            // The line did not contain the option we want to change.
            // If the value differs from the default we keep the new value
            if (Config.bg_config[key] != Config.bg_config_default[key])
            {
               new_line = key + "=" + Config.bg_config[key] + "\n";
               outbuffer << new_line;
            } else
               outbuffer << line_buffer;
         } else
         {
            // This is key we want to change
            if (Config.unset)
               outbuffer << line_buffer;
            else
            {
               new_line = Config.key + "=" + Config.value + "\n";
               outbuffer << new_line;
            }
            updated = true;
         }
      } else
      {
         // Line does not contain =
         outbuffer << line_buffer;
      }
   }

   // Key was not found in template
   if (!updated && !Config.unset)
   {
      cout << "\nError: Installed template is not compliant with current buildgear version.";
      cout << "\n       Please make sure buildgear is correctly installed.\n";
      exit(EXIT_FAILURE);
   }

   fclose(in_file);

   // Open the config file for writing
   try
   {
      config_file.exceptions(ofstream::failbit | ofstream::badbit);
      config_file.open(filename.c_str());
      config_file << outbuffer.str();
   } catch (ifstream::failure e)
   {
      cout << "\nError: Could not write to config file " << filename;
      cout << "\n       " << strerror(errno) << endl;
      exit(EXIT_FAILURE);
   }

}
