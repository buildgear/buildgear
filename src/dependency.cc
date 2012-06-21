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

void CDependency::ResolveSequentialBuildOrder(string name,
                          list<CBuildFile*> *buildfiles)
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
         CDependency::ResolveDependency(*it, &resolved, 
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
   build_order.insert(build_order.end(), resolved.begin(), resolved.end());
}

void CDependency::ShowDownloadOrder(void)
{
   int i;

   list<CBuildFile*>::iterator it;
   
   cout <<  "\nDownload order:" << endl;

   for (it=download_order.begin(), i=1; it!=download_order.end(); it++, i++)
   {
      cout << "   " << setw(3) << i << ". " << (*it)->name << endl;
   }
}

void CDependency::ShowBuildOrder(void)
{
   int i;
   int max_depth = parallel_build_order.front()->depth;

   list<CBuildFile*>::iterator it;
   
   cout <<  "\nBuild order:" << endl;

   for (it=parallel_build_order.begin(), i=1; it!=parallel_build_order.end(); it++, i++)
   {
      cout << "   [" << max_depth - (*it)->depth + 1 << "] " << std::setw(3) << i << ". " << (*it)->name << endl;
   }
}

bool compare(CBuildFile *first, CBuildFile *second)
{
   if (first->depth > second->depth)
      return true;
   else
      return false;
}

void CDependency::ResolveParallelBuildOrder()
{
/*
   list<CBuildFile*>::iterator it;
   list<CBuildFile*>::iterator itr;
   list<CBuildFile*>::reverse_iterator rit;
   vector<int> time;
   
   typedef ListDigraph::Node Node;
   typedef ListDigraph::Arc Arc;

   IdMap<ListDigraph,Node> id(graph);
   InDegMap<ListDigraph> inDegree(graph);
   
   // Add nodes to graph
   for (it=build_order.begin(); it!=build_order.end(); it++)
   {
      (*it)->node = graph.addNode();
      time.push_back(0);
   }
   
   // Add dependency arcs to nodes
   for (it=build_order.begin(); it!=build_order.end(); it++)
   {
      for (itr=(*it)->dependency.begin(); itr!=(*it)->dependency.end(); itr++)
      {
         Arc arc;
         arc = graph.addArc((*it)->node,(*itr)->node);
      }
   }

   // Chech that nodes constitute a DAG
   if (!dag(graph))
   {
      cout << "Error: The dependency graph is not a DAG!" << endl;
      exit(EXIT_FAILURE);
   }
   
   // Find parallel timeslots
   for (rit=build_order.rbegin(); rit!=build_order.rend(); rit++)
   {  
      ListDigraph::Node node = (*rit)->node;
      if (inDegree[node] > 0)
      {
         int maxdist = 0;
         for (ListDigraph::InArcIt iai(graph, node); iai != INVALID; ++iai)
            maxdist = max(time[graph.id(graph.source(iai))], maxdist);
         time[graph.id(node)] = maxdist + 1;
         
         // Set depth (timeslot)
         (*rit)->depth = maxdist + 1;
      }
   }
*/   
   parallel_build_order = build_order;
 
   // Sort parallel build order by time slots
//   parallel_build_order.sort(compare);
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
   Svg.addHeader(radius+40);

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
			  (*it)->type == "cross" ? SVG_COLOR_CROSS : SVG_COLOR_NATIVE,
			  ((int)build_order.size() != i+1) ? 0.5 : 1.0 );
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
         CDependency::ResolveDependency(*it, resolved, unresolved);
      }
   }
	
   resolved->push_back(buildfile);
   unresolved->remove(buildfile);
}
