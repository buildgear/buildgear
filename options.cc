#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "buildgear/config.h"
#include "buildgear/options.h"

COptions::COptions()
{
   COptions::download = false;
   COptions::build = false;
   COptions::info = false;
}

void COptions::Parse(int argc, char *argv[])
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
            COptions::ignore_checksum = " --ignore-sha256sum ";
				break;
			case 'u':
            // --update-checksum
            COptions::update_checksum = " --update-sha256sum ";
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
      COptions::download = true;
   else
   if (command == "build")
      COptions::build = true;
   else
   if (command == "info")
      COptions::info = true;
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
      COptions::name = argv[optind++];
      
      // Add default prefix in NAME
      if ((COptions::name.find("target/") != 0) &&
         (COptions::name.find("host/") != 0))
         COptions::name = "target/" + COptions::name;
      
      
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
	cout << "Usage: " << argv[0] << " [COMMAND] [OPTIONS] [BUILDFILE NAME]\n\n";
   cout << "Commands:\n";
   cout << "  build                   Build buildfile\n";
   cout << "  clean                   Remove package, work, and downloaded files\n";
	cout << "  download                Download source files\n";
   cout << "  extract                 Extract source files to work area\n";
   cout << "  show                    Show various information\n\n";
   cout << "Build options:\n";
   cout << "  -s, --skip-download     Do not download remote source files\n";
	cout << "  -c, --ignore-checksum   Do not perform source checksum check\n";
	cout << "  -C, --update-checksum   Update source checksum\n\n";
   cout << "  -k, --keep-work         Do not delete work files\n";
   cout << "  -e, --enforce-footprint Build and enforce footprint match\n";
   cout << "  -u, --update-footprint  Build and update footprint\n";
   cout << "  -n, --no-strip          Do not strip executable libraries or binaries\n";
   cout << "  -d, --up-to-date        Do not build, only check if package is up to date\n";
   cout << "  -f, --force             Build even if build appears to be up to date\n\n";
   cout << "Download options:\n";
   cout << "  -a, --all               Download remote source of all buildfiles\n\n";
   cout << "Show options:\n";
	cout << "  -t, --dependency-tree   Show dependency tree\n";
	cout << "  -o, --build-order       Show build order\n";
	cout << "  -b, --download-order    Show download order\n";
   cout << "  -i, --build-help        Show build help\n\n";
   cout << "General options:\n";
	cout << "  -v, --version           Display version\n";
	cout << "  -h, --help              Display help\n";
}

void COptions::ShowVersion(void)
{
   cout << "buildgear " << VERSION << "\n";
   cout << "Copyright (C) 2011 Martin Lund\n";
//   cout << "\n";
//   cout << "License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>.\n";
//   cout << "This is free software: you are free to change and redistribute it.\n";
//   cout << "There is NO WARRANTY, to the extent permitted by law.\n";
}
