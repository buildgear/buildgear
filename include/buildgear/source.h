#ifndef SOURCE_H
#define SOURCE_H

#include "buildgear/config.h"
#include "buildgear/filesystem.h"
#include "buildgear/options.h"

using namespace std;

class CSource : public CFileSystem, COptions
{
   public:
      CConfig *config;
      void ShowBuildHelp(void);
      void Download(list<CBuildFile*> *, string);
      void Build(list<CBuildFile*> *, CConfig *);
      void Do(string, CBuildFile*);
      bool UpToDate(CBuildFile *);
      bool DepBuildNeeded(CBuildFile *buildfile);
   private:
      int Remote(string item);
};

#endif
