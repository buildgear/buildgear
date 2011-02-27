#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "buildgear/config.h"
#include "buildgear/options.h"

void COptions::Parse(int argc, char *argv[], CConfig *config)
{
	int c;
   string command;
   
   /* Print usage help if no arguments provided */
   if (argc == 1)
   {
      COptions::ShowHelp(argv);
      exit(EXIT_SUCCESS);
   }

   /* Save second argument (command) */
   command = argv[1];

   /* Parse options */
	while (1)
	{
		static struct option long_options[] =
		{
			{"ignore-checksum", no_argument, 0, 'i'},
			{"update-checksum", no_argument, 0, 'u'},
			{"build-order",     no_argument, 0, 'b'},
			{"version",         no_argument, 0, 'v'},
			{"help",            no_argument, 0, 'h'},
			{0,                 0,           0,   0}
		};
		
		/* getopt_long stores the option index here. */
		int option_index = 0;

		/* Parse argument using getopt_long */
		c = getopt_long (argc, argv, "", long_options, &option_index);

		/* Detect the end of the options. */
		if (c == -1)
			break;

		switch (c)
		{
			case 0:
				// If this option set a flag, do nothing else now.
				if (long_options[option_index].flag != 0)
					break;
				cout << "option " << long_options[option_index].name;
				if (optarg)
					cout << " with arg " << optarg;
				cout << endl;
				break;

			case 'i':
            // --ignore-checksum
            config->ignore_checksum = " --ignore-sha256sum ";
				break;
			case 'u':
            // --update-checksum
            config->update_checksum = " --update-sha256sum ";
				break;
			
			case 'v':
            // --version
				COptions::ShowVersion();
				exit(EXIT_SUCCESS);
				break;

			case 'h':
            // --help
				COptions::ShowHelp(argv);
				exit(EXIT_SUCCESS);
				break;

			case '?':
				// getopt_long already printed an error message.
				exit(EXIT_FAILURE);
				break;

			default:
				exit(EXIT_FAILURE);
		}
	}

   /* Parse command */
   if (command == "download")
      config->download = true;
   else
   if (command == "build")
      config->build = true;
   else
   if (command == "info")
      config->info = true;
   else
   {
      cout << "Missing command!" << endl;
      exit(EXIT_FAILURE);
   }
   
	/* Handle remaining options (non '--', '-' options) */
	if (optind < argc)
	{
      optind++;
      
      // Get NAME of buildfile
      config->name = argv[optind++];
      
      if (optind < argc)
      {
         // Warn if too many arguments
         cout << argv[0] << ": too many arguments: ";
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
   cout << "  show                    Show various build information\n\n";
   cout << "Download options:\n";
   cout << "  --all                   Download source files of all builds\n\n";
   cout << "Build options:\n";
   cout << "  --no-download           Do not download\n";
   cout << "  --keep-work             Do not delete work files\n";
   cout << "  --keep-work-all         Do not delete work files of all builds\n";
   cout << "  --keep-sysroot          Do not delete sysroot\n";
   cout << "  --update-checksum       Update source checksum\n";
   cout << "  --update-checksum-all   Update source checksum of all builds\n";
   cout << "  --update-footprint      Update footprint\n";
   cout << "  --update-footprint-all  Update footprint of all builds\n";
   cout << "  --no-strip              Do not strip libraries or executables\n";
   cout << "  --no-strip-all          Do not strip libraries or executables of all builds\n\n";
   cout << "Clean options:\n";
   cout << "  --all                   Remove package and work files of all builds\n\n";
   cout << "Show options:\n";
//	cout << "  --dependency-tree       Show dependency tree\n";
	cout << "  --build-order           Show build order\n";
   cout << "  --build-actions         Show build actions\n";
	cout << "  --download-order        Show download order\n";
   cout << "  --footprint-conflicts   Show footprint file conflicts\n";
   cout << "  --help                  Show buildfiles help\n\n";
   cout << "General options:\n";
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
