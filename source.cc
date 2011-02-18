#include <iostream>
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

void CSource::Download(list<CBuildFile*> *buildfiles, CFileSystem *filesystem, COptions *options)
{
   CDownload Download;
   bool first;
   
   list<CBuildFile*>::iterator it;
   string command;
   
   /* Make sure that source temp dir exists */
   filesystem->CreateDirectory(SOURCE_TEMP_DIR);

   /* Traverse buildfiles download list */
   for (it=buildfiles->begin(); it!=buildfiles->end(); it++)
   {
      istringstream iss((*it)->source);
      string item;
      
      first = true;
      
      // For each source item      
      while ( getline(iss, item, ' ') )
      {
         // Download item if it is a remote URL
         if (CSource::remote(item))
         {
            if (first)
            {
               cout << " " << (*it)->name << ":" << endl;
               first = false;
            }
            cout << "   Downloading '" << item << "'" << endl; 
            Download.File(item, SOURCE_TEMP_DIR);
         }
      }
      
      // TODO: Fix broken progress bar!
      // TODO: Fix resume (seek to end of partial file before dl)
      // TODO: Check sha256sum
      // TODO: If OK move file from SOURCE_TEMP_DIR to SOURCE_DIR
      
      
      // Figure out which source elements are local or remote
      // Download remote files
         // Download.File("ftp://ftp.fu-berlin.de/unix/tools/file/file-5.05.tar.gz", "/home/mgl");
         // Download.URL(source, SOURCE_TEMP_DIR);
      // Copy local files
      
   }
}

void CSource::Build(CDependency *dependency, CFileSystem *filesystem, COptions *options)
{
   list<CBuildFile*>::iterator it;
   string command;

   cout << endl << "Building..." << endl;

   for (it=dependency->build_order.begin(); it!=dependency->build_order.end(); it++)
   {
      FILE *fp;
      char path[PATH_MAX];
      command = BUILD_SCRIPT " build " + 
                  (*it)->filename + " --target";

      if (chdir(filesystem->root.c_str()) != 0)
         throw std::runtime_error(strerror(errno));

      cout << "\nBuilding " << (*it)->name << endl;
      cout << " " << command << endl;

      fp = popen(command.c_str(), "r");
      if (fp == NULL)
         throw std::runtime_error(strerror(errno));

      while (fgets(path, PATH_MAX, fp) != NULL)
         cout << path;

      pclose(fp);
   }

   cout << endl;
}
