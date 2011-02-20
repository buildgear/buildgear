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

void CSource::Build(CDependency *dependency)
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

      if (chdir(root.c_str()) != 0)
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
