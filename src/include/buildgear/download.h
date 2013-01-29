/*
 * This file is part of Build Gear.
 *
 * Copyright (C) 2011-2013  Martin Lund
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include <curl/multi.h>

#include "buildgear/filesystem.h"

#define DOWNLOAD_LINE_SIZE 3

using namespace std;

struct File
{
   char *filename;
   FILE *stream;
};

class CDownloadItem;

class CDownload : public CFileSystem
{
   public:
      void URL(string, string);
      static int progress(void *obj,
                          double dltotal,
                          double dlnow,
                          double ultotal,
                          double ulnow);
      void update_progress();
      CDownload();
      bool activate_download();
      CURLM *curlm;
      list<CDownloadItem*> active_downloads;
      list<CDownloadItem*> pending_downloads;
      void lock();
      void unlock();
      bool first;
      bool error;
   private:
};

class CDownloadItem : public CFileSystem
{
   public:
      CDownloadItem(string, string, CDownload*);
      void File();
      static size_t CurlFileWrite(void *buffer,
                                  size_t size,
                                  size_t nmemb,
                                  void *stream);
      void print_progress();
      struct File file;
      string source_dir;
      string filename;
      string status;
      int tries;
      bool alternative_url;
      string url;
      string mirror_url;
      CURL *curl;
      CDownload *parent;
      double downloaded;
      double total;
};

#endif
