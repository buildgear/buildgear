#ifndef BUILDFILES_H
#define BUILDFILES_H

#include <list>
#include "buildgear/config.h"
#include "buildgear/buildfile.h"
#include "buildgear/filesystem.h"

using namespace std;

class CBuildFiles
{
   public:
      CBuildFiles();
      list<CBuildFile*> buildfiles;
      void ParseAndVerify(void);
      void LoadCrossDependency(void);
      void LoadDependency(void);
      void AddCrossDependency(void);
      void ShowMeta(void);
      void ShowReadme(void);
      CBuildFile * BuildFile(string name);
      list<CBuildFile*> cross_dependency;
   private:
};

#endif
