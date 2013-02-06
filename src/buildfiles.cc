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

#include "config.h"
#include <iostream>
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
}

bool NameCompare(CBuildFile *first, CBuildFile *second)
{
   if (first->name > second->name)
      return false;
   else
      return true;
}

bool NameUnique(CBuildFile *first, CBuildFile *second)
{
   vector<string>::iterator it;
   if (first->name != second->name)
      return false;
   if (first->layer == second->layer)
   {
      cout << "\n\nError: Multiple buildfiles for build '";
      cout << first->name << "' on layer '" << first->layer << "' found." << endl;
      exit(EXIT_FAILURE);
   }
   // If the name is the same, we make sure first points to the lowest priority
   // If funtion returns true, second is removed from list
   for (it = BuildFiles.layers.begin();it != BuildFiles.layers.end(); it++)
   {
      // first points to the highest priority. Return true to remove second
      if (first->layer == (*it))
         return true;

      if (second->layer == (*it))
      {
         // Remove the element explicit since second would be removed be returning true
         BuildFiles.buildfiles.remove(first);
         return false;
      }
   }
   return false;
}

void CBuildFiles::Parse(void)
{
   list<CBuildFile*>::iterator it;

   /* Parse all buildfiles */
   for (it=buildfiles.begin();
        it!=buildfiles.end();
        it++)
       (*it)->Parse();

}

void CBuildFiles::RemoveDuplicates(void)
{
   istringstream iss(Config.bf_config[CONFIG_KEY_LAYERS]);
   string layer;

   /* Resolve layer priority */
   while (getline(iss, layer, ' '))
   {
      BuildFiles.layers.push_back(layer);
   }

   /* Sort by name */
   buildfiles.sort(NameCompare);
   /* Find duplicates and remove according to layers */
   buildfiles.unique(NameUnique);
}

void CBuildFiles::AddCrossDependency(void)
{
   list<CBuildFile*>::iterator it;

   /* Add dependencies for all buildfiles */
   for (it=buildfiles.begin();
        it!=buildfiles.end();
        it++)
   {
      if ((*it)->type == "cross")
         (*it)->dependency.insert((*it)->dependency.end(),
                                  cross_dependency.begin(),
                                  cross_dependency.end());
   }
}

void CBuildFiles::ShowMeta(void)
{
   list<CBuildFile*>::iterator it;

   Debug << endl << "Buildfiles:" << endl << endl;

   /* Traverse through all buildfiles */
   for (it=buildfiles.begin();
        it!=buildfiles.end();
        it++)
   {
      Debug << KEY_NAME << ":    " << (*it)->name << endl;
      Debug << KEY_VERSION << ": " << (*it)->version << endl;
      Debug << KEY_RELEASE << ": " << (*it)->release << endl;
      Debug << KEY_SOURCE << ":  " << (*it)->source << endl;
      Debug << KEY_DEPENDS ": " << (*it)->depends << endl << endl;
   }
}

void CBuildFiles::ShowVersion(CBuildFile *buildfile)
{
   cout << endl << buildfile->name << " " << buildfile->version << "-" << buildfile->release << endl << endl;
}

void CBuildFiles::LoadDependency(void)
{
   list<CBuildFile*>::iterator it, itr;

   /* Traverse buildfiles */
   for (it=buildfiles.begin();
        it!=buildfiles.end();
        it++)
   {
      int no_match;
      string dep;
      istringstream iss((*it)->depends);

      // For each dependency element
      while ( getline(iss, dep, ' ') )
      {
         // Reset match state
         no_match = true;

         // Prepend "cross/" if missing (default for depends)
         if ((dep.find("cross/") != 0) && (dep.find("native/") != 0))
            dep = "cross/" + dep;

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

         // Add missing buildfile to list of missing dependencies
         if (no_match)
            (*it)->missing_depends.append(dep+" ");
      }
   }
}

void CBuildFiles::LoadCrossDependency(void)
{
   list<CBuildFile*>::iterator it;

   string dep;
   istringstream iss(Config.bf_config[CONFIG_KEY_CROSS_DEPENDS]);

   // For each cross dependency element
   while ( getline(iss, dep, ' ') )
   {
      // Prepend "native/" if missing (default for CROSS_DEPENDS)
      if (dep.find("native/") != 0)
         dep = "native/" + dep;

      // Find matching buildfile
      for (it=buildfiles.begin();
              it!=buildfiles.end();
              it++)
      {
         // If match found add to cross dependency list
         if (dep == (*it)->name)
            cross_dependency.push_back((*it));
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

void CBuildFiles::ShowReadme(void)
{
   string readme_file = Config.root + "/" + string(BUILD_FILES_README);
   if (!FileSystem.Cat(readme_file))
   {
      cout << endl << "Error: Could not show " << readme_file << endl;
      exit(EXIT_FAILURE);
   }
}
