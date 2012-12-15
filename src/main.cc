/*
 * Build Gear - a lightweight embedded firmware build tool
 *
 * Copyright (C) 2011-2012  Martin Lund
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

int main (int argc, char *argv[])
{
   /* Debug stream option */
   Debug.On() = false;

   /* Start counting elapsed time */
   Clock.Start();

   /* Install custom signal handlers */
   Signals.Install();

   /* Make sure to reenable cursor on exit */
   atexit(cursor_restore);

   /* Disable cursor */
   Cursor.hide();

   /* Parse command line options */
   Options.Parse(argc, argv);

   /* Handle init command */
   if (Config.init)
   {
      FileSystem.InitRoot();
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
      cout << "Please specify build name or use 'clean --all' to clean all builds\n";
      exit(EXIT_FAILURE);
   }

   /* Display help hint on incorrect show command */
   if ((Config.show) && (Config.name == "")
                     && (!Config.readme)
                     && (!Config.log)
                     && (!Config.show_version))
   {
      cout << "Please specify build name to show\n";
      exit(EXIT_FAILURE);
   }

   /* Respawn into fakeroot session if building */
   if ((!Config.no_fakeroot) && (Config.build))
      Fakeroot.Respawn(argc, argv);

   /* Search for build root directory */
   FileSystem.FindRoot(ROOT_DIR);

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

   /* Guess build system type */
   Config.GuessBuildSystem();

   /* Parse buildgear configuration file(s) */
   ConfigFile.Parse(GLOBAL_CONFIG_FILE);
   ConfigFile.Parse(LOCAL_CONFIG_FILE);

   /* Check terminal size and set max sim. downloads */
   if (Cursor.no_lines < 2 * DOWNLOAD_LINE_SIZE) {
      cout << "Your terminal has " << Cursor.no_lines << ". You need at least "
           << DOWNLOAD_LINE_SIZE << " to run buildgear." << endl;
      exit(EXIT_FAILURE);
   }

   if (Cursor.no_lines - 1 < Config.download_connections * DOWNLOAD_LINE_SIZE) {
      cout << endl << "Restricting download_connections to "
           << (Cursor.no_lines - 1) / DOWNLOAD_LINE_SIZE << " due to terminal height." << endl;
      Config.download_connections = (Cursor.no_lines - 1) / DOWNLOAD_LINE_SIZE;
   }

   /* Parse buildfiles configuration file */
   ConfigFile.Parse(BUILD_FILES_CONFIG);
   
   /* Correct source dir */
   Config.CorrectSourceDir();

   /* Correct name - adds default prefix */
   Config.CorrectName();

   /* Handle 'clean --all' command */
   if ((Config.clean) && (Config.all))
   {
      cout << "\nCleaning all builds.. " << flush;
      BuildManager.CleanAll();
      cout << "Done\n\n";
      exit(EXIT_SUCCESS);
   }

   /* Find all build files */
   cout << "\nSearching for build files..     " << flush;
   FileSystem.FindFiles(BUILD_FILES_DIR,
                        BUILD_FILE,
                        &BuildFiles.buildfiles);

   /* Print number of buildfiles found */
   cout << "Done (" << BuildFiles.buildfiles.size() << " files)\n";

   /* Parse buildfiles */
   cout << "Loading build files..           " << flush;
   BuildFiles.Parse();

   /* Show buildfiles meta info (debug only) */
   BuildFiles.ShowMeta();
   
   /* Load dependencies */
   BuildFiles.LoadDependency();

   if (Config.cross_depends != "")
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

   /* Handle clean command */
   if (Config.clean)
   {
      cout << "\nCleaning build '" << Config.name << "'.. ";
      BuildManager.Clean(BuildFiles.BuildFile(Config.name));
      cout << "Done\n\n";
      exit(EXIT_SUCCESS);
   }

   /* Only resolve dependencies if we are not downloading all sources or not downloading  */
   if ((Config.download) && (Config.name != Config.default_name_prefix) || !Config.download)
   {
      /* Resolve build dependencies */
      cout << "Resolving dependencies..        ";
      Dependency.ResolveSequentialBuildOrder(Config.name, &BuildFiles.buildfiles);
      Dependency.ResolveParallelBuildOrder();
      cout << "Done (" << (Dependency.resolved.size()-1) << " dependencies)\n";
   }

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
         Dependency.ShowDependencyCircleSVG(OUTPUT_DIR "/dependency." + Config.name_stripped + ".svg");
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

   /* Create build directory */
   FileSystem.CreateDirectory(BUILD_DIR);

   if (Config.download)
   {
      /* If 'download --all' command then download source of all builds available */
      if ((Config.all) && (Config.name == Config.default_name_prefix))
         Dependency.download_order=BuildFiles.buildfiles;

      /* If 'download <name>' command then only download source of named build  */
      if ((!Config.all) && (Config.name != Config.default_name_prefix))
      {
         Dependency.download_order.clear();
         Dependency.download_order.push_back(Dependency.build_order.back());
      }
   }

   /* Download */
   cout << "Downloading sources..           " << flush;

   /* Disable terminal echo */
   system("stty -echo");
   Cursor.disable_wrap();

   Source.Download(&Dependency.download_order, Config.source_dir);

   Cursor.enable_wrap();
   /* Reenable terminal echo */
   system("stty echo");
   cout << "Done\n";

   /* Quit if download command is used */
   if (Config.download)
      exit(EXIT_SUCCESS);

   /* Show system information */
   cout << "Detected BUILD system type..    " << Config.build_system << endl;
   cout << "Configured HOST system type..   " << Config.host_system << endl;
   cout << endl;
   
   /* Delete old build log */
   BuildManager.CleanLog();

   /* Delete old work */
   BuildManager.CleanWork();
   
   /* Create build output directory */
   FileSystem.CreateDirectory(OUTPUT_DIR);

   /* Start building */
   cout << "Building '" << Config.name << "'.. " << flush;
   BuildManager.Build(&Dependency.parallel_build_order);
   if (Config.keep_work == "no")
      BuildManager.CleanWork();
   cout << "Done\n\n";
   
   /* Show log location */
   cout << "See " BUILD_LOG " for details.\n\n";

   /* Stop counting elapsed time */
   Clock.Stop();
   
   /* Show elapsed time */
   Clock.ShowElapsedTime();
   
   /* Enable cursor again */
   Cursor.restore();
}
