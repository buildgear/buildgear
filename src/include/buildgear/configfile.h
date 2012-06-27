#ifndef CONFIGFILE_H
#define CONFIGFILE_H

#include "buildgear/config.h"
#include "buildgear/filesystem.h"
#include "buildgear/utility.h"

using namespace std;

class CConfigFile : public CFileSystem, CUtility
{
   public:
      void Parse(string);
   private:
};

#endif
