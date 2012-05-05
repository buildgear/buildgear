#ifndef CONFIG_H
#define CONFIG_H

//#include "buildgear/filesystem.h"

using namespace std;

class CConfig
{
   public:
      string name;
      string root;
      
      bool download;
      int download_timeout;
      int download_retry;
      
      bool build;
      string keep_work;
      string update_checksum;
      string update_footprint;
      string no_strip;
      
      bool clean;
      
      bool show;
      bool build_order;
      bool download_order;
      bool help;
      bool dependency_circle;
      
      bool all;
      
      string default_name_prefix;
      string source_dir;
      unsigned int parallel_builds;
      string build_system;
      string host_system;
      string build_toolchain;
      string host_toolchain;
      CConfig();
      void CorrectName(void);
      void CorrectSourceDir(void);
      void GuessSystem(void);
      void ShowSystem(void);
   private:
};


/* Buildgear static configuration */
#define VERSION                  "0.7 beta"

#define ROOT_DIR                 ".buildgear"
#define BUILD_FILES_DIR          "buildfiles"
#define BUILD_FILES_BUILD_DIR    BUILD_FILES_DIR "/build"
#define BUILD_FILES_HOST_DIR     BUILD_FILES_DIR "/host"
#define BUILD_DIR                "build"
#define SOURCE_DIR               BUILD_DIR "/source"
#define WORK_DIR                 BUILD_DIR "/work"
#define PACKAGE_DIR              BUILD_DIR "/package"
#define SYSROOT_DIR              WORK_DIR  "/sysroot"
#define BUILD_LOG                BUILD_DIR "/build.log"
#define BUILD_FILE               "Buildfile"
#define PACKAGE_EXTENSION        ".pkg.tar.gz"

#define GLOBAL_CONFIG_FILE       "~/.buildgearconfig"
#define LOCAL_CONFIG_FILE        ROOT_DIR "/config"
#define BUILD_FILES_CONFIG       BUILD_FILES_DIR "/config"
#define BUILD_FILES_HELP         BUILD_FILES_DIR "/help"
#define BUILD_FILES_CHECK        BUILD_FILES_DIR "/check"

#define KEY_NAME                 "name"
#define KEY_VERSION              "version"
#define KEY_RELEASE              "release"
#define KEY_SOURCE               "source"
#define KEY_DEPENDS              "depends"

#define CONFIG_KEY_DEFAULT_NAME_PREFIX       "default_name_prefix"
#define CONFIG_KEY_PARALLEL_BUILDS           "parallel_builds"
#define CONFIG_KEY_DOWNLOAD_TIMEOUT          "download_timeout"
#define CONFIG_KEY_DOWNLOAD_RETRY            "download_retry"
#define CONFIG_KEY_SOURCE_DIR                "source_dir"
#define CONFIG_KEY_SOURCE_MIRROR             "source_mirror"

#define CONFIG_KEY_BUILD_TOOLCHAIN           "build_toolchain"
#define CONFIG_KEY_HOST_TOOLCHAIN            "host_toolchain"
#define CONFIG_KEY_BUILD                     "build"
#define CONFIG_KEY_HOST                      "host"

#define TERMINFO_CIVIS           "\033[?25l"
#define TERMINFO_CNORM           "\033[?25h"

#define SCRIPT "/home/mgl/projects/buildgear/git/buildgear/buildgear.sh"
#define CONFIG_GUESS_SCRIPT "/home/mgl/projects/buildgear/git/buildgear/config.guess"

#define SVG_DEPENDENCY_FILE      "dependency.svg"

#endif

extern CConfig Config;
