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

#include "config.h"
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <sstream>
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
#include "buildgear/thread.h"

sem_t build_semaphore;
pthread_mutex_t add_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t cout_mutex = PTHREAD_MUTEX_INITIALIZER;
CBuildFile *last_build;

class buildThread: public Thread, CBuildManager
{
   public:
	   buildThread(CBuildFile *bf) : buildfile(bf) {}
	   virtual void* run();
   private:
	   CBuildFile *buildfile;
};

void* buildThread::run() {
   sem_wait(&build_semaphore);

   // Only build if build (package) is not up to date
//   if (buildfile->build == true)
      Do("build", buildfile);
 
   pthread_mutex_lock(&add_mutex);
   // Don't add last build
   if (buildfile != last_build)
      Do("add", buildfile);
   pthread_mutex_unlock(&add_mutex);
   
   sem_post(&build_semaphore);
	return 0;
}

void CBuildManager::Do(string action, CBuildFile* buildfile)
{
   int result;
   string config;
   string command;
   stringstream pid;
   string build(buildfile->build ? "yes" : "no");
   
   // Get PID
   pid << (int) getpid();
   
   // Set script action
   config  = " BG_ACTION=" + action;
   
   // Set required script variables
   config += " BG_BUILD_FILES_CONFIG=" BUILD_FILES_CONFIG;
   config += " BG_BUILD_TYPE=" + buildfile->type;
   config += " BG_WORK_DIR=" WORK_DIR;
   config += " BG_PACKAGE_DIR=" PACKAGE_DIR;
   config += " BG_OUTPUT_DIR=" OUTPUT_DIR;
   config += " BG_SOURCE_DIR=" SOURCE_DIR;
   config += " BG_SYSROOT_DIR=" SYSROOT_DIR;
   config += " BG_BUILD_LOG=" BUILD_LOG;
   config += " BG_PID=" + pid.str();
   config += " BG_VERBOSE=no";
   config += " BG_BUILD=" + Config.build_system;
   config += " BG_HOST=" + Config.host_system;

   // Apply build settings to all builds if '--all' is used
   if (Config.all)
   {
      config += " BG_BUILD_BUILD=" + build;
      config += " BG_UPDATE_CHECKSUM=" + Config.update_checksum;
      config += " BG_UPDATE_FOOTPRINT=" + Config.update_footprint;
      config += " BG_NO_STRIP=" + Config.no_strip;
      config += " BG_KEEP_WORK=" + Config.keep_work;
   } else
   {
      // Apply settings to main build
      if (Config.name == buildfile->name)
      {
         config += " BG_BUILD_BUILD=" + build;
         config += " BG_UPDATE_CHECKSUM=" + Config.update_checksum;
         config += " BG_UPDATE_FOOTPRINT=" + Config.update_footprint;
         config += " BG_NO_STRIP=" + Config.no_strip;
         config += " BG_KEEP_WORK=" + Config.keep_work;
      } else
      {
         // Apply default settings to the build dependencies
         config += " BG_BUILD_BUILD=" + build;
         config += " BG_UPDATE_CHECKSUM=no";
         config += " BG_UPDATE_FOOTPRINT=no";
         config += " BG_NO_STRIP=no";
         config += " BG_KEEP_WORK=no";
      }
   }
   
   command = config + " " SCRIPT " " + buildfile->filename;

   /* Make sure we are using bash */
   command = "bash --norc --noprofile -O extglob -c '" + command + "'";
   
   pthread_mutex_lock(&cout_mutex);
   
   if ((action == "build") && (buildfile->build))
   {
      cout << "   Building      '" << buildfile->name << "'" << endl;
   }
   
   if (action == "add")
      cout << "   Adding        '" << buildfile->name << "'" << endl;
   
   if (action == "remove")
      cout << "   Removing      '" << buildfile->name << "'" << endl;
   
   pthread_mutex_unlock(&cout_mutex);

   result = system(command.c_str());
   if (result != 0)
   {
      if (WIFSIGNALED(result) && (WTERMSIG(result) == SIGINT))
         cout << "\n\nInterrupt signal received - stopped!\n";
      else
         cout << "\nsystem() failed\n";
      exit(EXIT_FAILURE);
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

//      vector<buildThread *> builder;

      // Initialize build semaphore
      if (sem_init(&build_semaphore, 0, Config.parallel_builds) == -1)
      {
         cerr << "Error: Semaphore init failed" << endl;
         exit(EXIT_FAILURE);
      }

      int current_depth = buildfiles->front()->depth;
      last_build = buildfiles->back();

      // Process build order
      it=buildfiles->begin();
      while (it != buildfiles->end())
      {
         // Do not do build if no build() function available
         if ((*it)->build_function == "yes")
         {
            Do("build", *it);

            // Don't add last build
            if (*it != last_build)
               Do("add", *it);
         }

         it++;

// FIXME: Fix broken thread implementation (replace with c++11 threads)
//         int i=0;
//         while ( ((*it)->depth == current_depth) && (it != buildfiles->end()))
//         {
//            // Start build threads of same depth (count how many started i)
//            buildThread* build(new buildThread(*it));
//            builder.push_back(build);
//            builder[i]->start();
//            i++;
//            it++;
//         }
//
//         // Wait for i build threads to complete
//         for (int j=0; j<i; j++)
//         {
//            builder[j]->join();
//            delete builder[j];
//            builder.pop_back();
//         }
//
//         // Proceed to next depth level
//         current_depth--;
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
