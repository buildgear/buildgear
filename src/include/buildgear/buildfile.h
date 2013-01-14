/*
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

#ifndef BUILDFILE_H
#define BUILDFILE_H

#include <list>

using namespace std;

class CBuildFile : public CUtility
{
   public:
      CBuildFile(string filename);
      void Parse(void);
      string filename;
      string name;
      string short_name;
      string version;
      string release;
      string source;
      string depends;
      string missing_depends;
      string type;
      string build_function;
      string check_function;
      bool build;
      bool visited;
      int depth;
      float x;
      float y;
      list<CBuildFile*> dependency;
      int tick;
   private:
};

#endif
