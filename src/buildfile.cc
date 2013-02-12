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
#include <limits.h>
#include "buildgear/config.h"
#include "buildgear/buildfile.h"
#include "buildgear/filesystem.h"
#include "buildgear/utility.h"
#include "buildgear/log.h"
#include "buildgear/cursor.h"

CBuildFile::CBuildFile(string filename)
{
   CBuildFile::filename = filename;
   CBuildFile::build = false;
   CBuildFile::visited = false;
   CBuildFile::depth = 0;
}

void CBuildFile::Show(void)
{
   string qoute = "\"";
   string command =
      "bash --norc --noprofile -O extglob -c 'source " +
      (string) BUILD_FILES_CONFIG +
      "; source " + filename +
      "; BG_TEMP=$(cat " + filename + ") \
       ; BG_TEMP=$(echo \"$BG_TEMP\" | sed -e \"s|\\\\\\|\\\\\\\\\\\\\\|g\") \
       ; BG_TEMP=${BG_TEMP//\\" + qoute + "/\\\\\\" + qoute + "} \
       ; echo \"echo -E \\" + qoute + "\" > /tmp/buildgear.bftmp \
       ; echo \"$BG_TEMP\" >> /tmp/buildgear.bftmp \
       ; echo \"\\" + qoute + "\" >> /tmp/buildgear.bftmp \
       ; BG_TEMP=$(source /tmp/buildgear.bftmp) \
       ; echo \"$BG_TEMP\" \
       ; rm -f /tmp/buildgear.bftmp'";

   if (system(command.c_str()) != 0)
   {
      cout << "\nError: Could not show buildfile for '" << name << "'\n";
      cout << strerror(errno) << endl;
      exit(EXIT_FAILURE);
   }
}

void CBuildFile::Parse(void)
{
   FILE *fp;
   char line_buffer[PATH_MAX];
   size_t pos;
   string fifo_name;
   string layerfile;
   string command =
      "bash --norc --noprofile -O extglob -c 'source " +
       (string) BUILD_FILES_CONFIG + " 2>/dev/null \
       ; echo config_return=$? \
       ; source " + filename + " 2>/dev/null \
       ; echo build_return=$? \
       ; echo name=$name \
       ; echo version=$version \
       ; echo release=$release \
       ; echo source=${source[@]} \
       ; echo depends=${depends[@]} \
       ; type build &> /dev/null && echo build_function=yes || echo build_function=no \
       ; type check &> /dev/null && echo check_function=yes || echo check_function=no'";

   // Open buildfile for reading
   fp = popen(command.c_str(), "r");
   if (fp == NULL)
      throw std::runtime_error(strerror(errno));

   // Assign name and type based on filename
   pos = filename.rfind("cross/");
   if (pos != filename.npos)
      type = "cross";
   else
   {
      pos = filename.rfind("native/");
      type = "native";
   }

   if (pos == filename.npos)
   {
      cout << "Error: " << filename << " is invalid." << endl;
      exit(EXIT_FAILURE);
   }

   name = filename.substr(pos);
   pos = name.rfind("/Buildfile");
   if (pos == filename.npos)
   {
      cout << "Error: " << filename << " is invalid." << endl;
      exit(EXIT_FAILURE);
   }

   // Parse Buildfile variables
   while (fgets(line_buffer, PATH_MAX, fp) != NULL)
   {
      // Parse key=value pairs
      string line(line_buffer);
      string key, value;
      size_t pos = line.find_first_of('=');

      key=line.substr(0, pos);
      value=line.substr(pos+1);

      stripChar(value, '\n');

      // Required keys (FIXME: add check for empty values)
      if (key == KEY_NAME)
         short_name = value;
      if (key == KEY_VERSION)
         version = value;
      if (key == KEY_RELEASE)
         release = value;
      if (key == KEY_CONFIG_RETURN)
      {
         if (stoi(value) != 0)
         {
            string command;
            int dummy;

            command = "source " BUILD_FILES_CONFIG;
            cout << endl << endl << "Error parsing " << BUILD_FILES_CONFIG  << endl;

            // Source the file again without redirecting the error out
            dummy = system(command.c_str());
            cout << endl;
            exit(EXIT_FAILURE);
         }
      }
      if (key == KEY_BUILD_RETURN)
      {
         if (stoi(value) != 0)
         {
            string command;
            int dummy;

            command = "source " + filename;
            cout << endl << endl << "Error parsing " << filename << endl;

            // Source the file again without redirecting the error out
            dummy = system(command.c_str());
            cout << endl;
            exit(EXIT_FAILURE);
         }
      }

      // Optional keys
      if (key == KEY_SOURCE)
         source = value;
      if (key == KEY_DEPENDS)
         depends = value;
      if (key == "build_function")
         build_function = value;
      if (key == "check_function")
         check_function = value;
   }
   pclose(fp);

   // Assign name based on type and name variable
   name = type + "/" + short_name;

   // Set control fifo name
   fifo_name = "/tmp/buildgear-" + type + "-" + short_name;
   fifo_name += "." + to_string(Config.pid) + ".fifo";
   control_fifo = new char [fifo_name.length() + 1];
   strcpy(control_fifo, fifo_name.c_str());

   // Check for layer file
   layerfile = filename.substr(0,filename.find_last_of("/")) + "/" LAYER_FILE_NAME;
   if (FileSystem.FileExist(layerfile))
   {
      char layer_name[LAYER_NAME_SIZE];
      fp = fopen(layerfile.c_str(), "r");

      if (!fp)
         perror("error\n");

      if (fscanf(fp, "%s", layer_name) > 0)
      {
         // Check if layer is in config file
         if (Config.bf_config[CONFIG_KEY_LAYERS].find(layer_name) == string::npos
             && layer_name != string(DEFAULT_LAYER_NAME))
         {
            string line_buffer;
            layer = DEFAULT_LAYER_NAME;
            line_buffer = string("======> Reverting '") +
               layerfile.substr(layerfile.find(BUILD_FILES_DIR "/")) +
               string("'\n   Layer '") + layer_name +
               string("' not defined in ") + BUILD_FILES_CONFIG +
               string(". Reverting to '" DEFAULT_LAYER_NAME "'.\n");
            Log.write((char *)line_buffer.c_str(), line_buffer.size());
         } else
            layer = string(layer_name);
      }
      else
         layer = DEFAULT_LAYER_NAME;

   } else
   {
      layer = DEFAULT_LAYER_NAME;
   }
}
