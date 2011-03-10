#ifndef BUILDMANAGER_H
#define BUILDMANAGER_H

#include "buildgear/config.h"
#include "buildgear/filesystem.h"
#include "buildgear/options.h"

using namespace std;

class CBuildManager : public CFileSystem, COptions
{
   public:
      void Build(list<CBuildFile*> *);
      void Do(string, CBuildFile*);
      void Clean(CBuildFile *);
      void CleanAll(void);
      void CleanWork(void);
      void CleanLog(void);
      bool UpToDate(CBuildFile *);
      bool DepBuildNeeded(CBuildFile *buildfile);
   private:
};

#endif
