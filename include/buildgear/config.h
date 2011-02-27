#ifndef CONFIG_H
#define CONFIG_H

#include "buildgear/filesystem.h"

using namespace std;

class CConfig
{
   public:
      string name;
      
      bool download;
      bool download_all;
      
      bool build;
      bool keep_work;
      bool keep_work_all;
      bool keep_sysroot;
      bool update_checksum;
      bool update_checksum_all;
      bool update_footprint;
      bool update_footprint_all;
      bool no_strip;
      bool no_strip_all;
      bool no_download;
      
      bool clean;
      bool clean_all;
      
      bool show;
      bool build_order;
      bool build_actions;
      bool download_order;
      bool footprint_conflicts;
      bool help;
      
      string default_name_prefix;
      string source_dir;
      string build_system;
      string host_system;
      string target_system;
      string target_toolchain;
      CConfig();
      void CorrectName(void);
      void CorrectSourceDir(void);
      void GuessSystem(void);
      void ShowSystem(void);
   private:
};


/* Buildgear static configuration */
#define VERSION                  "0.4 beta"

#define ROOT_DIR                 ".buildgear"
#define BUILD_FILES_DIR          "buildfiles"
#define BUILD_FILES_HOST_DIR     BUILD_FILES_DIR "/host"
#define BUILD_FILES_TARGET_DIR   BUILD_FILES_DIR "/target"
#define BUILD_DIR                "build"
#define SOURCE_DIR               BUILD_DIR  "/source"
#define WORK_DIR                 BUILD_DIR  "/work"
#define PACKAGE_DIR              BUILD_DIR  "/package"
#define PACKAGE_EXTENSION        ".pkg.tar.gz"

#define GLOBAL_CONFIG_FILE       "~/.buildgearconfig"
#define LOCAL_CONFIG_FILE        ROOT_DIR "/config"
#define BUILD_FILE               "Buildfile"
#define BUILD_FILES_CONFIG       BUILD_FILES_DIR "/config"
#define BUILD_LOG_FILE           BUILD_DIR "/build.log"
#define BUILD_CONFIG_FILE        BUILD_FILES_DIR "/config"
#define BUILD_HELP_FILE          BUILD_FILES_DIR "/help"
#define BUILD_CHECK_FILE         BUILD_FILES_DIR "/check"

#define KEY_NAME                 "name"
#define KEY_VERSION              "version"
#define KEY_RELEASE              "release"
#define KEY_SOURCE               "source"
#define KEY_DEPENDS              "depends"

#define CONFIG_KEY_DEFAULT_NAME_PREFIX       "default_name_prefix"
#define CONFIG_KEY_BUILD_PARALLEL_LEVEL      "build_parallel_level"
#define CONFIG_KEY_DOWNLOAD_PARALLEL_LEVEL   "download_parallel_level"
#define CONFIG_KEY_SOURCE_DIR                "source_dir"
#define CONFIG_KEY_SOURCE_MIRROR             "source_mirror"
#define CONFIG_KEY_PACKAGE_COMPRESSION_LEVEL "package_compression_level"
#define CONFIG_KEY_IGNORE_CHECKSUM           "ignore_checksum"
#define CONFIG_KEY_IGNORE_FOOTPRINT          "ignore_footprint"

#define CONFIG_KEY_TARGET_TOOLCHAIN          "target_toolchain"
#define CONFIG_KEY_BUILD                     "build"
#define CONFIG_KEY_HOST                      "host"
#define CONFIG_KEY_TARGET                    "target"

#define HOST                     0
#define TARGET                   1

#define TERMINFO_CIVIS           "\033[?25l"
#define TERMINFO_CNORM           "\033[?25h"

#define SCRIPT "/home/mgl/projects/buildgear/git/buildgear/buildgear.sh"
#define CONFIG_GUESS_SCRIPT "/home/mgl/projects/buildgear/git/buildgear/config.guess"

#endif


