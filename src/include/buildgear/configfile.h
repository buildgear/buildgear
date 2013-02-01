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

#ifndef CONFIGFILE_H
#define CONFIGFILE_H

#include "buildgear/config.h"
#include "buildgear/filesystem.h"
#include "buildgear/utility.h"

using namespace std;

typedef bool (*sanity_function)(string);

class CConfigOption
{
   public:
      CConfigOption(string, sanity_function);
      string key;
      sanity_function check;
};

class CConfigFile : public CFileSystem, CUtility
{
   public:
      CConfigFile();
      void Parse(string);
      void Update(string);
      void Init(string);
      list<CConfigOption*> options;
   private:

};

extern CConfigFile ConfigFile;

#endif
