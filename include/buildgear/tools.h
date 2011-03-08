#ifndef TOOLS_H
#define TOOLS_H

#include "buildgear/filesystem.h"

using namespace std;

class CTools : public CFileSystem
{
   public:
      void Check(void);
      void RunCheckFile(void);
   private:
};

#endif
