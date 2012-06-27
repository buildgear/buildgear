#include "config.h"
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include "buildgear/config.h"
#include "buildgear/buildsystem.h"
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

void CBuildSystem::RunCheckFile(void)
{
   int result;
   
   // Run buildfiles/tools file
   if (FileExist(string(BUILD_FILES_CHECK)))
   {
      result = system("bash -c 'source " BUILD_FILES_CHECK " 2> /dev/null'");
      if (result != 0)
      {
         cout << endl << "Please install missing tools or libraries." 
              << endl << endl;
         exit(EXIT_FAILURE);
      }
   }
}

void CBuildSystem::ShowLog(void)
{
   string log_file = Config.root + "/" + string(BUILD_LOG);
   FileSystem.Cat(log_file);
}
