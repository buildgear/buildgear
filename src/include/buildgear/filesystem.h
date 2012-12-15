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

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <string>
#include "buildgear/buildfiles.h"

using namespace std;

class CFileSystem
{
   public:
      void FindFiles(string dirname, string filename, list<CBuildFile*> *buildfiles);
      void FindRoot(string dirname);
      void CreateDirectory(string dirname);
      bool DirExists(string dirname);
      bool FileExist(string filename);
      bool FileExistSize(string filename, unsigned int &filesize);
      long Age(string filename);
      void Move(string source, string destination);
      void Cat(string filename);
      void Tail(string filename);
      void CopyFile(string source, string destination);
      void InitRoot(void);
      string GetWorkingDir(void);
      string root;
   private:
      void FindFile(string dirname, string filename, list<CBuildFile*> *buildfiles);
};

#endif
