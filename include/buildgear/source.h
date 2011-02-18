#ifndef SOURCE_H
#define SOURCE_H

using namespace std;

class CSource
{
   public:
      void Download(list<CBuildFile*> *, CFileSystem *, COptions *);
      void Build(CDependency *, CFileSystem *, COptions *);
   private:
      int remote(string item);
};

#endif
