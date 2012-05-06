#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include "buildgear/filesystem.h"

using namespace std;

class CDownload : public CFileSystem
{
   public:
      int File(string, string);
      void URL(string, string);
   private:
};

#endif
