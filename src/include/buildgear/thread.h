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

#ifndef THREAD_H
#define THREAD_H

#include <memory>
#include <iostream>
#include <vector>
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include <cassert>
#include <error.h>
#include <string.h>

using namespace std;

class Runnable {
public:
   virtual void* run() = 0;
   virtual ~Runnable() = 0;
};


class Thread {
public:
   Thread(auto_ptr<Runnable> run, bool isDetached = false);
   Thread(bool isDetached = false);
   virtual ~Thread();
   void start();
   void* join();
private:
   // thread ID
   pthread_t PthreadThreadID;
   // true if thread created in detached state
   bool detached;
   pthread_attr_t threadAttribute;
   // runnable object will be deleted automatically
   auto_ptr<Runnable> runnable;
   Thread(const Thread&);
   const Thread& operator=(const Thread&);
   // called when run() completes
   void setCompleted();
   // stores return value from run()
   void* result;
   virtual void* run() {}
   static void* startThreadRunnable(void* pVoid);
   static void* startThread(void* pVoid);
   void printError(const char * msg, int status, const char* fileName, int lineNumber);
};

#endif
