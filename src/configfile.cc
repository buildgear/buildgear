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
               if (key == CONFIG_KEY_DEFAULT_NAME_PREFIX)
                  Config.default_name_prefix = value;
               if (key == CONFIG_KEY_SOURCE_DIR)
                  Config.source_dir = value;
               if (key == CONFIG_KEY_DOWNLOAD_TIMEOUT)
                  Config.download_timeout = atoi(value.c_str());
               if (key == CONFIG_KEY_DOWNLOAD_RETRY)
                  Config.download_retry = atoi(value.c_str());
               if (key == CONFIG_KEY_DOWNLOAD_MIRROR_FIRST)
                  Config.download_mirror_first = value;
               if (key == CONFIG_KEY_DOWNLOAD_CONNECTIONS)
                  Config.download_connections = atoi(value.c_str());
               if (key == CONFIG_KEY_PARALLEL_BUILDS)
                  Config.parallel_builds = atoi(value.c_str());
            }
            else
            {
               // buildfiles/config :
               if (key == CONFIG_KEY_CROSS_DEPENDS)
                  Config.cross_depends = value;
               if (key == CONFIG_KEY_BUILD)
                  Config.build_system = value;
               if (key == CONFIG_KEY_HOST)
                  Config.host_system = value;
               if (key == CONFIG_KEY_DOWNLOAD_MIRROR)
                  Config.download_mirror = value;
               if (key == CONFIG_KEY_LAYERS)
                  Config.layers = value;
            }
         }
      }
      pclose(fp);
}

void CConfigFile::Update(string filename)
{
   FILE *in_file, *out_file;
   char line_buffer[LINE_MAX];
   string new_line;
   bool updated = false;
   char *tmpfile;

   if (!FileSystem.FileExist(filename))
      Init(filename);

   in_file = fopen(filename.c_str(), "r");

   if (!in_file)
   {
      cout << "Could not open " << filename << " for reading.\n";
      exit(EXIT_FAILURE);
   }

   // Open a temporary file to keep the new configuration
   tmpfile = tmpnam(NULL);
   out_file = fopen(tmpfile, "w");

   if (!out_file)
   {
      cout << "Could not open temporary file for writing.\n";
      exit(EXIT_FAILURE);
   }

   while (fgets(line_buffer, LINE_MAX, in_file) != NULL)
   {
      // Line does not contain a key=value pair
      if ( strchr(line_buffer, '=') == NULL)
      {
         fputs(line_buffer, out_file);
         continue;
      }

      if (Config.unset)
      {
         // Key is alreadu unset so we just rewrite the line
         if (line_buffer[0] == '#')
         {
            fputs(line_buffer, out_file);
            continue;
         }

         // Put a # tag infront of the line to unset it
         new_line = "#" + string(line_buffer);
         fputs(new_line.c_str(), out_file);
      } else
      {
         // Check if the line contains the key= we are looking for
         if (string(line_buffer).find(Config.key + "=") == string::npos)
            fputs(line_buffer, out_file);
         else
         {
            new_line = Config.key + "=" + Config.value + "\n";
            fputs(new_line.c_str(), out_file);
            updated = true;
         }
      }
   }

   // Key was not found in file, adding it to the end
   if (!updated && !Config.unset)
   {
      new_line = "\n" + Config.key + "=" + Config.value + "\n";
      fputs(new_line.c_str(), out_file);
   }

   fclose(in_file);
   fclose(out_file);

   // Move the temporary file to the config file
   rename(tmpfile, filename.c_str());

   // Delete the temporary file
   unlink(tmpfile);

}

void CConfigFile::Init(string filename)
{
   FILE *fp;
   string command;
   string destination;

   if (FileSystem.FileExist(string(TEMPLATE_LOCAL_CONFIG)))
   {
      // Install the template if it exists
      command = "cp " + string(TEMPLATE_LOCAL_CONFIG) + " ";
      if (Config.global)
         destination = Config.home_dir + GLOBAL_CONFIG_FILE;
      else
         destination = LOCAL_CONFIG_FILE;

      command += destination + " 2>/dev/null";

      if (system(command.c_str()) != 0)
      {
         cout << "\nError: Could not install config template";
         cout << " in '" << destination;
         cout << "'.\n";
         cout << strerror(errno) << endl << endl;
         exit(EXIT_FAILURE);
      }

   } else
   {
      // Otherwise just create an empty file
      fp = fopen(filename.c_str(), "w");
      if (!fp)
      {
         cout << endl << "Error: Could not create '" << filename << "'." << endl;
         cout << strerror(errno) << endl;
         exit(EXIT_FAILURE);
      }
      fclose(fp);
   }
}
