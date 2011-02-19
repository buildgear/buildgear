/* Buildgear static configuration */
#define VERSION                "0.2 alpha"

#define GLOBAL_CONFIG_FILE     ".buildgearconfig"
#define LOCAL_CONFIG_FILE      "buildgear.config"
#define BUILD_FILE             "Buildfile"

#define ROOT_DIR               ".buildgear"
#define BUILD_FILES_DIR        "buildfiles"
#define BUILD_FILES_HOST_DIR   BUILD_FILES_DIR "/host"
#define BUILD_FILES_TARGET_DIR BUILD_FILES_DIR "/target"
#define BUILD_DIR              "build"
#define SOURCE_DIR             BUILD_DIR  "/source"
#define SOURCE_TEMP_DIR        SOURCE_DIR "/.temp"
#define WORK_DIR               BUILD_DIR  "/work"
#define PACKAGE_DIR            BUILD_DIR  "/package"

#define KEY_NAME               "name"
#define KEY_VERSION            "version"
#define KEY_RELEASE            "release"
#define KEY_SOURCE             "source"
#define KEY_DEPENDS            "depends"

#define HOST                   0
#define TARGET                 1

#define TERMINFO_CIVIS         "\033[?25l"
#define TERMINFO_CNORM         "\033[?25h"
#define BUILD_SCRIPT           "/home/mgl/projects/buildgear/git/buildgear/buildgear.sh"
