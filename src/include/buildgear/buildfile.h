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
      string type;
      string build_function;
      string check_function;
      bool build;
      bool visited;
      int depth;
      float x;
      float y;
      list<CBuildFile*> dependency;
      private:
};

#endif
