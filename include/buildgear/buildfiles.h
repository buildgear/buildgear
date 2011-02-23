#ifndef BUILDFILES_H
#define BUILDFILES_H

#include <list>
#include "buildgear/filesystem.h"

using namespace std;

class CBuildFile
{
   public:
      CBuildFile(string filename);
      string filename;
      string name;
      string version;
      string release;
      string source;
      string depends;
      string host_depends;
      string target_depends;
      string type;
      bool build;
      list<CBuildFile*> dependency;
      private:
};

class CBuildFiles
{
   public:
      list<CBuildFile*> buildfiles;
      void ParseAndVerify(list<CBuildFile*> *buildfiles);
      void ShowMeta(list<CBuildFile*> *buildfiles);
      void LoadDependency(list<CBuildFile*> *buildfiles);
   private:
};

#endif
