#include "config.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <list>
#include <stdio.h>
#include <errno.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "buildgear/config.h"
#include "buildgear/filesystem.h"
#include "buildgear/debug.h"
#include "buildgear/buildfile.h"
#include "buildgear/buildfiles.h"

extern CFileSystem FileSystem;

CBuildFiles::CBuildFiles()
{
   host_toolchain = NULL;
}

void stripChar(string &str, char c)
{
   unsigned int i;

   for (i=0; i<str.length(); i++)
      if (str[i]==c)
      {
         str.erase(i,1);
         i--;
      }
}

void CBuildFiles::ParseAndVerify(void)
{   
   list<CBuildFile*>::iterator it;
   
   /* Traverse through all buildfiles */
   for (it=buildfiles.begin();
        it!=buildfiles.end();
        it++)
   {
      FILE *fp;
      char line_buffer[PATH_MAX];
      size_t pos;
      string command =  "bash -c 'source " + 
                        (*it)->filename + 
                        "; echo name=$name \
                         ; echo version=$version \
                         ; echo release=$release \
                         ; echo source=${source[@]} \
                         ; echo depends=${depends[@]}'";

      // Open buildfile for reading
      fp = popen(command.c_str(), "r");
      if (fp == NULL)
         throw std::runtime_error(strerror(errno));

      // Assign name and type based on filename
      pos = (*it)->filename.rfind("host/");
      if (pos != (*it)->filename.npos)
         (*it)->type = "host";
      else
      {
         pos = (*it)->filename.rfind("build/");
         (*it)->type = "build";
      }

      if (pos == (*it)->filename.npos)
      {
         cout << "Error: " << (*it)->filename << " is invalid." << endl;
         exit(EXIT_FAILURE);
      }
      (*it)->name = (*it)->filename.substr(pos);
      pos = (*it)->name.rfind("/Buildfile");
      if (pos == (*it)->filename.npos)
      {
         cout << "Error: " << (*it)->filename << " is invalid." << endl;
         exit(EXIT_FAILURE);
      }
      (*it)->name.erase(pos,10);

      while (fgets(line_buffer, PATH_MAX, fp) != NULL)
      {
         // Parse key=value pairs
         string line(line_buffer);
         string key, value;
         size_t pos = line.find_first_of('=');
	
         key=line.substr(0, pos);
         value=line.substr(pos+1);

         stripChar(value, '\n');

         // Required keys (FIXME: add check for "" values)
         if (key == KEY_NAME)
            (*it)->short_name = value;
         if (key == KEY_VERSION)
            (*it)->version = value;
         if (key == KEY_RELEASE)
            (*it)->release = value;
         
         // Optional keys
         if (key == KEY_SOURCE)
            (*it)->source = value;
         if (key == KEY_DEPENDS)
            (*it)->depends = value;
      }
      pclose(fp);
      
      // If host toolchain defined
      if ((Config.host_toolchain != "") && 
          ((*it)->name == Config.host_toolchain))
      {
         // Save reference to host toolchain buildfile
         CBuildFiles::host_toolchain = (*it);
      }
   }
   
   if ((Config.host_toolchain != "") && 
       (CBuildFiles::host_toolchain == NULL))
   {
      cout << "Error: Host toolchain buildfile is not found."<< endl;
      exit(EXIT_FAILURE);
   }
}

void CBuildFiles::AddHostToolchainDependency(void)
{
   list<CBuildFile*>::iterator it;
   
   /* Traverse through all buildfiles */
   for (it=buildfiles.begin(); 
        it!=buildfiles.end();
        it++)
   {
      if ((*it)->type == "host")
         (*it)->dependency.push_back(host_toolchain);
   }
}


void CBuildFiles::ShowMeta(void)
{   
   list<CBuildFile*>::iterator it;

   debug << endl << "Buildfiles:" << endl << endl;
   
   /* Traverse through all buildfiles */
   for (it=buildfiles.begin(); 
        it!=buildfiles.end();
        it++)
   {
      debug << KEY_NAME << ":    " << (*it)->name << endl;
      debug << KEY_VERSION << ": " << (*it)->version << endl;
      debug << KEY_RELEASE << ": " << (*it)->release << endl;
      debug << KEY_SOURCE << ":  " << (*it)->source << endl;
      debug << KEY_DEPENDS ": " << (*it)->depends << endl << endl;
   }
}

void CBuildFiles::LoadDependency(void)
{
   list<CBuildFile*>::iterator it, itr;
   
   /* Traverse buildfiles */
   for (it=buildfiles.begin();
        it!=buildfiles.end();
        it++)
   {
      int no_match, no_match_exit=false;
      string dep;
      istringstream iss((*it)->depends);
      
      // For each dependency element      
      while ( getline(iss, dep, ' ') )
      {
         // Reset match state
         no_match = true;

         // Find matching buildfile
         for (itr=buildfiles.begin(); 
              itr!=buildfiles.end();
              itr++)
         {
            // If match found make dependency relation
            if (dep == (*itr)->name)
            {
               (*it)->dependency.push_back((*itr));
               no_match = false;
            }
         }
         
         // Warn if missing buildfile
         if (no_match)
         {
            cout << "Error: " << (*it)->name << " " << dep << " not found" << endl;
            no_match_exit = true;
         }
      }
      
      if (no_match_exit)
      {
         cout << "Error: " << (*it)->name << " is missing one or more dependencies" << endl;
         exit(EXIT_FAILURE);
      }
   }
}

CBuildFile * CBuildFiles::BuildFile(string name)
{   
   list<CBuildFile*>::iterator it;
   
   /* Traverse through all buildfiles */
   for (it=buildfiles.begin(); 
        it!=buildfiles.end();
        it++)
      if ((*it)->name == name)
         return (*it);
   
   cout << "Error: build '" << name << " is not found" << endl;
   exit(EXIT_FAILURE);
}

void CBuildFiles::ShowHelp(void)
{
  ifstream fin;
  char s[100000];
  
  string help_file = Config.root + "/" + string(BUILD_FILES_HELP);
  
  if (FileSystem.FileExists(help_file))
  {
      
      fin.open(help_file.c_str(), ios::in);
  
      if(fin.fail())
      {
         cout << "Error: Unable to open " << help_file << endl;
         exit(EXIT_FAILURE);
      }
   
      while(!fin.fail() && !fin.eof())
      {
         fin.getline(s, 100000);
         if (s[0] != '#')
            cout << s << endl;
      }
      fin.close();
   }
}
