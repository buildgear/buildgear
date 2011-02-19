#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <string>
#include "buildgear/buildfiles.h"

using namespace std;

class CFileSystem
{
   public:
      void FindFiles(string dirname, string filename, list<CBuildFile*> *buildfiles);
      void FindRoot(string dirname);
      void CreateDirectory(string dirname);
       int DirExists(string dirname);
      void Move(string source, string destination);
      string root;
   private:
      void FindFile(string dirname, string filename, list<CBuildFile*> *buildfiles);
      string GetWorkingDir(void);
};

#endif
