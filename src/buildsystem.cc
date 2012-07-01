#include "config.h"
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include "buildgear/config.h"
#include "buildgear/buildsystem.h"
#include "buildgear/buildfile.h"
#include "buildgear/buildfiles.h"
#include "buildgear/filesystem.h"

extern CFileSystem FileSystem;

void CBuildSystem::Check(void)
{
   int status, i=0;
   string tool[] = {      "bash",
                      "fakeroot",
                     "sha256sum",
                           "sed",
                           "awk",
                           "tar",
                          "gzip",
                         "bzip2",
                            "xz",
                         "unzip",
                          "lzma",
                          "diff",
                          "sort",
                          "find",
                          "grep",
                          "file",
                           "cat",
                            "mv",
                            "rm",
                            "cp",
                         "xargs",
                       "dirname",
                      "basename",
                       "unalias",
                              "" };

   while (tool[i] != "")
   {
      string command = "type " + tool[i] + " > /dev/null";
      
      status = system(command.c_str());
      
      if (status != 0)
      {
         cout << "Failed ('" << tool[i] << "' is not found)" << endl;
         exit(EXIT_FAILURE);
      }
      
      i++;
   }
}

void CBuildSystem::CallCheck(list<CBuildFile*> *buildfiles)
{
   int result;
   string command;
   list<CBuildFile*>::iterator it;
   
   /* Traverse through all buildfiles */
   for (it=buildfiles->begin(); it!=buildfiles->end(); it++)
   {
      if ((*it)->check == "yes")
      {
         command = "bash -c 'source " + (*it)->filename + "; check 1> /dev/null'";
         result = system(command.c_str());
         if (result != 0)
         {
            cout << endl << "Build system check failed - please install missing tools or libraries."
                 << endl << endl;
            exit(EXIT_FAILURE);
         }
      }
   }
}

void CBuildSystem::ShowLog(void)
{
   string log_file = Config.root + "/" + string(BUILD_LOG);
   FileSystem.Cat(log_file);
}
