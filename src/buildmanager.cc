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

#include "config.h"
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <thread>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <linux/limits.h>
#include <semaphore.h>
#include <unistd.h>
#include "buildgear/config.h"
#include "buildgear/options.h"
#include "buildgear/filesystem.h"
#include "buildgear/buildfiles.h"
#include "buildgear/dependency.h"
#include "buildgear/source.h"
#include "buildgear/buildmanager.h"
#include "buildgear/download.h"
#include "buildgear/log.h"

sem_t build_semaphore;
pthread_mutex_t add_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t cout_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;
CBuildFile *last_build;

class CBuildThread : CBuildManager
{
   public:
      CBuildThread(CBuildFile *buildfile);
      void operator()();
      void Start(void);
      void Join(void);
   private:
      thread *buildthread;
	   CBuildFile *buildfile;
};

CBuildThread::CBuildThread(CBuildFile *buildfile)
{
   this->buildfile = buildfile;
}

void CBuildThread::operator()()
{
   sem_wait(&build_semaphore);

   // Only build if build() function is available
   if (buildfile->build_function == "yes")
   {
      Do("build", buildfile);

      pthread_mutex_lock(&add_mutex);
      // Don't add last build
      if (buildfile != last_build)
         Do("add", buildfile);
      pthread_mutex_unlock(&add_mutex);
   }
 
   sem_post(&build_semaphore);
};

void CBuildThread::Start(void)
{
   buildthread = new thread(ref(*this));
}

void CBuildThread::Join(void)
{
   buildthread->join();
}

void CBuildManager::Do(string action, CBuildFile* buildfile)
{
   FILE *fp;
   char line_buffer[LINE_MAX];
   vector<char> log_buffer;
   string arguments;
   string command;
   stringstream pid;
   string build(buildfile->build ? "yes" : "no");
   
   // Get PID
   pid << (int) getpid();
   
   // Set required script arguments
   arguments += " --BG_BUILD_FILE '" + buildfile->filename + "'";
   arguments += " --BG_ACTION '" + action + "'";
   arguments += " --BG_BUILD_FILES_CONFIG '" BUILD_FILES_CONFIG "'";
   arguments += " --BG_BUILD_TYPE '" + buildfile->type + "'";
   arguments += " --BG_WORK_DIR '" WORK_DIR "'";
   arguments += " --BG_PACKAGE_DIR '" PACKAGE_DIR "'";
   arguments += " --BG_OUTPUT_DIR '" OUTPUT_DIR "'";
   arguments += " --BG_SOURCE_DIR '" SOURCE_DIR "'";
   arguments += " --BG_SYSROOT_DIR '" SYSROOT_DIR "'";
   arguments += " --BG_PID '" + pid.str() + "'";
   arguments += " --BG_BUILD '" + Config.build_system + "'";
   arguments += " --BG_HOST '" + Config.host_system + "'";
   arguments += " --BG_VERBOSE 'no'";

   // Apply build settings to all builds if '--all' is used
   if (Config.all)
   {
      arguments += " --BG_BUILD_BUILD '" + build + "'";
      arguments += " --BG_UPDATE_CHECKSUM '" + Config.update_checksum + "'";
      arguments += " --BG_UPDATE_FOOTPRINT '" + Config.update_footprint + "'";
      arguments += " --BG_NO_STRIP '" + Config.no_strip + "'";
      arguments += " --BG_KEEP_WORK '" + Config.keep_work + "'";
   } else
   {
      // Apply settings to main build
      if (Config.name == buildfile->name)
      {
         arguments += " --BG_BUILD_BUILD '" + build + "'";
         arguments += " --BG_UPDATE_CHECKSUM '" + Config.update_checksum + "'";
         arguments += " --BG_UPDATE_FOOTPRINT '" + Config.update_footprint + "'";
         arguments += " --BG_NO_STRIP '" + Config.no_strip + "'";
         arguments += " --BG_KEEP_WORK '" + Config.keep_work + "'";
      } else
      {
         // Apply default settings to the build dependencies
         arguments += " --BG_BUILD_BUILD '" + build + "'";
         arguments += " --BG_UPDATE_CHECKSUM 'no'";
         arguments += " --BG_UPDATE_FOOTPRINT 'no'";
         arguments += " --BG_NO_STRIP 'no'";
         arguments += " --BG_KEEP_WORK 'no'";
      }
   }
   
   command = SCRIPT " " + arguments;

   /* Make sure we are using bash */
   command = "bash --norc --noprofile -O extglob -c '" + command + "' 2>&1";

   pthread_mutex_lock(&cout_mutex);
   
   if ((action == "build") && (buildfile->build))
      cout << "   Building      '" << buildfile->name << "'" << endl;
   
   if (action == "add")
      cout << "   Adding        '" << buildfile->name << "'" << endl;
   
   if (action == "remove")
      cout << "   Removing      '" << buildfile->name << "'" << endl;
   
   pthread_mutex_unlock(&cout_mutex);

   /* Execute command */
   fp = popen(command.c_str(), "r");
   if (fp == NULL)
   {
      cout << "\npopen() failed\n";
      exit(EXIT_FAILURE);
   }

   /* Log output from build script (buildgear.sh) */
   if (Config.parallel_builds <= 1)
   {
      // For non-parallel builds, write every log line continously to build log
      while (fgets(line_buffer, LINE_MAX, fp) != NULL)
         Log.write(line_buffer, strlen(line_buffer));
   } else
   {
      // For parallel builds, write only finished log buffers to build log
      log_buffer.reserve(LOG_BUFFER_SIZE);
      while (fgets(line_buffer, LINE_MAX, fp) != NULL)
         log_buffer.insert(log_buffer.end(), line_buffer, line_buffer + strlen(line_buffer));
      pthread_mutex_lock(&log_mutex);
      Log.write(log_buffer.data(), log_buffer.size());
      pthread_mutex_unlock(&log_mutex);
   }
}

bool CBuildManager::PackageUpToDate(CBuildFile *buildfile)
{
   string package;
   
   package = PACKAGE_DIR "/" +
             buildfile->name + "#" + 
             buildfile->version + "-" +
             buildfile->release + PACKAGE_EXTENSION;
   
   if (!FileExist(package))
      return false;
   
   if (Age(package) > Age(buildfile->filename))
      return true;
   
   return false;
}

bool CBuildManager::DepBuildNeeded(CBuildFile *buildfile)
{
   list<CBuildFile*>::iterator it;
   
   for (it=buildfile->dependency.begin(); it!=buildfile->dependency.end(); it++)
   {
      if ((*it)->build)
         return true;
      
      if (DepBuildNeeded(*it))
         return true;
   }
   
   return false;
}

void CBuildManager::Build(list<CBuildFile*> *buildfiles)
{
   list<CBuildFile*>::iterator it;
   list<CBuildFile*>::reverse_iterator rit;

   // FIXME:
   // Check if buildfiles/config is newer than package or buildfiles
   // If so warn and delete work/packages (force total rebuild)

   // Set build action of all builds (based on package vs. Buildfile age)
   for (it=buildfiles->begin(); it!=buildfiles->end(); it++)
   {
      if (!PackageUpToDate((*it)))
         (*it)->build = true;
   }

   // Set build action of all builds (based on dependencies build status)
   for (it=buildfiles->begin(); it!=buildfiles->end(); it++)
   {
      // Skip if build action already set
      if ((*it)->build)
         continue;
      
      // If one or more dependencies needs to be build
      if (DepBuildNeeded((*it)))
      {
         // Then build is needed
         (*it)->build = true;
      } else
      {
         // Else no build is needed
         (*it)->build = false;
      }
   }

   // Only build if main build requires a build
   if ((buildfiles->back()->build) ||
       (Config.update_footprint=="yes") ||
       (Config.update_checksum=="yes"))
   {
      cout << endl;

      vector<CBuildThread *> builder;

      // Initialize build semaphore
      if (sem_init(&build_semaphore, 0, Config.parallel_builds) == -1)
      {
         cerr << "Error: Semaphore init failed" << endl;
         exit(EXIT_FAILURE);
      }

      // Start with buildfiles of depth 0
      int current_depth = buildfiles->front()->depth;
      last_build = buildfiles->back();

      // Process build order
      it=buildfiles->begin();
      while (it != buildfiles->end())
      {
         int thread_count=0;
         while ( ((*it)->depth == current_depth) && (it != buildfiles->end()))
         {
            // Start building threads of same depth in parallel
            CBuildThread *bt = new CBuildThread(*it);
            builder.push_back(bt);
            builder[thread_count]->Start();
            thread_count++;
            it++;
         }

         // Wait for thread_count build threads to complete
         for (int i=0; i<thread_count; i++)
         {
            builder[i]->Join();
            delete builder[i];
            builder.pop_back();
         }

         // Proceed to next depth level
         current_depth++;
      }
      sem_destroy(&build_semaphore);
   }
}

void CBuildManager::Clean(CBuildFile *buildfile)
{
   string command;
   
   command  = "rm -f ";
   command += string(PACKAGE_DIR) + "/" + 
              buildfile->name + "#" +
              buildfile->version + "-" +
              buildfile->release + 
              PACKAGE_EXTENSION;
   
   if (system(command.c_str()) < 0)
      perror("error\n");
}

void CBuildManager::CleanAll(void)
{
   CleanPackages();
   CleanWork();
   CleanLog();
}

void CBuildManager::CleanPackages(void)
{
   if (system("rm -rf " PACKAGE_DIR) < 0)
	   perror("error\n");
}

void CBuildManager::CleanWork(void)
{
   if (system("rm -rf " WORK_DIR) < 0)
	   perror("error\n");
}

void CBuildManager::CleanLog(void)
{
   if (system("rm -f " BUILD_LOG) < 0)
      perror("error\n");
}
