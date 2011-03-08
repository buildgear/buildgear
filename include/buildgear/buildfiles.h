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
      list<CBuildFile*> buildfiles;
      void ParseAndVerify(list<CBuildFile*> *buildfiles);
      void LoadDependency(list<CBuildFile*> *buildfiles);
      void ShowMeta(list<CBuildFile*> *buildfiles);
      void ShowHelp(void);
      CBuildFile * BuildFile(string name, list<CBuildFile*> *buildfiles);
   private:
};

#endif
