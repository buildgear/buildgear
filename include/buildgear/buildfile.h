#ifndef BUILDFILE_H
#define BUILDFILE_H

#include <list>
#include "lemon/connectivity.h"
#include "lemon/list_graph.h"
#include "lemon/graph_to_eps.h"

using namespace std;
using namespace lemon;

class CBuildFile
{
   public:
      CBuildFile(string filename);
      string filename;
      string name;
      string short_name;
      string version;
      string release;
      string source;
      string depends;
      string build_depends;
      string host_depends;
      string type;
      bool build;
      bool visited;
      int depth;
      list<CBuildFile*> dependency;
      ListDigraph::Node node;
      private:
};

#endif
