#ifndef CONFIG_H
#define CONFIG_H

#include "buildgear/utility.h"

using namespace std;

class CConfig : public CUtility
{
   public:
      string name;
      string name_stripped;
      string root;
      
      bool download;
      int download_timeout;
      int download_retry;
      
      bool build;
      string keep_work;
      string update_checksum;
      string update_footprint;
      string no_strip;
      bool no_fakeroot;
      
      bool clean;

      bool show;
      bool build_order;
      bool download_order;
      bool dependency_circle;
      bool readme;
      bool log;
      
      bool all;

      string default_name_prefix;
      string source_dir;
      string download_mirror;
      string download_mirror_first;
      unsigned int parallel_builds;
      string build_system;
      string host_system;
      string cross_depends;
      CConfig();
      void CorrectName(void);
      void CorrectSourceDir(void);
      void GuessSystem(void);
      void ShowSystem(void);
   private:
};


/* Buildgear configuration */
#define ROOT_DIR                 ".buildgear"
#define BUILD_FILES_DIR          "buildfiles"
#define BUILD_FILES_NATIVE_DIR   BUILD_FILES_DIR "/native"
#define BUILD_FILES_CROSS_DIR    BUILD_FILES_DIR "/cross"
#define BUILD_DIR                "build"
#define OUTPUT_DIR               BUILD_DIR "/output"
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
#define BUILD_FILES_README       BUILD_FILES_DIR "/readme"

#define KEY_NAME                 "name"
#define KEY_VERSION              "version"
#define KEY_RELEASE              "release"
#define KEY_SOURCE               "source"
#define KEY_DEPENDS              "depends"

#define CONFIG_KEY_DEFAULT_NAME_PREFIX       "default_name_prefix"
#define CONFIG_KEY_PARALLEL_BUILDS           "parallel_builds"
#define CONFIG_KEY_DOWNLOAD_TIMEOUT          "download_timeout"
#define CONFIG_KEY_DOWNLOAD_RETRY            "download_retry"
#define CONFIG_KEY_DOWNLOAD_MIRROR           "download_mirror"
#define CONFIG_KEY_DOWNLOAD_MIRROR_FIRST     "download_mirror_first"
#define CONFIG_KEY_SOURCE_DIR                "source_dir"

#define CONFIG_KEY_CROSS_DEPENDS             "cross_depends"
#define CONFIG_KEY_BUILD                     "build"
#define CONFIG_KEY_HOST                      "host"

#define TERMINFO_CIVIS           "\033[?25l"
#define TERMINFO_CNORM           "\033[?25h"

#define SCRIPT                   AM_PKGDATADIR "/buildgear.sh"
#define CONFIG_GUESS_SCRIPT      AM_PKGDATADIR "/config.guess"

#define SVG_DEPENDENCY_FILE      BUILD_DIR "/dependency.svg"
#define SVG_COLOR_NATIVE         "#ffff7f"
#define SVG_COLOR_CROSS          "#7fff7f"
#define SVG_DASH                 1
#define SVG_DASH_NO              0

extern CConfig Config;

#endif
