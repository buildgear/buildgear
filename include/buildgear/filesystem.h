#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <string>
#include "buildgear/buildfiles.h"

using namespace std;

class CFileSystem
{
   public:
      void FindFiles(string dirname, string filename, CBuildFiles *buildfiles);
      void FindRoot(string dirname);
      void CreateDirectory(string dirname);
      int DirExists(string dirname);
      string root;
   private:
      void FindFile(string dirname, string filename, list<CBuildFile*> *buildfiles);
      string GetWorkingDir(void);
};

#endif
