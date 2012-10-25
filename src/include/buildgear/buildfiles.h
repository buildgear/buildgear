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

#ifndef BUILDFILES_H
#define BUILDFILES_H

#include <list>
#include "buildgear/config.h"
#include "buildgear/buildfile.h"
#include "buildgear/filesystem.h"
#include "buildgear/utility.h"

using namespace std;

class CBuildFiles
{
   public:
      CBuildFiles();
      list<CBuildFile*> buildfiles;
      void Parse(void);
      void LoadCrossDependency(void);
      void LoadDependency(void);
      void AddCrossDependency(void);
      void ShowMeta(void);
      void ShowReadme(void);
      void ShowVersion(CBuildFile *buildfile);
      void ShowVersions(list<CBuildFile*> *buildfiles);
      CBuildFile * BuildFile(string name);
      list<CBuildFile*> cross_dependency;
   private:
};

#endif
