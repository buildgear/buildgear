#ifndef BUILDSYSTEM_H
#define BUILDSYSTEM_H

#include "buildgear/filesystem.h"

using namespace std;

class CBuildSystem : public CFileSystem
{
   public:
      void Check(void);
      void RunCheckFile(void);
   private:
};

#endif
