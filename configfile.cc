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

void CConfigFile::Parse(string filename, CConfig *config)
{
      FILE *fp;
      char line_buffer[PATH_MAX];
      string command =  "bash -c 'source " + filename + " 2> /dev/null";
      
      if (filename != BUILD_FILES_CONFIG)
         command += "; echo source_dir=$source_dir \
                     ; echo source_mirror=$source_mirror \
                     ; echo required_version=$required_version \
                     ; echo default_name_prefix=$default_name_prefix \
                     ; echo ignore_footprint=$ignore_footprint \
                     ; echo ignore_checksum=$ignore_checksum \
                     ; echo download_parallel_level=$download_parallel_level \
                     ; echo download_timeout=$download_timeout \
                     ; echo download_retry=$download_retry \
                     ; echo build_parallel_level=$build_parallel_level \
                     ; echo package_compression_level=$package_compression_level \
                     '";
      else
         command += "; echo host_toolchain=$HOST_TOOLCHAIN \
                     ; echo target_toolchain=$TARGET_TOOLCHAIN \
                     ; echo build=$BUILD \
                     ; echo host=$HOST \
                     ; echo target=$TARGET \
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
                  config->default_name_prefix = value;
               if (key == CONFIG_KEY_SOURCE_DIR)
                  config->source_dir = value;
               if (key == CONFIG_KEY_DOWNLOAD_TIMEOUT)
                  config->download_timeout = atoi(value.c_str());
               if (key == CONFIG_KEY_DOWNLOAD_RETRY)
                  config->download_retry = atoi(value.c_str());
            }
            else
            {
               // buildfiles/config :
               if (key == CONFIG_KEY_HOST_TOOLCHAIN)
                  config->host_toolchain = value;
               if (key == CONFIG_KEY_TARGET_TOOLCHAIN)
                  config->target_toolchain = value;
               if (key == CONFIG_KEY_BUILD)
                  config->build_system = value;
               if (key == CONFIG_KEY_HOST)
                  config->host_system = value;
               if (key == CONFIG_KEY_TARGET)
                  config->target_system = value;
            }
         }
      }
      pclose(fp);
}
