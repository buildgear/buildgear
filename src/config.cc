#include "config.h"
#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <list>
#include <stdio.h>
#include <errno.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "buildgear/config.h"

extern void stripChar(string &str, char c);

CConfig::CConfig()
{
   name = "";
   
   download = false;
   download_timeout = 20;
   download_retry = 3;
      
   build = false;
   keep_work = "no";
   update_checksum = "no";
   update_footprint = "no";
   no_strip = "no";
      
   clean = false;
      
   show = false;
   build_order = false;
   download_order = false;
   help = false;
   dependency_circle=false;
   
   all = false;
   
   default_name_prefix = "host/";
   source_dir = SOURCE_DIR;
   
   parallel_builds = 1;
}

void CConfig::CorrectName()
{
   // Prepend default name prefix if none is provided
   if ((CConfig::name.find("host/") != 0) &&
      (CConfig::name.find("build/") != 0))
      CConfig::name = CConfig::default_name_prefix + CConfig::name;
}

void CConfig::CorrectSourceDir(void)
{
   // Replace "~" with $HOME value
   if (CConfig::source_dir.find("~/") == 0)
   {
      FILE *fp;
      char line_buffer[PATH_MAX];
      string command = "echo $HOME";
      string home;
      
      fp = popen(command.c_str(), "r");
      if (fp == NULL)
         throw std::runtime_error(strerror(errno));
      
      while (fgets(line_buffer, PATH_MAX, fp) != NULL)
      {
         home = line_buffer;
         stripChar(home, '\n');
      }
      
      pclose(fp);

      if (home != "")
      {
         CConfig::source_dir.erase(0,1);
         CConfig::source_dir = home + CConfig::source_dir;
      }
   }
}

void CConfig::GuessSystem(void)
{
      FILE *fp;
      char line_buffer[PATH_MAX];
      
      fp = popen(CONFIG_GUESS_SCRIPT, "r");
      if (fp == NULL)
         throw std::runtime_error(strerror(errno));
      
      while (fgets(line_buffer, PATH_MAX, fp) != NULL)
      {
         build_system = line_buffer;
         stripChar(build_system, '\n');
      }
      
      pclose(fp);
}

void CConfig::ShowSystem(void)
{
   cout << "Building '" << CConfig::name << "' for:" << endl;
   cout << "   BUILD  = " << CConfig::build_system << endl;
   cout << "   HOST   = " << CConfig::host_system << endl;
   cout << endl;
}
