#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include "buildgear/tools.h"

void CTools::Check(void)
{
   int status, i=0;
   string tool[] = {      "bash",
                      "fakeroot",
                     "sha256sum",
                           "sed",
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
                              "" };

   while (tool[i] != "")
   {
      string command = "type " + tool[i] + " > /dev/null";
      
      status = system(command.c_str());
      
      if (status != 0)
      {
         cout << "Command '" << tool[i] << "' is not found. Please install." << endl;
         exit(EXIT_FAILURE);
      }
      
      i++;
   }
}
