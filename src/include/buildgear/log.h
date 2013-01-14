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

#ifndef LOG_H
#define LOG_H

#include "buildgear/config.h"
#include "buildgear/buildfile.h"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;

class CStreamDescriptor;

class CLog
{
   public:
      void open(string filename);
      void write(char *buffer, int length);
      void close();
      CStreamDescriptor* add_stream(FILE*, CBuildFile*);
      list<CStreamDescriptor*> log_streams;
      mutex log_streams_mutex;
   private:
      ofstream log_file;
};

class CStreamDescriptor: CLog
{
   public:
      CStreamDescriptor(FILE *, CBuildFile *);
      mutex output_mutex;
      FILE *fp;
      int fd;
      vector<char> log_buffer;
      CBuildFile *buildfile;
      condition_variable done_cond;
      mutex done_mutex;
      bool done_flag;
};

extern CLog Log;

#endif
