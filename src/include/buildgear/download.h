#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include "buildgear/filesystem.h"

using namespace std;

class CDownload : public CFileSystem
{
   public:
      int File(string, string);
      void URL(string, string);
      static int progress(void *v,
                          double dltotal,
                          double dlnow,
                          double ultotal,
                          double ulnow);
      static size_t CurlFileWrite(void *buffer,
                                  size_t size,
                                  size_t nmemb,
                                  void *stream);
  private:
};

#endif
