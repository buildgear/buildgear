#ifndef CONFIGFILE_H
#define CONFIGFILE_H

#include "buildgear/config.h"
#include "buildgear/filesystem.h"

using namespace std;

class CConfigFile : public CFileSystem
{
   public:
      void Parse(string);
   private:
};

#endif
