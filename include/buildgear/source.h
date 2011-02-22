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
      void Download(list<CBuildFile*> *, string);
      void Build(list<CBuildFile*> *, CConfig *);
      void Do(string, CBuildFile*);
   private:
      int remote(string item);
};

#endif
