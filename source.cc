#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sstream>
#include <linux/limits.h>
#include "buildgear/config.h"
#include "buildgear/options.h"
#include "buildgear/filesystem.h"
#include "buildgear/buildfiles.h"
#include "buildgear/dependency.h"
#include "buildgear/source.h"
#include "buildgear/download.h"

int CSource::remote(string item)
{
   int i;
   string protocol[4] = { "http://",
                           "ftp://",
                         "https://",
                          "ftps://"  };
   
   for (i=0; i<4; i++)
   {
      if (item.find(protocol[i]) != item.npos)
         return true;
   }
   
   return false;
}

void CSource::Download(list<CBuildFile*> *buildfiles, string source_dir)
{
   CDownload Download;
   
   list<CBuildFile*>::iterator it;
   string command;
   
   /* Make sure that source dir exists */
   CreateDirectory(source_dir);

   /* Traverse buildfiles download list */
   for (it=buildfiles->begin(); it!=buildfiles->end(); it++)
   {
      istringstream iss((*it)->source);
      string item;
      
      // For each source item      
      while ( getline(iss, item, ' ') )
      {
         // Download item if it is a remote URL
         if (CSource::remote(item))
            Download.URL(item, source_dir);
      }
   }
}

void CSource::Do(string action, CBuildFile* buildfile)
{
   string config;
   string command;
   
   // Set action
   config  = " ACTION=" + action;
   
   // Set required script variables
   config += " BUILD_FILES_CONFIG=" BUILD_FILES_CONFIG;
   config += " BUILD_TYPE=" + buildfile->type;
   config += " WORK_DIR=" WORK_DIR;
   config += " PACKAGE_DIR=" PACKAGE_DIR;
   config += " SOURCE_DIR=" + CSource::config->source_dir;
   config += " BUILD_LOG_FILE=" BUILD_LOG_FILE;
   config += " NAME=" + buildfile->name;
   
   command = config + " fakeroot " SCRIPT " " + buildfile->filename;      
   
   if (action == "build")
      cout << "   Building     '" << buildfile->name << "'" << endl;
   
   if (action == "add")
      cout << "   Adding       '" << buildfile->name << "'" << endl;
   
   if (action == "remove")
   {
      cout << "   Removing     '" << buildfile->name << "'" << endl;
   }
   
   if (system(command.c_str()) != 0)
      throw std::runtime_error(strerror(errno));
}

void CSource::Build(list<CBuildFile*> *buildfiles, CConfig *config)
{
   list<CBuildFile*>::iterator it;
   list<CBuildFile*>::reverse_iterator rit;
   
   CSource::config = config;

   // Process build order
   for (it=buildfiles->begin(); it!=buildfiles->end(); it++)
   {
      Do("build", (*it));
      
      if ((*it) != buildfiles->back())
         Do("add", (*it));
   }

   // Process build order
   for (rit=buildfiles->rbegin(); rit!=buildfiles->rend(); rit++)
   {
      if ((*rit) != buildfiles->front())
         Do("remove", (*rit));
   }

   // Announce successful build of all
   cout << "Done" << endl << endl;
}
