/*
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
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include "buildgear/config.h"
#include "buildgear/options.h"

void COptions::Parse(int argc, char *argv[])
{
   int option;
   string command;
   
   /* Print usage help if no arguments are provided */
   if (argc == 1)
   {
      COptions::ShowHelp(argv);
      exit(EXIT_SUCCESS);
   }
   
   // Save second argument (assumed to be COMMAND)
   command = argv[1];
   
   // getopt_long stores the option index here
   int option_index = 0;
   
   // Skip ahead past command
   optind = 2;

   // Help command
   if (command == "help")
   {
      // If a command is specified, show help for that
      if (argc > 2)
      {
         string help_command;

         help_command = argv[2];

         if (help_command == "download")
            execlp("man", "man", "buildgear-download", NULL);
         else if (help_command == "build")
            execlp("man", "man", "buildgear-build", NULL);
         else if (help_command == "clean")
            execlp("man", "man", "buildgear-clean", NULL);
         else if (help_command == "show")
            execlp("man", "man", "buildgear-show", NULL);
         else if (help_command == "init")
            execlp("man", "man", "buildgear-init", NULL);
         else
            COptions::ShowHelp(argv);
      } else
         COptions::ShowHelp(argv);

      exit(EXIT_SUCCESS);
   }
   
   // Download command
   if (command == "download")
   {
      Config.download = true;
      
      // Parse download OPTIONS
      static struct option long_options[] =
      {
         {"all", no_argument, 0, 'a'},
         {0,     0,           0,  0 }
      };
      
      option = getopt_long (argc, argv, "", long_options, &option_index);
      while( option != -1 ) 
      {
         switch( option ) 
         {
            case 'a':
               Config.all = true;
               break;
            default:
               exit(EXIT_FAILURE);
               break;
         }
         option = getopt_long (argc, argv, "", long_options, &option_index);
      }
   }
   // Build command
   else if (command == "build")
   {
      Config.build = true;
      
      // Parse build OPTIONS
      static struct option long_options[] =
		{
         {"keep-work",        no_argument, 0, 'w'},
         {"update-checksum",  no_argument, 0, 'c'},
         {"update-footprint", no_argument, 0, 'f'},
         {"no-strip",         no_argument, 0, 's'},
         {"all",              no_argument, 0, 'a'},
         {"no-fakeroot",      no_argument, 0, 'n'},
         {0,                  0,           0,  0 }
		};
      
      option = getopt_long (argc, argv, "", long_options, &option_index);
      while( option != -1 ) 
      {
         switch( option ) 
         {
            case 'w':
               Config.keep_work = true;
               break;
            case 'c':
               Config.update_checksum = "yes";
               break;
            case 'f':
               Config.update_footprint = "yes";
               break;
            case 's':
               Config.no_strip = "yes";
               break;
            case 'a':
               Config.all = true;
               break;
            case 'n':
               Config.no_fakeroot = true;
               break;
            default:
               exit(EXIT_FAILURE);
               break;
         }
         option = getopt_long (argc, argv, "", long_options, &option_index);
      }
   }
   // Clean command
   else if (command == "clean")
   {
      Config.clean = true;
      
      // Parse clean OPTIONS
      static struct option long_options[] =
      {
         {"all", no_argument, 0, 'a'},
         {0,     0,           0,  0 }
      };
      
      option = getopt_long (argc, argv, "", long_options, &option_index);
      while( option != -1 ) 
      {
         switch( option ) 
         {
            case 'a':
               Config.all = true;
               break;
            default:
               exit(EXIT_FAILURE);
               break;
         }
		option = getopt_long (argc, argv, "", long_options, &option_index);
      }
   }
   // Show command
   else if (command == "show")
   {
      Config.show = true;
      
      // Parse show OPTIONS
      static struct option long_options[] =
      {
         {"build-order",         no_argument, 0, 'o'},
         {"download-order",      no_argument, 0, 'd'},
         {"dependency",          no_argument, 0, 'c'},
         {"readme",              no_argument, 0, 'r'},
         {"log",                 no_argument, 0, 'l'},
         {"log-tail",            no_argument, 0, 't'},
         {"log-mismatch",        no_argument, 0, 'm'},
         {"version",             no_argument, 0, 'v'},
         {0,                     0,           0,  0 }
      };
      
      option = getopt_long (argc, argv, "", long_options, &option_index);
      while( option != -1 ) 
      {
         switch( option ) 
         {
            case 'o':
               Config.build_order = true;
               break;
            case 'd':
               Config.download_order = true;
               break;
            case 'c':
               Config.dependency_circle = true;
               break;
            case 'r':
               Config.readme = true;
               break;				
            case 'l':
               Config.log = true;
               break;
            case 't':
               Config.log = true;
               Config.log_tail = true;
               break;
            case 'm':
               Config.log = true;
               Config.mismatch = true;
               break;
            case 'v':
               Config.show_version = true;
               break;
            default:
               exit(EXIT_FAILURE);
               break;
         }
         option = getopt_long (argc, argv, "", long_options, &option_index);
      }
   }
   // Init command
   else if (command == "init")
   {
      Config.init = true;
   }
   // No-command
   else
   {
      // No command provided so we don't skip ahead
      optind = 1;
      
      // Parse OPTIONS
      static struct option long_options[] =
         {
            {"version", no_argument, 0, 'v'},
            {"help",    no_argument, 0, 'h'},
            {0,         0,           0,  0 }
         };
      
      option = getopt_long (argc, argv, "", long_options, &option_index);
      while( option != -1 ) 
      {
         switch( option ) 
         {
            case 'v':
               COptions::ShowVersion();
               exit(EXIT_SUCCESS);
               break;
            case 'h':
               COptions::ShowHelp(argv);
               exit(EXIT_SUCCESS);
               break;
            default:
               exit(EXIT_FAILURE);
               break;
         }
         option = getopt_long (argc, argv, "", long_options, &option_index);
      }
      COptions::ShowHelp(argv);
      exit(EXIT_SUCCESS);
   }

   // Handle remaining options (non '--', '-' options)
   if (optind < argc)
   {
      // Get NAME of build
      Config.name = argv[optind++];

      // Create name stripped from any cross/ or native/ parts
      Config.name_stripped = Config.name;
      int pos = Config.name_stripped.rfind("/");
      if (pos != string::npos)
         Config.name_stripped.erase(0,++pos);
      
      if (optind < argc)
      {
         // Warn if too many arguments
         cout <<  "Too many arguments: ";
         while (optind < argc)
            cout << argv[optind++] << " ";
         cout << endl;
         exit(EXIT_FAILURE);
      }
   }
}

void COptions::ShowHelp(char *argv[])
{
   cout << "Usage: " << argv[0] << " [COMMAND] [OPTIONS] [BUILD NAME]\n";
   cout << "\n";
   cout << "Commands:\n";
   cout << "  download                Download source files\n";
   cout << "  build                   Build\n";
   cout << "  clean                   Clean\n";
   cout << "  show                    Show various information\n";
   cout << "  init                    Create an empty build area\n";
   cout << "\n";
   cout << "Download options:\n";
   cout << "  --all                   Download source files of all builds or build dependencies\n";
   cout << "\n";
   cout << "Build options:\n";
   cout << "  --keep-work             Do not delete work files\n";
   cout << "  --update-checksum       Update source checksum\n";
   cout << "  --update-footprint      Update footprint\n";
   cout << "  --no-strip              Do not strip libraries and executables\n";
   cout << "  --no-fakeroot           Do not use fakeroot\n";
   cout << "  --all                   Apply to all build dependencies\n";
   cout << "\n";
   cout << "Clean options:\n";
   cout << "  --all                   Clean all builds or build dependencies\n";
   cout << "\n";
   cout << "Show options:\n";
   cout << "  --build-order           Show build order\n";
   cout << "  --download-order        Show download order\n";
   cout << "  --dependency            Show dependency graph\n";
   cout << "  --readme                Show buildfiles readme\n";
   cout << "  --version               Show build version\n";
   cout << "  --log                   Show build log\n";
   cout << "  --log-tail              Show build log (tailed)\n";
   cout << "  --log-mismatch          Show build log mismatches\n";
   cout << "\n";
   cout << "Options:\n";
   cout << "  --version               Display version\n";
   cout << "  --help                  Display help\n";
   cout << "\n";
   cout << "See buildgear help <command> for help on a specific command\n";
}

void COptions::ShowVersion(void)
{
   cout << "Build Gear " << VERSION << "\n";
   cout << "Copyright (C) 2011-2012 Martin Lund\n";
   cout << "\n";
   cout << "License GPLv2+: GNU GPL version 2 or later <http://gnu.org/licenses/gpl-2.0.html>.\n";
   cout << "This is free software: you are free to change and redistribute it.\n";
   cout << "There is NO WARRANTY, to the extent permitted by law.\n";
   cout << "\n";
}
