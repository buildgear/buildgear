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

void CDependency::Resolve(string name, list<CBuildFile*> *buildfiles)
{
   bool found = false;
   list<CBuildFile*>::iterator it, itr;
   
   cout << "Resolving dependencies..." << endl;
   
   /* Traverse buildfiles */
   for (it=buildfiles->begin(); 
        it!=buildfiles->end();
        it++)
   {
      // If matching buildfile found
      if (name == (*it)->name)
      {
         // Resolve target dependency
         CDependency::ResolveDep(*it, &target_resolved, 
                                      &target_unresolved, TARGET);
         found = true;
      }
   }

   if (!found)
   {
      cout << "Error: " << name << " is not found" << endl;
      exit(EXIT_FAILURE);
   }

   /* Traverse resolved target buildfiles to resolve host dependencies */
   for (it=target_resolved.begin(); 
        it!=target_resolved.end();
        it++)
   {
      // Resolve host dependency
      CDependency::ResolveDep(*it, &(*it)->host_resolved, 
                                   &(*it)->host_unresolved, HOST);
   }
   
   /* Traverse resolved target buildfiles to create build order */
   for (it=target_resolved.begin(); 
        it!=target_resolved.end();
        it++)
   {
//      build_order.push_back(*it); // FIXME: This sould be correct?
      
      for (itr=(*it)->host_resolved.begin(); 
         itr!=(*it)->host_resolved.end();
         itr++)
      {
         build_order.push_back(*itr);
      }
   }

   /* Create download order list */
   download_order = build_order;
   download_order.sort();
   download_order.unique();
   
   cout << "Done" << endl << endl;
}

void CDependency::ShowResolved(void)
{
   int i=1;

   list<CBuildFile*>::iterator it;

   cout <<  "Target resolved:" << endl;

   for (it=target_resolved.begin(); it!=target_resolved.end(); it++, i++)
   {
      cout << " " << i << ". " << (*it)->name << endl;
   }
   cout << endl;

   i=1;

   cout <<  "Build order:" << endl;

   for (it=build_order.begin(); it!=build_order.end(); it++, i++)
   {
      cout << " " << i << ". " << (*it)->name << endl;
   }
   
   cout << endl;
   
   i=1;

   cout <<  "Download order:" << endl;

   for (it=download_order.begin(); it!=download_order.end(); it++, i++)
   {
      cout << " " << i << ". " << (*it)->name << endl;
   }
   
   cout << endl;
}

void CDependency::ResolveDep(CBuildFile *buildfile,
                             list<CBuildFile*> *resolved,
                             list<CBuildFile*> *unresolved,
                             bool type)
{	
	list<CBuildFile*>::iterator it;
   list<CBuildFile*> *dependency;
	
   debug << buildfile->filename << endl;
   
   unresolved->push_back(buildfile);

   if (type == TARGET)
      dependency = &buildfile->target_dependency;
   else
      dependency = &buildfile->host_dependency;

	for (it=dependency->begin(); 
        it!=dependency->end(); it++)
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
         CDependency::ResolveDep(*it, resolved, unresolved, type);
      }
   }
	
   resolved->push_back(buildfile);
   unresolved->remove(buildfile);
}
