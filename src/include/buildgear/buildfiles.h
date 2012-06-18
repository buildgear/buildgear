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
      void LoadDependency(void);
      void AddCrossToolchainDependency(void);
      void ShowMeta(void);
      void ShowReadme(void);
      CBuildFile * BuildFile(string name);
      CBuildFile * cross_toolchain;
   private:
};

#endif
