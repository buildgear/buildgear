/*
 * This file is part of Build Gear.
 *
 * Copyright (C) 2011-2013  Martin Lund
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <unordered_map>
#include "buildgear/utility.h"

using namespace std;

class CConfig : public CUtility
{
   public:
      string name;
      string name_stripped;
      string root;
      pid_t pid;
      string tmp_dir;

      unordered_map <string, string> bg_config;
      unordered_map <string, string> bg_config_default;
      unordered_map <string, string> bg_config_local;
      unordered_map <string, string> bg_config_global;
      unordered_map <string, string> bf_config;

      bool download;

      bool build;
      string keep_work;
      string update_checksum;
      string update_footprint;
      string no_strip;
      bool no_fakeroot;
      bool load_chart;

      bool clean;

      bool show;
      bool build_order;
      bool download_order;
      bool dependency_circle;
      bool readme;
      bool show_version;
      bool log;
      bool log_tail;
      bool mismatch;
      bool footprint;
      bool checksum;
      bool buildfile;

      bool manifest_plain_text;
      bool manifest_xml;
      bool manifest_html;

      bool init;

      bool config;
      bool global;
      bool unset;
      bool blist;
      string key;
      string value;

      bool all;

      string home_dir;
      string cmdline;
      string sys_datetime;

      CConfig();
      void CorrectName(void);
      void CorrectSourceDir(void);
      void GuessBuildSystem(void);
      void SetConfig(void);
      void List(void);
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
#define MANIFEST_FILE            OUTPUT_DIR "/manifest"
#define FOOTPRINT_NATIVE_DIR     ".footprint/native"
#define FOOTPRINT_CROSS_DIR      ".footprint/cross"
#define CHECKSUM_NATIVE_DIR      ".checksum/native"
#define CHECKSUM_CROSS_DIR       ".checksum/cross"
#define BUILD_FILE               "Buildfile"
#define PACKAGE_EXTENSION        ".pkg.tar.gz"

#define GLOBAL_CONFIG_FILE       "/.buildgearconfig"
#define LOCAL_CONFIG_FILE        ROOT_DIR "/config"
#define BUILD_FILES_CONFIG_DIR   "config"
#define BUILD_FILES_CONFIG       BUILD_FILES_CONFIG_DIR "/config"
#define BUILD_FILES_README       "README"
#define LAYER_FILE_NAME          ".layer"
#define DEFAULT_LAYER_NAME       "default"
#define LOCK_FILE                ROOT_DIR "/.lock"

#define TEMP(a)                  (Config.tmp_dir + string(a))
#define SCRIPT_OUTPUT_FIFO       TEMP("script_output.fifo")
#define SHOW_BUILD_FILE          TEMP("show_buildfile.tmp")

#define KEY_NAME                 "name"
#define KEY_VERSION              "version"
#define KEY_RELEASE              "release"
#define KEY_SOURCE               "source"
#define KEY_DEPENDS              "depends"
#define KEY_OPTIONS_             "options"
#define KEY_LAYER                "layer"
#define KEY_BUILD_FUNCTION       "build_function"
#define KEY_CHECK_FUNCTION       "check_function"
#define KEY_CONFIG_RETURN        "config_return"
#define KEY_BUILD_RETURN         "build_return"

// Buildgear config
#define CONFIG_KEY_SOURCE_DIR                "source_dir"
#define CONFIG_KEY_DOWNLOAD_MIRROR_FIRST     "download_mirror_first"
#define CONFIG_KEY_DOWNLOAD_TIMEOUT          "download_timeout"
#define CONFIG_KEY_DOWNLOAD_RETRY            "download_retry"
#define CONFIG_KEY_DOWNLOAD_CONNECTIONS      "download_connections"
#define CONFIG_KEY_PARALLEL_BUILDS           "parallel_builds"
#define CONFIG_KEY_CERTIFICATE_DIR           "certificate_dir"
#define CONFIG_KEY_SSH_PUBLIC_KEYFILE        "ssh_public_key"
#define CONFIG_KEY_SSH_PRIVATE_KEYFILE       "ssh_private_key"
#define CONFIG_KEY_LOG_ROTATION              "log_rotation"

// Buildfiles config
#define CONFIG_KEY_DEFAULT_NAME_PREFIX       "default_name_prefix"
#define CONFIG_KEY_DOWNLOAD_MIRROR           "download_mirror"
#define CONFIG_KEY_LAYERS                    "layers"

#define CONFIG_KEY_CROSS_DEPENDS             "cross_depends"
#define CONFIG_KEY_BUILD                     "build"
#define CONFIG_KEY_HOST                      "host"

#define SCRIPT                   AM_PKGDATADIR "/buildgear.sh"
#define CONFIG_GUESS_SCRIPT      AM_PKGDATADIR "/config.guess"
#define TEMPLATE_CONFIG          AM_PKGDATADIR "/template.config"
#define TEMPLATE_README          AM_PKGDATADIR "/template.readme"
#define TEMPLATE_LOCAL_CONFIG    AM_PKGDATADIR "/template.buildgearconfig"
#define TEMPLATE_BUILDFILE       AM_PKGDATADIR "/template.buildfile"

#define SVG_DEPENDENCY_PREFIX    OUTPUT_DIR "/dependency."
#define SVG_COLOR_NATIVE         "#ffff7f"
#define SVG_COLOR_CROSS          "#7fff7f"
#define SVG_DASH                 1
#define SVG_DASH_NO              0

#define LOAD_CHART_PREFIX        OUTPUT_DIR "/load-chart."
#define LOAD_CHART_WIDTH         500.0
#define LOAD_CHART_HEIGHT        100.0
#define LOAD_CHART_XLINES        14
#define LOAD_CHART_RESOLUTION    1
#define LOAD_CHART_MARGIN        10
#define LOAD_CHART_LINE_HEIGHT   8

#define LOG_BUFFER_SIZE          10000000
#define LAYER_NAME_SIZE          32
#define OUTPUT_PREFIX_SIZE       12

extern CConfig Config;

#endif
