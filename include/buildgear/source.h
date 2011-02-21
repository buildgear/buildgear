#ifndef SOURCE_H
#define SOURCE_H

#include "buildgear/filesystem.h"
#include "buildgear/options.h"

using namespace std;

class CSource : public CFileSystem, COptions
{
   public:
      void Download(list<CBuildFile*> *, string);
      void Build(list<CBuildFile*> *, string);
   private:
      int remote(string item);
};

#endif
