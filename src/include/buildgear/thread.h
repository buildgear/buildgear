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
