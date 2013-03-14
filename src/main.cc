/*
 * Build Gear - a lightweight embedded firmware build tool
 *
 * Copyright (c) 2011-2013  Martin Lund
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

#include "config.h"
#include <string>
#include <cstdlib>
#include <thread>
#include <ncurses.h>
#include "buildgear/config.h"
#include "buildgear/signals.h"
#include "buildgear/fakeroot.h"
#include "buildgear/configfile.h"
#include "buildgear/debug.h"
#include "buildgear/clock.h"
#include "buildgear/options.h"
#include "buildgear/filesystem.h"
#include "buildgear/buildfiles.h"
#include "buildgear/dependency.h"
#include "buildgear/source.h"
#include "buildgear/buildmanager.h"
#include "buildgear/download.h"
#include "buildgear/buildsystem.h"
#include "buildgear/cursor.h"
#include "buildgear/log.h"
#include "buildgear/stats.h"

CSignals      Signals;
CFakeroot     Fakeroot;
CDebug        Debug(cout);
CConfig       Config;
COptions      Options;
CConfigFile   ConfigFile;
CClock        Clock;
CFileSystem   FileSystem;
CBuildFiles   BuildFiles;
CDependency   Dependency;
CSource       Source;
CBuildManager BuildManager;
CBuildSystem  BuildSystem;
CCursor       Cursor;
CLog          Log;
CStats        Stats;

int main (int argc, char *argv[])
{
   /* Debug stream option */
   Debug.On() = false;

   /* Start counting elapsed time */
   Clock.Start();

   /* Install custom signal handlers */
   Signals.Install();

   /* Make sure to reenable cursor on exit */
   atexit(clean_up);
   set_terminate(clean_up);

   /* Disable cursor */
   Cursor.hide();

   /* Parse command line options */
   Options.Parse(argc, argv);

   /* Handle init command */
   if (Config.init)
   {
      if (Config.buildfile)
         BuildFiles.Init(Config.name);
      else
         FileSystem.InitRoot();
      exit(EXIT_SUCCESS);
   }

   /* Search for build root directory */
   FileSystem.FindRoot(ROOT_DIR);

   /* Handle config command */
   if (Config.config)
   {
      if ((Config.blist))
         Config.List();
      else
         Config.SetConfig();
      exit(EXIT_SUCCESS);
   }

   /* Make sure basic tools are installed */
   BuildSystem.Check();

   /* Display help hint on incorrect download command */
   if ((Config.download) && (Config.name == "") && (Config.all==false))
   {
      cout << "Please specify build name or use 'download --all' " <<
                                    "to download source files of all builds\n";
      exit(EXIT_FAILURE);
   }

   /* Display help hint on incorrect build command */
   if ((Config.build) && (Config.name == ""))
   {
      cout << "Please specify build name\n";
      exit(EXIT_FAILURE);
   }

   /* Display help hint on incorrect clean command */
   if ((Config.clean) && (Config.name == "") && (Config.all==false))
   {
      if ((Config.footprint))
      {
         cout << "Please specify build name or use 'clean --all ";
         cout << "--footprint' to clean all footprints\n";
         exit(EXIT_FAILURE);
      }

      if ((Config.checksum))
      {
         cout << "Please specify build name or use 'clean --all ";
         cout << "--checksum' to clean all checksums\n";
         exit(EXIT_FAILURE);
      }

      cout << "Please specify build name or use 'clean --all' to clean all builds\n";
      exit(EXIT_FAILURE);
   }

   /* Display help hint on incorrect show command */
   if ((Config.show) && (Config.name == "")
                     && (!Config.readme)
                     && (!Config.log))
   {
      cout << "Please specify build name to show\n";
      exit(EXIT_FAILURE);
   }

   /* Respawn into fakeroot session if building */
   if ((!Config.no_fakeroot) && (Config.build))
      Fakeroot.Respawn(argc, argv);

   /* Show buildfiles readme */
   if ((Config.show) && (Config.readme))
   {
      BuildFiles.ShowReadme();
      exit(EXIT_SUCCESS);
   }

   /* Show build log */
   if (Config.show && Config.log)
   {
      BuildSystem.ShowLog();
      exit(EXIT_SUCCESS);
   }

   /* Lock the build root */
   FileSystem.LockRoot();

   /* Guess build system type */
   Config.GuessBuildSystem();

   /* Parse buildgear configuration file(s) */
   ConfigFile.Parse(Config.home_dir + GLOBAL_CONFIG_FILE);
   ConfigFile.Parse(LOCAL_CONFIG_FILE);

   /* Check terminal size and set max sim. downloads */
   if (Cursor.no_lines < 2 * DOWNLOAD_LINE_SIZE) {
      cout << "Your terminal has " << Cursor.no_lines << " lines. You need at least "
           << DOWNLOAD_LINE_SIZE << " to run buildgear." << endl;
      exit(EXIT_FAILURE);
   }

   if (Cursor.no_lines - 1 < (uint)stoi(Config.bg_config[CONFIG_KEY_DOWNLOAD_CONNECTIONS]) * DOWNLOAD_LINE_SIZE) {
      cout << endl << "Restricting download_connections to "
           << (Cursor.no_lines - 1) / DOWNLOAD_LINE_SIZE << " due to terminal height." << endl;
      Config.bg_config[CONFIG_KEY_DOWNLOAD_CONNECTIONS] = to_string((Cursor.no_lines - 1) / DOWNLOAD_LINE_SIZE);
   }

   /* Parse buildfiles configuration file */
   ConfigFile.Parse(BUILD_FILES_CONFIG);

   /* Correct source dir */
   Config.CorrectSourceDir();

   /* Handle 'clean --all' command */
   if ((Config.clean) && (Config.all) && (Config.name == ""))
   {
      if (Config.footprint)
      {
         cout << "\nCleaning all footprints.. " << flush;
         BuildManager.CleanAllFootprint();
         cout << "Done\n\n";
         exit(EXIT_SUCCESS);
      }

      if (Config.checksum)
      {
         cout << "\nCleaning all checksums.. " << flush;
         BuildManager.CleanAllChecksum();
         cout << "Done\n\n";
         exit(EXIT_SUCCESS);
      }

      cout << "\nCleaning all builds.. " << flush;
      BuildManager.CleanAll();
      cout << "Done\n\n";
      exit(EXIT_SUCCESS);
   }

   /* Correct name - adds default prefix */
   Config.CorrectName();

   /* Show footprint or checksum */
   if (Config.show && (Config.footprint || Config.checksum))
   {
      string build_name;
      build_name = Config.name.substr( Config.name.find_last_of("/") );
      if (Config.name.find("native/") == 0)
      {
         if (Config.footprint)
         {
            if (!FileSystem.Cat(FOOTPRINT_NATIVE_DIR + build_name + ".footprint"))
            {
               cout << endl << "Error: Could not show footprint for build '" << Config.name;
               cout << "'" << endl;
               exit(EXIT_FAILURE);
            }
         } else
         {
            if (!FileSystem.Cat(CHECKSUM_NATIVE_DIR + build_name + ".sha256sum"))
            {
               cout << endl << "Error: Could not show checksum for build '" << Config.name;
               cout << "'" << endl;
               exit(EXIT_FAILURE);
            }
         }
      } else
      {
         if (Config.footprint)
         {
            if (!FileSystem.Cat(FOOTPRINT_CROSS_DIR + build_name + ".footprint"))
            {
               cout << endl << "Error: Could not show footprint for build '" << Config.name;
               cout << "'" << endl;
               exit(EXIT_FAILURE);
            }
         } else
         {
            if (!FileSystem.Cat(CHECKSUM_CROSS_DIR + build_name + ".sha256sum"))
            {
               cout << endl << "Error: Could not show checksum for build '" << Config.name;
               cout << "'" << endl;
               exit(EXIT_FAILURE);
            }
         }
      }
      exit(EXIT_SUCCESS);
   }

   /* Find all build files */
   cout << "\nSearching for build files..     " << flush;
   FileSystem.FindFiles(BUILD_FILES_DIR,
                        BUILD_FILE,
                        &BuildFiles.buildfiles);

   /* Print number of buildfiles found */
   cout << "Done (" << BuildFiles.buildfiles.size() << " files)\n";

   /* Delete old build log */
   BuildManager.CleanLog();

   /* Create build directory */
   FileSystem.CreateDirectory(BUILD_DIR);

   /* Open build log file */
   Log.open(BUILD_LOG);

   /* Parse buildfiles */
   cout << "Loading build files..           " << flush;
   BuildFiles.Parse();

   /* Show buildfiles meta info (debug only) */
   BuildFiles.ShowMeta();

   /* Remove duplicate buildfiles according to layer */
   BuildFiles.RemoveDuplicates();

   /* Load dependencies */
   BuildFiles.LoadDependency();

   /* Handle clean command */
   if (Config.clean)
   {
      cout << "Done\n";
      if (Config.all)
      {
         if (Config.footprint)
         {
            BuildManager.CleanDependenciesFootprint(BuildFiles.BuildFile(Config.name));
            cout << "\n\nDone\n\n";
            exit(EXIT_SUCCESS);
         }

         if (Config.checksum)
         {
            BuildManager.CleanDependenciesChecksum(BuildFiles.BuildFile(Config.name));
            cout << "\n\nDone\n\n";
            exit(EXIT_SUCCESS);
         }

         BuildManager.CleanDependencies(BuildFiles.BuildFile(Config.name));
      } else
      {
         if (Config.footprint)
         {
            BuildManager.CleanFootprint(BuildFiles.BuildFile(Config.name));
            cout << "\n\nDone\n\n";
            exit(EXIT_SUCCESS);
         }

         if (Config.checksum)
         {
            BuildManager.CleanChecksum(BuildFiles.BuildFile(Config.name));
            cout << "\n\nDone\n\n";
            exit(EXIT_SUCCESS);
         }

         BuildManager.Clean(BuildFiles.BuildFile(Config.name));
      }
      cout << "\n\nDone\n\n";
      exit(EXIT_SUCCESS);
   }

   if (Config.bf_config[CONFIG_KEY_CROSS_DEPENDS] != "")
   {
      /* Load dependencies defined in "CROSS_DEPENDS" */
      BuildFiles.LoadCrossDependency();

      /* Add cross dependency to all cross buildfiles */
      BuildFiles.AddCrossDependency();
   }

   cout << "Done\n";

   /* Show version of build */
   if (Config.show_version)
   {
      BuildFiles.ShowVersion(BuildFiles.BuildFile(Config.name));
      exit(EXIT_SUCCESS);
   }

   /* Create temp directory */
   FileSystem.CreateDirectory(Config.tmp_dir);

   if (Config.show && Config.buildfile)
   {
      cout << "\nShowing expanded buildfile for '" << Config.name << "'..\n";
      BuildFiles.BuildFile(Config.name)->Show();
      exit(EXIT_SUCCESS);
   }

   /* Only resolve dependencies if we are not downloading all sources or not downloading  */
   if ( ((Config.download) && (Config.name != Config.bg_config[CONFIG_KEY_DEFAULT_NAME_PREFIX]))
         || !Config.download)
   {
      /* Resolve build dependencies */
      cout << "Resolving dependencies..        ";
      Dependency.ResolveSequentialBuildOrder(Config.name, &BuildFiles.buildfiles);
      Dependency.ResolveParallelBuildOrder();
      cout << "Done (" << (Dependency.resolved.size()-1) << " dependencies)\n";
   }

   /* Find longest build name */
   Dependency.SetNameLength();

   /* Handle show options */
   if (Config.show)
   {
      if (Config.download_order)
         Dependency.ShowDownloadOrder();

      if (Config.build_order)
         Dependency.ShowBuildOrder();

      if (Config.dependency_circle)
      {
         FileSystem.CreateDirectory(OUTPUT_DIR);
         Dependency.ShowDependencyCircleSVG(SVG_DEPENDENCY_PREFIX + Config.name_stripped + ".svg");
      }
      cout << endl;
      exit(EXIT_SUCCESS);
   }

   if (Config.build)
   {
      /* Check for build system prerequisites */
      cout << "Running build system checks..   " << flush;
      BuildSystem.CallCheck(&Dependency.build_order);
      cout << "Done\n";
   }

   /* Create footprint and checksum directories */
   FileSystem.CreateDirectory(FOOTPRINT_NATIVE_DIR);
   FileSystem.CreateDirectory(FOOTPRINT_CROSS_DIR);
   FileSystem.CreateDirectory(CHECKSUM_NATIVE_DIR);
   FileSystem.CreateDirectory(CHECKSUM_CROSS_DIR);

   if (Config.download)
   {
      /* If 'download --all' command then download source of all builds available */
      if ((Config.all) && (Config.name == Config.bg_config[CONFIG_KEY_DEFAULT_NAME_PREFIX]))
         Dependency.download_order=BuildFiles.buildfiles;

      /* If 'download <name>' command then only download source of named build  */
      if ((!Config.all) && (Config.name != Config.bg_config[CONFIG_KEY_DEFAULT_NAME_PREFIX]))
      {
         Dependency.download_order.clear();
         Dependency.download_order.push_back(Dependency.build_order.back());
      }
   }

   /* Download */
   cout << "Downloading sources..           " << flush;

   /* Disable terminal echo */
   Cursor.disable_echo();
   Cursor.disable_wrap();

   thread download_thread([&] (void) {Source.Download(&Dependency.download_order,
                                      Config.bg_config[CONFIG_KEY_SOURCE_DIR]);});

   download_thread.join();
   cout << "Done\n";

   /* Quit if download command is used */
   if (Config.download)
      exit(EXIT_SUCCESS);

   /* Show system information */
   cout << "Detecting BUILD system type..   " << Config.bf_config[CONFIG_KEY_BUILD] << endl;
   cout << "Configured HOST system type..   " << Config.bf_config[CONFIG_KEY_HOST] << endl;

   /* Delete old work */
   cout << "Cleaning old work directory..   " << flush;
   BuildManager.CleanWork();
   cout << "Done" << endl << endl;

   /* Create build output directory */
   FileSystem.CreateDirectory(OUTPUT_DIR);

   if (Config.load_chart)
      Stats.enableCapture();

   /* Start building */
   cout << "Building '" << Config.name << "'.. " << flush;
   BuildManager.Build(&Dependency.parallel_build_order);
   if (Config.keep_work == "no" && !BuildManager.build_error)
      BuildManager.CleanWork();
   Cursor.clear_below();
   if (BuildManager.build_error)
      cout << "Error\n\n";
   else
      cout << "Done\n\n";

   if (Config.load_chart)
   {
      Stats.disableCapture();
      Stats.saveCapture(LOAD_CHART_PREFIX + Config.name_stripped + ".svg");
   }

   /* Reenable terminal echo */
   Cursor.enable_wrap();
   Cursor.enable_echo();

   /* Close build log */
   Log.close();

   /* Show log location */
   cout << "See " BUILD_LOG " for details.\n\n";

   /* Stop counting elapsed time */
   Clock.Stop();

   /* Show elapsed time */
   Clock.ShowElapsedTime();

   /* Enable cursor again */
   Cursor.restore();
}
