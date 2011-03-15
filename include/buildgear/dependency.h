#ifndef DEPENDENCY_H
#define DEPENDENCY_H

#include "lemon/list_graph.h"
#include "lemon/graph_to_eps.h"
#include "buildgear/buildfiles.h"
#include "buildgear/dependency.h"

using namespace std;
using namespace lemon;

class CDependency
{
	public:
      void ShowDownloadOrder(void);
      void ShowBuildOrder(void);
      void Resolve(string name,
                   list<CBuildFile*> *buildfiles,
                   list<CBuildFile*> *build_order);
      void ResolveDepths(list<CBuildFile*> *);
      void ShowDependencyCircleEps(string filename);
      list<CBuildFile*> resolved;
      list<CBuildFile*> unresolved;
      list<CBuildFile*> build_order;
      list<CBuildFile*> download_order;
   private:
      void ResolveDep(CBuildFile *buildfile,
                   list<CBuildFile*> *resolved,
                   list<CBuildFile*> *unresolved);
      int setDepth(CBuildFile *buildfile, int depth);
      ListDigraph graph;
};

#endif
