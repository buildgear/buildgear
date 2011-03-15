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
   list<CBuildFile*>::iterator it;
   list<CBuildFile*>::iterator itr;
   list<CBuildFile*>::reverse_iterator rit;
   vector<int> time;
   
   typedef ListDigraph::Node Node;
   typedef ListDigraph::Arc Arc;

   IdMap<ListDigraph,Node> id(graph);
   InDegMap<ListDigraph> inDegree(graph);
   
   // Add nodes to graph
   for (it=build_order->begin(); it!=build_order->end(); it++)
   {
      (*it)->node = graph.addNode();
      time.push_back(0);
   }
   
   // Add dependency arcs to nodes
   for (it=build_order->begin(); it!=build_order->end(); it++)
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
   for (rit=build_order->rbegin(); rit!=build_order->rend(); rit++)
   {  
      ListDigraph::Node node = (*rit)->node;
      if (inDegree[node] > 0)
      {
         int maxdist = 0;
         for (ListDigraph::InArcIt iai(graph, node); iai != INVALID; ++iai)
            maxdist = max(time[graph.id(graph.source(iai))], maxdist);
         time[graph.id(node)] = maxdist + 1;
         (*rit)->depth = maxdist + 1;
      }
   }
}

void CDependency::ShowDependencyCircleEps(string filename)
{

   list<CBuildFile*>::iterator it;
   unsigned int i;
   Palette palette;
   Palette paletteW(true);
   
   typedef dim2::Point<int> Point;

   // Layout attributes
   ListDigraph::NodeMap<string> name(graph);
   ListDigraph::NodeMap<Point> coords(graph);
   ListDigraph::NodeMap<double> sizes(graph);
   ListDigraph::NodeMap<int> colors(graph);
   ListDigraph::NodeMap<int> shapes(graph);
   ListDigraph::ArcMap<int> acolors(graph);
   ListDigraph::ArcMap<int> widths(graph);
   
   // Assign layout attributes for nodes
   double angle = 360 / (double) build_order.size();
   int r = 10 + 2*build_order.size();
   angle = angle * 3.14159265/180;
   
   for (it=build_order.begin(), i=0; it!=build_order.end(); it++, i++)
   {
      ListDigraph::Node node = (*it)->node;
      coords[node] = Point(r*cos(i*angle),r*sin(i*angle));
      sizes[node] = 5;
      colors[node] = ((*it)->type == "host") ? 15 : 17;
      shapes[node] = (build_order.size() != i+1) ? 0 : 2;
      name[node] = (*it)->short_name;
   }

   // Assign layout attributes for arcs
   for (ListDigraph::ArcIt a(graph); a != INVALID; ++a)
   {
      acolors[a]=0;
      widths[a]=8;
   }
   
   // Create .eps files showing the dependency circle
   graphToEps(graph,filename).
      coords(coords).
      nodeTexts(name).
      nodeTextSize(1).
      drawArrows().
      arrowWidth(1).
      arrowLength(1).
      absoluteNodeSizes().
      absoluteArcWidths().
      nodeScale(1).
      nodeSizes(sizes).
      nodeShapes(shapes).
      nodeColors(composeMap(palette,colors)).
      arcColors(composeMap(palette,acolors)).
      arcWidthScale(.02).
      arcWidths(widths).
      title("BuildGear dependency circle").
      copyright("(C) 2011 BuildGear Project").
      run();
      
   cout << endl << "Saved dependency circle to " << filename << endl;
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
