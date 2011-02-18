#ifndef DEPENDENCY_H
#define DEPENDENCY_H

#include "buildgear/buildfiles.h"
#include "buildgear/dependency.h"

using namespace std;

class CDependency
{
	public:
      void ShowResolved(void);
      void Resolve(string name, list<CBuildFile*> *buildfiles);
      list<CBuildFile*> target_resolved; // Maybe move to CBuildfile
      list<CBuildFile*> target_unresolved;
      list<CBuildFile*> build_order;
      list<CBuildFile*> download_order;
   private:
      void ResolveDep(CBuildFile *buildfile,
                   list<CBuildFile*> *resolved,
                   list<CBuildFile*> *unresolved,
                   bool type);
};

#endif
