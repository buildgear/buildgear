#ifndef DEPENDENCY_H
#define DEPENDENCY_H

#include "buildgear/buildfiles.h"
#include "buildgear/dependency.h"

using namespace std;

class CDependency
{
	public:
      void ShowDownloadOrder(void);
      void ShowBuildOrder(void);
      void Resolve(string name,
                   list<CBuildFile*> *buildfiles,
                   list<CBuildFile*> *build_order);
      void ResolveDepths(list<CBuildFile*> *build_order);
      list<CBuildFile*> resolved;
      list<CBuildFile*> unresolved;
      list<CBuildFile*> build_order;
      list<CBuildFile*> build_toolchain_build_order;
      list<CBuildFile*> host_toolchain_build_order;
      list<CBuildFile*> download_order;
   private:
      int countDependencies(CBuildFile *buildfile);
      void ResolveDep(CBuildFile *buildfile,
                   list<CBuildFile*> *resolved,
                   list<CBuildFile*> *unresolved);
};

#endif
