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
      bool DirExists(string dirname);
      bool FileExist(string filename);
      bool FileExistSize(string filename, unsigned int &filesize);
      long Age(string filename);
      void Move(string source, string destination);
      void Cat(string filename);
      void Tail(string filename);
      void CopyFile(string source, string destination);
      void InitRoot(void);
      string root;
   private:
      void FindFile(string dirname, string filename, list<CBuildFile*> *buildfiles);
      string GetWorkingDir(void);
};

#endif
