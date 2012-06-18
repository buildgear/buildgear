#include "config.h"
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
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
   
   // Download command
   if (command == "download")
   {
      Config.download = true;
      
      // Parse command OPTIONS
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
      
      // Parse command OPTIONS
      static struct option long_options[] =
		{
         {"keep-work",        no_argument, 0, 'w'},
         {"update-checksum",  no_argument, 0, 'c'},
         {"update-footprint", no_argument, 0, 'f'},
         {"no-strip",         no_argument, 0, 's'},
         {"all",              no_argument, 0, 'a'},
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
      
      // Parse command OPTIONS
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
      
      // Parse command OPTIONS
      static struct option long_options[] =
      {
         {"build-order",         no_argument, 0, 'o'},
         {"download-order",      no_argument, 0, 'd'},
         {"dependency",          no_argument, 0, 'c'},
         {"readme",              no_argument, 0, 'r'},
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
            default:
               exit(EXIT_FAILURE);
               break;
         }
         option = getopt_long (argc, argv, "", long_options, &option_index);
      }
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
      exit(EXIT_SUCCESS);
   }

   // Handle remaining options (non '--', '-' options)
   if (optind < argc)
   {
      // Get NAME of build
      Config.name = argv[optind++];
      
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
   cout << "Usage: " << argv[0] << " [COMMAND] [OPTIONS] [BUILD NAME]\n\n";
   cout << "Commands:\n";
   cout << "  download                Download source files\n";
   cout << "  build                   Build build\n";
   cout << "  clean                   Clean build\n";
   cout << "  show                    Show various information\n\n";
   cout << "Download options:\n";
   cout << "  --all                   Download source files of all builds or build dependencies\n\n";
   cout << "Build options:\n";
   cout << "  --keep-work             Do not delete work files\n";
   cout << "  --update-checksum       Update source checksum\n";
   cout << "  --update-footprint      Update footprint\n";
   cout << "  --no-strip              Do not strip libraries or executables\n";
   cout << "  --all                   Apply to all build dependencies\n\n";
   cout << "Clean options:\n";
   cout << "  --all                   Clean all builds or build dependencies\n\n";
   cout << "Show options:\n";
   cout << "  --build-order           Show build order\n";
   cout << "  --download-order        Show download order\n";
   cout << "  --dependency            Show dependecy graph\n";
   cout << "  --readme                Show buildfiles readme\n\n";
   cout << "Options:\n";
   cout << "  --version               Display version\n";
   cout << "  --help                  Display help\n";
}

void COptions::ShowVersion(void)
{
   cout << "buildgear " << VERSION << "\n";
   cout << "Copyright (C) 2011-2012 Martin Lund\n";
   cout << "\n";
   cout << "License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>.\n";
   cout << "This is free software: you are free to change and redistribute it.\n";
   cout << "There is NO WARRANTY, to the extent permitted by law.\n\n";
}
