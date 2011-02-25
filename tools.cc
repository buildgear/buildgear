#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include "buildgear/config.h"
#include "buildgear/tools.h"
#include "buildgear/filesystem.h"

void CTools::Check(void)
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

void CTools::RunToolsFile(void)
{
   int result;
   
   // Run buildfiles/tools file
   if (FileExists(string(BUILD_TOOLS_FILE)))
   {
      result = system("bash -c 'source " BUILD_TOOLS_FILE " 2> /dev/null'");
      if (result != 0)
      {
         cout << endl << "Please install missing tools." << endl << endl;
         exit(EXIT_FAILURE);
      }
   }
}
