#ifndef BUILDFILE_H
#define BUILDFILE_H

#include <list>

using namespace std;

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
      private:
};

#endif
