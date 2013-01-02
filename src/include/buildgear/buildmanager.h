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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#ifndef BUILDMANAGER_H
#define BUILDMANAGER_H

#include "buildgear/config.h"
#include "buildgear/filesystem.h"
#include "buildgear/options.h"

using namespace std;

class CBuildManager : public CFileSystem, COptions
{
   public:
      void Build(list<CBuildFile*> *);
      void Do(string, CBuildFile*);
      void Clean(CBuildFile *);
      void CleanAll(void);
      void CleanPackages(void);
      void CleanWork(void);
      void CleanLog(void);
      bool PackageUpToDate(CBuildFile *);
      bool DepBuildNeeded(CBuildFile *buildfile);
      void BuildOutputTick(CBuildFile *buildfile);
      void BuildOutputPrint(void);
      list<CBuildFile*> active_builds;
      list<CBuildFile*> active_adds;
   private:
};

extern CBuildManager BuildManager;
extern pthread_mutex_t cout_mutex;
#endif
