#include <iostream>
#include <string>
#include <stdexcept>
#include <list>
#include <algorithm>
#include "buildgear/config.h"
#include "buildgear/filesystem.h"
#include "buildgear/debug.h"
#include "buildgear/buildfiles.h"
#include "buildgear/dependency.h"

void CDependency::Resolve(string name,
                          list<CBuildFile*> *buildfiles,
                          list<CBuildFile*> *build_order)
{
   bool found = false;
   list<CBuildFile*>::iterator it, itr;
   
   // Clear lists
   unresolved.clear();
   resolved.clear();
   
   /* Traverse buildfiles */
   for (it=buildfiles->begin(); 
        it!=buildfiles->end();
        it++)
   {
      // If matching buildfile found
      if (name == (*it)->name)
      {
         // Resolve dependency
         CDependency::ResolveDep(*it, &resolved, 
                                      &unresolved);
         found = true;
      }
   }

   if (!found)
   {
      cout << "Error: build '" << name << "' is not found" << endl;
      exit(EXIT_FAILURE);
   }   

   /* Add to download order list */
   download_order.insert(download_order.end(), resolved.begin(), resolved.end());
   download_order.sort();
   download_order.unique();
   
   /* Add to build order list */
   build_order->insert(build_order->end(), resolved.begin(), resolved.end());
}

void CDependency::ShowDownloadOrder(void)
{
   int i;

   list<CBuildFile*>::iterator it;
   
   cout <<  "\nDownload order:" << endl;

   for (it=download_order.begin(), i=1; it!=download_order.end(); it++, i++)
   {
      cout << "   " << i << ". " << (*it)->name << endl;
   }
}

void CDependency::ShowBuildOrder(void)
{
   int i;

   list<CBuildFile*>::iterator it;
   
   cout <<  "\nBuild order:" << endl;

   for (it=build_order.begin(), i=1; it!=build_order.end(); it++, i++)
   {
      cout << "   [" << (*it)->depth << "] " << i << ". " << (*it)->name << endl;
   }
}

void CDependency::ResolveDepths(list<CBuildFile*> *build_order)
{
   unsigned int* depth = NULL; 
   unsigned int size,i,j;
   unsigned int dependencies;
   list<CBuildFile*>::iterator it;
   
   size = build_order->size();
   depth = new unsigned int[size];
   
   // Calculate dependency depth of builds
   for (it=build_order->begin(),i=0; it!=build_order->end(); it++, i++)
   {
      depth[i] = 0;
      dependencies = (*it)->dependency.size();
      
      for (j=0; j<dependencies; j++)
         depth[i]=max(depth[i], depth[j] + 1);
      
      (*it)->depth = depth[i];
   }
}

void CDependency::ResolveDep(CBuildFile *buildfile,
                             list<CBuildFile*> *resolved,
                             list<CBuildFile*> *unresolved)
{	
	list<CBuildFile*>::iterator it;
	
   debug << buildfile->filename << endl;
   
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
         CDependency::ResolveDep(*it, resolved, unresolved);
      }
   }
	
   resolved->push_back(buildfile);
   unresolved->remove(buildfile);
}
