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

#ifndef DEPENDENCY_H
#define DEPENDENCY_H

#include "buildgear/buildfiles.h"
#include "buildgear/dependency.h"

using namespace std;

class CDependency
{
   public:
      void ShowDownloadOrder(void);
      void ShowBuildOrder(void);
      void ResolveSequentialBuildOrder(string name,
                   list<CBuildFile*> *buildfiles);
      void ResolveParallelBuildOrder(void);
      void ShowDependencyCircleSVG(string filename);
      list<CBuildFile*> resolved;
      list<CBuildFile*> unresolved;
      list<CBuildFile*> build_order;
      list<CBuildFile*> parallel_build_order;
      list<CBuildFile*> download_order;
      void ResolveDependency(CBuildFile *buildfile,
                   list<CBuildFile*> *resolved,
                   list<CBuildFile*> *unresolved);
      void SetNameLength(void);
      int name_length;
   private:
      int setDepth(CBuildFile *buildfile, int depth);
};

extern CDependency Dependency;

#endif
