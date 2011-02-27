#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "buildgear/config.h"
#include "buildgear/options.h"

void COptions::Parse(int argc, char *argv[], CConfig *config)
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
      config->download = true;
      
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
               config->download_all = true;
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
      config->build = true;
      
      // Parse command OPTIONS
      static struct option long_options[] =
		{
         {"keep-work",            no_argument, 0, 'w'},
         {"keep-work-all",        no_argument, 0, 'W'},
         {"keep-sysroot",         no_argument, 0, 'r'},
         {"update-checksum",      no_argument, 0, 'c'},
         {"update-checksum-all",  no_argument, 0, 'C'},
         {"update-footprint",     no_argument, 0, 'f'},
         {"update-footprint-all", no_argument, 0, 'F'},
         {"no-strip",             no_argument, 0, 's'},
         {"no-strip-all",         no_argument, 0, 'S'},
         {"no-download",          no_argument, 0, 'd'},
			{0,                      0,           0,  0 }
		};
      
      option = getopt_long (argc, argv, "", long_options, &option_index);
      while( option != -1 ) 
      {
         switch( option ) 
         {
            case 'w':
               config->keep_work = true;
               break;
            case 'W':
               config->keep_work_all = true;
               break;
            case 'r':
               config->keep_sysroot = true;
               break;
            case 'c':
               config->update_checksum = true;
               break;
            case 'C':
               config->update_checksum_all = true;
               break;
            case 'f':
               config->update_footprint = true;
               break;
            case 'F':
               config->update_footprint_all = true;
               break;
            case 's':
               config->no_strip = true;
               break;
            case 'S':
               config->no_strip_all = true;
               break;
            case 'd':
               config->no_download = true;
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
      config->clean = true;
      
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
               config->clean_all = true;
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
      config->show = true;
      
      // Parse command OPTIONS
      static struct option long_options[] =
		{
			{"build-order",         no_argument, 0, 'o'},
         {"build-actions",       no_argument, 0, 'a'},
         {"download-order",      no_argument, 0, 'd'},
         {"footprint-conflicts", no_argument, 0, 'c'},
         {"help",                no_argument, 0, 'h'},
			{0,                     0,           0,  0 }
		};
      
      option = getopt_long (argc, argv, "", long_options, &option_index);
      while( option != -1 ) 
      {
         switch( option ) 
         {
            case 'o':
               config->build_order = true;
               break;
            case 'a':
               config->build_actions = true;
               break;
            case 'd':
               config->download_order = true;
               break;
            case 'c':
               config->footprint_conflicts = true;
               break;
            case 'h':
               config->help = true;
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
      config->name = argv[optind++];
      cout << "name=" << config->name << endl;
      
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
   cout << "  build                   Build\n";
   cout << "  clean                   Remove package and work files\n";
   cout << "  show                    Show various information\n\n";
   cout << "Download options:\n";
   cout << "  --all                   Download source files of all builds\n\n";
   cout << "Build options:\n";
   cout << "  --keep-work             Do not delete work files\n";
   cout << "  --keep-work-all         Do not delete work files of all builds\n";
   cout << "  --keep-sysroot          Do not delete sysroot\n";
   cout << "  --update-checksum       Update source checksum\n";
   cout << "  --update-checksum-all   Update source checksum of all builds\n";
   cout << "  --update-footprint      Update footprint\n";
   cout << "  --update-footprint-all  Update footprint of all builds\n";
   cout << "  --no-strip              Do not strip libraries or executables\n";
   cout << "  --no-strip-all          Do not strip libraries or executables of all builds\n";
   cout << "  --no-download           Do not download\n\n";
   cout << "Clean options:\n";
   cout << "  --all                   Remove package and work files of all builds\n\n";
   cout << "Show options:\n";
	cout << "  --build-order           Show build order\n";
   cout << "  --build-actions         Show build actions\n";
   cout << "  --download-order        Show download order\n";
   cout << "  --footprint-conflicts   Show footprint file conflicts\n";
   cout << "  --help                  Show buildfiles help\n\n";
   cout << "Options:\n";
	cout << "  --version               Display version\n";
	cout << "  --help                  Display help\n";
}

void COptions::ShowVersion(void)
{
   cout << "buildgear " << VERSION << "\n";
   cout << "Copyright (C) 2011 Martin Lund\n";
   cout << "\n";
   cout << "License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>.\n";
   cout << "This is free software: you are free to change and redistribute it.\n";
   cout << "There is NO WARRANTY, to the extent permitted by law.\n";
}
