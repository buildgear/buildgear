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

void CSource::Build(list<CBuildFile*> *buildfiles, string source_dir)
{
   list<CBuildFile*>::iterator it;
   string config;
   string command;
   
   // Build action sequence
   string action[][3] = 
   { { "do_checksum", " Checksum  ", "[Checksum mismatch]"       },
     {  "do_extract", " Extract   ", "[Extracting source failed]"},
     {    "do_build", " Build     ", "[Build() failed]"          },
     {    "do_strip", " Strip     ", "[Striping failed]"         },
     {  "do_package", " Package   ", "[Creating package failed]" },
     {"do_footprint", " Footprint ", "[Footprint mismatch]"      },
     {    "do_clean", " Clean     ", "[Cleanup failed]"          },
     {            "",            "", ""                          } };

   // Traverse build files (build order)
   for (it=buildfiles->begin(); it!=buildfiles->end(); it++)
   {
      int i=0;
      
      // Announce build name in progress
      cout << "   Building     '" << (*it)->name << "'" << endl;
      
      // Set required buildgear script variables
      config  = " BUILD_FILES_CONFIG=" BUILD_FILES_CONFIG;
      config += " BUILD_TYPE=" + (*it)->type;
      config += " WORK_DIR=" WORK_DIR;
      config += " PACKAGE_DIR=" PACKAGE_DIR;
      config += " SOURCE_DIR=" + source_dir;
      config += " BUILD_FILE_DIR=" + string(BUILD_FILES_DIR) + "/" + (*it)->name;
      config += " BUILD_LOG_FILE=" BUILD_LOG_FILE;
      config += " NAME=" + (*it)->name;
    
      while (action[i][0] != "")
      {
         // Announce build action in progress
         cout << setw(16) << action[i][1] << "'" << (*it)->name << "'" << endl;
         
         command = config + " " SCRIPT " " + action[i][0] + " " + (*it)->filename;

         // Fire action command
         if (system(command.c_str()) != 0)
         {
            // Announce action failure message
            cout << "   Error       '" << (*it)->name << "' " << action[i][2] << endl;
            cout << "Failed" << endl << endl;
            return;
         }
         i++;
      }
   // Announce successful build
   cout << "   Done         '" << (*it)->name << "'" << endl;
   
   // Add contents of package to sysroot
//   command = config + " " SCRIPT " do_add " + (*it)->filename;
//   system(command.c_str());
   
   }
   // Announce successful build of all
   cout << "Done" << endl << endl;
}
