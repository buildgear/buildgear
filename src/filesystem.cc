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

#include "config.h"
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <errno.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <signal.h>
#include "buildgear/config.h"
#include "buildgear/filesystem.h"
#include "buildgear/debug.h"

#define IN_EVENT_SIZE  ( sizeof (struct inotify_event) )
#define IN_BUF_LEN     ( 1024 * ( IN_EVENT_SIZE + 16 ) )

string CFileSystem::GetWorkingDir(void)
{
   char temp[PATH_MAX];

   if (getcwd(temp, sizeof(temp)) != 0)
      return std::string(temp);

   throw std::runtime_error(strerror(errno));
}

void CFileSystem::FindRoot(string dirname)
{
   string cwd;
   string temp_dirname;

   cwd = GetWorkingDir();

   temp_dirname = cwd + "/" + dirname;

   while ((cwd != "/") && chdir(temp_dirname.c_str()) != 0)
   {
      if (chdir("..") != 0)
         throw std::runtime_error(strerror(errno));

      cwd = GetWorkingDir();

      temp_dirname = cwd + "/" + dirname;

      Debug << "Backstepping to " << cwd << endl;
   }

   if (cwd == "/")
   {
      cerr << "Build Gear root directory (.buildgear) is not found!" << endl;
      exit(EXIT_FAILURE);
   }

   CFileSystem::root = cwd;
   Config.root = cwd;

   // Change to buildgear root dir and stay there
   if (chdir(CFileSystem::root.c_str()) != 0)
      throw std::runtime_error(strerror(errno));

   Debug << "Build root: " << CFileSystem::root << endl;
}

void CFileSystem::FindFile(string dirname, string filename, list<CBuildFile*> *buildfiles) 
{
   DIR           *d;
   struct dirent *dir;
   string cwd;

   d = opendir(dirname.c_str());
   if (d == NULL)
      throw std::runtime_error(strerror(errno));

   if (chdir(dirname.c_str()) != 0)
      throw std::runtime_error(strerror(errno));

   while((dir = readdir(d)))
   {
      if(strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
         continue;
      if(dir->d_type == DT_DIR)
      {
         if (chdir(dir->d_name) != 0)
            throw std::runtime_error(strerror(errno));
         CFileSystem::FindFile(".", filename, buildfiles);
         if (chdir("..") != 0)
            throw std::runtime_error(strerror(errno));
      } else 
      {
         if(strcmp(dir->d_name, filename.c_str()) == 0 )
         {
            cwd = GetWorkingDir();
            // IMPROVE: maybe not do full path? (make it all relative)
            CBuildFile *b = new CBuildFile(cwd + "/" + dir->d_name);
            buildfiles->push_back(b);
            continue;
         }
      }
   }
   closedir(d);
}

void CFileSystem::FindFiles(string dirname, string filename, list<CBuildFile*> *buildfiles)
{
   if (CFileSystem::DirExists(dirname))
      CFileSystem::FindFile(dirname, filename, buildfiles);

   /* Return to buildgear root */
   if (chdir(CFileSystem::root.c_str()) != 0)
      throw std::runtime_error(strerror(errno));
}

bool CFileSystem::DirExists(string dirname)
{
   struct stat st;

   if (stat(dirname.c_str(), &st) != 0)
      return false;
   else if (!S_ISDIR(st.st_mode))
      return false;

   return true;
}

bool CFileSystem::FileExist(string filename)
{
   struct stat st;

   if (stat(filename.c_str(), &st) != 0)
      return false;
   else if (!S_ISREG(st.st_mode))
      return false;

   return true;
}

bool CFileSystem::FileExistSize(string filename, unsigned int &filesize )
{
   struct stat buffer;

   if (stat(filename.c_str(), &buffer ))
   {
      filesize = 0;
      return false;
   }

   filesize = buffer.st_size;
   return true;
}

long CFileSystem::Age(string filename)
{
   struct stat st;

   if (stat(filename.c_str(), &st) != 0)
      return -1;

   return st.st_mtime;
}

void CFileSystem::CreateDirectory(string dirname)
{
   int status;
   string command = "mkdir -p " + dirname;

   status = system(command.c_str());

   if (status != 0)
      throw std::runtime_error(strerror(errno));
}

void CFileSystem::Move(string source, string destination)
{
   int status;
   string command = "mv " + source + " " + destination;

   status = system(command.c_str());

   if (status != 0)
      throw std::runtime_error(strerror(errno));
}

void CFileSystem::Cat(string filename)
{
   int status;
   string command = "cat " + filename;

   status = system(command.c_str());

   if (status != 0)
      throw std::runtime_error(strerror(errno));
}

void * tail(void *filename)
{
   int status;
   string file = *((string *) filename);
   string command = "tail -f " + file;

   status = system(command.c_str());
   if (status != 0)
      exit(0);

   pthread_exit(&status);
}

void CFileSystem::Tail(string filename)
{
   int length, i = 0;
   int fd, wd;
   char buffer[IN_BUF_LEN];
   pthread_t tail_thread;
   string file, dir;

   // Parse name and path parts from filename
   size_t pos = filename.find_last_of('/');

   if (pos == filename.npos )
   {
      cout << "Error: " << filename << " is invalid." << endl;
         exit(EXIT_FAILURE);
   }

   file=filename.substr(pos+1);
   dir=filename.substr(0, pos);

   // Tail file if it already exists
   if (FileExist(filename))
      pthread_create(&tail_thread, NULL, tail, (void *) &filename);

   fd = inotify_init();
   wd = inotify_add_watch(fd, dir.c_str(), IN_CREATE);

   // Constantly monitor for file creation event
   while (1)
   {
      length = read(fd, buffer, IN_BUF_LEN);

      while (i < length)
      {
         struct inotify_event *event = (struct inotify_event *) &buffer[i];
         if (event->len)
         {
            if ((event->mask & IN_CREATE) && (strcmp(event->name, file.c_str()) == 0))
            {
               // New file created -> tail (re)open
               pthread_cancel(tail_thread);
               pthread_join(tail_thread, NULL);
               pthread_create(&tail_thread, NULL, tail, (void *) &filename);
            }
         }
         i += IN_EVENT_SIZE + event->len;
      }
      i=0;
   }

   (void) inotify_rm_watch(fd, wd);
   (void) close(fd);
}

void CFileSystem::CopyFile(string source, string destination)
{
   int status;
   string command = "cp " + source + " " + destination;

   status = system(command.c_str());

   if (status != 0)
      throw std::runtime_error(strerror(errno));
}

void CFileSystem::InitRoot(void)
{
   if (DirExists(ROOT_DIR))
      cout << "Build Gear area already exists." << endl;
   else
   {
      CreateDirectory(ROOT_DIR);
      CreateDirectory(BUILD_FILES_DIR);
      CreateDirectory(BUILD_FILES_NATIVE_DIR);
      CreateDirectory(BUILD_FILES_CROSS_DIR);
      CreateDirectory(FOOTPRINT_NATIVE_DIR);
      CreateDirectory(FOOTPRINT_CROSS_DIR);
      CreateDirectory(CHECKSUM_NATIVE_DIR);
      CreateDirectory(CHECKSUM_CROSS_DIR);

      CopyFile(TEMPLATE_CONFIG, BUILD_FILES_CONFIG);
      CopyFile(TEMPLATE_README, BUILD_FILES_README);
      CopyFile(TEMPLATE_LOCAL_CONFIG, LOCAL_CONFIG_FILE);

      cout << "Initialized empty build area in "
           << GetWorkingDir() << endl;
   }
}
