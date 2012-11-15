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
#include "buildgear/buildfile.h"
#include "buildgear/filesystem.h"
#include "buildgear/utility.h"

CBuildFile::CBuildFile(string filename)
{
   CBuildFile::filename = filename;
   CBuildFile::build = false;
   CBuildFile::visited = false;
   CBuildFile::depth = 0;
}

void CBuildFile::Parse(void)
{
   FILE *fp;
   char line_buffer[PATH_MAX];
   size_t pos;
   string command =
      "bash -O extglob -c 'source " +
      filename +
      "; echo name=$name \
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
}
