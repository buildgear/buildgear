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
#include "buildgear/configfile.h"

extern void stripChar(string &str, char c);

void CConfigFile::Parse(string filename)
{
      FILE *fp;
      char line_buffer[PATH_MAX];
      string command =  "bash -c 'source " + filename + " 2> /dev/null";
      
      if (filename != BUILD_FILES_CONFIG)
         command += "; echo source_dir=$source_dir \
                     ; echo download_mirror=$download_mirror \
                     ; echo download_mirror_first=$download_mirror_first \
                     ; echo download_timeout=$download_timeout \
                     ; echo download_retry=$download_retry \
                     ; echo required_version=$required_version \
                     ; echo default_name_prefix=$default_name_prefix \
                     ; echo parallel_builds=$parallel_builds \
                     '";
      else
         command += "; echo cross_depends=${CROSS_DEPENDS[@]} \
                     ; echo build=$BUILD \
                     ; echo host=$HOST \
                     '";
      
      fp = popen(command.c_str(), "r");
      if (fp == NULL)
         throw std::runtime_error(strerror(errno));

      while (fgets(line_buffer, PATH_MAX, fp) != NULL)
      {
         // Parse key=value pairs
         string line(line_buffer);
         string key, value;
         size_t pos = line.find_first_of('=');
	
         key=line.substr(0, pos);
         value=line.substr(pos+1);

         stripChar(value, '\n');

         if (value != "")
         {
            if (filename != BUILD_FILES_CONFIG)
            {
               // ~/.buildgearconfig, .buildgear/config :
               if (key == CONFIG_KEY_DEFAULT_NAME_PREFIX)
                  Config.default_name_prefix = value;
               if (key == CONFIG_KEY_SOURCE_DIR)
                  Config.source_dir = value;
               if (key == CONFIG_KEY_DOWNLOAD_TIMEOUT)
                  Config.download_timeout = atoi(value.c_str());
               if (key == CONFIG_KEY_DOWNLOAD_RETRY)
                  Config.download_retry = atoi(value.c_str());
               if (key == CONFIG_KEY_DOWNLOAD_MIRROR)
                  Config.download_mirror = value;
               if (key == CONFIG_KEY_DOWNLOAD_MIRROR_FIRST)
                  Config.download_mirror_first = value;
//               if (key == CONFIG_KEY_PARALLEL_BUILDS)
//                  Config.parallel_builds = atoi(value.c_str());
// Temporarily disabled support for parallel builds
            }
            else
            {
               // buildfiles/config :
               if (key == CONFIG_KEY_CROSS_DEPENDS)
                  Config.cross_depends = value;
               if (key == CONFIG_KEY_BUILD)
                  Config.build_system = value;
               if (key == CONFIG_KEY_HOST)
                  Config.host_system = value;
            }
         }
      }
      pclose(fp);
}
