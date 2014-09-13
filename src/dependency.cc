/*
 * This file is part of Build Gear.
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
#include <iostream>
#include <string>
#include <stdexcept>
#include <list>
#include <algorithm>
#include <vector>
#include <iomanip>
#include <cmath>
#include "buildgear/config.h"
#include "buildgear/filesystem.h"
#include "buildgear/debug.h"
#include "buildgear/buildfiles.h"
#include "buildgear/dependency.h"
#include "buildgear/svg.h"
#include "buildgear/buildmanager.h"

void CDependency::ResolveSequentialBuildOrder(string name,
                          list<CBuildFile*> *buildfiles)
{
   list<CBuildFile*>::iterator it, itr;
   CBuildFile *buildfile = NULL;

   // Clear lists
   unresolved.clear();
   resolved.clear();

   /* Traverse buildfiles */
   for (it=buildfiles->begin();
        it!=buildfiles->end();
        it++)
   {
      // Save matching buildfile
      if (name == (*it)->name)
         buildfile = *it;
   }

   // If no buildfile found then exit
   if (!buildfile)
   {
      cout << "Error: build '" << name << "' is not found" << endl;
      exit(EXIT_FAILURE);
   }

   // Resolve dependency
   CDependency::ResolveDependency(buildfile, &resolved, &unresolved);

   // Check if any of the resolved builds depends on any missing buildfiles
   for (it=resolved.begin();
        it!=resolved.end();
        it++)
   {
      if (!(*it)->missing_depends.empty())
      {
         cout << "Error: " << (*it)->name \
              << " is missing the following dependencies: " \
              << (*it)->missing_depends << endl;
         exit(EXIT_FAILURE);
      }
   }

   /* Add to download order list */
   download_order.insert(download_order.end(), resolved.begin(), resolved.end());
   download_order.sort();
   download_order.unique();

   /* Add to build order list */
   build_order.insert(build_order.end(), resolved.begin(), resolved.end());
}

void CDependency::ShowDownloadOrder(void)
{
   int i;

   list<CBuildFile*>::iterator it;

   cout <<  "\nDownload order:" << endl;

   for (it=download_order.begin(), i=1; it!=download_order.end(); it++, i++)
   {
      cout << "   " << setw(3) << i << ". " << (*it)->name;
      if ((*it)->layer != DEFAULT_LAYER_NAME)
      {
         cout << setw(max_name_length - (*it)->name.size()) << "";
         cout << " [" << (*it)->layer << "]";
      }
      cout << endl;
   }
}

void CDependency::ShowBuildOrder(void)
{
   int i;
   list<CBuildFile*>::iterator it;

   cout <<  "\nBuild order:" << endl;

   for (it=parallel_build_order.begin(), i=0; it!=parallel_build_order.end(); it++, i++)
   {
      cout << "   [" << (*it)->depth << "] " \
           << std::setw(3) << i << ". " << (*it)->name;
      if ((*it)->layer != DEFAULT_LAYER_NAME)
      {
         cout << setw(max_name_length - (*it)->name.size()) << "";
         cout << " [" << (*it)->layer << "]";
      }
      cout << endl;
   }
}

bool compare_depth(CBuildFile *first, CBuildFile *second)
{
   if (first->depth < second->depth)
      return true;
   else
      return false;
}

void CDependency::ResolveParallelBuildOrder()
{
   list<CBuildFile*>::iterator it;
   list<CBuildFile*>::iterator itr;

   for (it=build_order.begin(); it!=build_order.end(); it++)
   {
      (*it)->depth = 0;
      for (itr=(*it)->dependency.begin(); itr!=(*it)->dependency.end(); itr++)
         (*it)->depth = max((*it)->depth, (*itr)->depth + 1);
   }

   parallel_build_order = build_order;

   // Sort parallel build order by depths
   parallel_build_order.sort(compare_depth);
}

void CDependency::ShowDependencyCircleSVG(string filename)
{
   CSvg Svg;
   list<CBuildFile*>::iterator it,itr;
   int i;
   int count;
   float angle;
   float radius;

   // Count number of dependencies
   count = build_order.size();
   angle = 360/(float)count;
   radius = 8*count;

   // Calculate coordinates of build circles
   for (it=build_order.begin(), i=0; it!=build_order.end(); it++, i++)
   {
      (*it)->x = radius * cos(((angle*M_PI)/180)*i);
      (*it)->y = radius * sin(((angle*M_PI)/180)*i);
   }

   Svg.open(filename);
   Svg.addHeader(radius+60);

   // Add arrows
   for (it=build_order.begin(); it!=build_order.end(); it++)
   {
      for (itr=(*it)->dependency.begin(); itr!=(*it)->dependency.end(); itr++)
      {
         Svg.addArrow((*it)->x,(*it)->y,(*itr)->x,(*itr)->y);
      }
   }

   // Add circles
   for (it=build_order.begin(), i=0; it!=build_order.end(); it++, i++)
   {
	   Svg.addCircle((*it)->x, (*it)->y,
	                  (*it)->short_name,
	                  (*it)->version,
			  (*it)->type == "cross" ? SVG_COLOR_CROSS : SVG_COLOR_NATIVE,
			  ((int)build_order.size() != i+1) ? SVG_DASH_NO : SVG_DASH);
   }

   Svg.addFooter();
   Svg.close();

   cout << endl << "Saved dependency graph to " << filename << endl;
}

void CDependency::ResolveDependency(CBuildFile *buildfile,
                             list<CBuildFile*> *resolved,
                             list<CBuildFile*> *unresolved)
{
	list<CBuildFile*>::iterator it;

   Debug << buildfile->filename << endl;

   unresolved->push_back(buildfile);

	for (it=buildfile->dependency.begin();
        it!=buildfile->dependency.end(); it++)
   {
      if(find(resolved->begin(), resolved->end(), *it) == resolved->end())
      {
         if(find(unresolved->begin(), unresolved->end(), *it) != unresolved->end())
         {
            cout << "Error: Circular dependency detected ("
                 << buildfile->name << " <-> "
                 << (*it)->name << ")" << endl;
            exit(EXIT_FAILURE);
         }
         CDependency::ResolveDependency(*it, resolved, unresolved);
      }
   }

   resolved->push_back(buildfile);
   unresolved->remove(buildfile);
}

void CDependency::SetMaxNameLength(void)
{
   list<CBuildFile*>::iterator it;
   int length;

   for (it = parallel_build_order.begin(); it != parallel_build_order.end(); it++)
   {
      length = (*it)->name.size();
      if (length > max_name_length)
         max_name_length = length;
   }
}

void CDependency::SetMaxLayerLength(void)
{
   list<CBuildFile*>::iterator it;
   int length;

   for (it = parallel_build_order.begin(); it != parallel_build_order.end(); it++)
   {
      length = (*it)->layer.size();
      if (length > max_layer_length)
         max_layer_length = length;
   }
}
