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

#include "config.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <curl/curl.h>
#include <curl/easy.h>
#include "buildgear/config.h"
#include "buildgear/filesystem.h"
#include "buildgear/download.h"
#include "buildgear/cursor.h"
#include "buildgear/buildmanager.h"

static unsigned int filesize;

size_t CDownloadItem::CurlFileWrite(void *buffer, size_t size, size_t nmemb, void *data)
{
   CDownloadItem *item = (CDownloadItem *)data;
   if(item && !item->file.stream)
   {
      // open file for writing
      item->file.stream=fopen(item->file.filename, "a");
      if(!item->file.stream)
         return -1; // failure, can't open file to write
   }

   return fwrite(buffer, size, nmemb, item->file.stream);
}

int CDownload::progress(void *obj,
                        double dltotal,
                        double dlnow,
                        double ultotal,
                        double ulnow)
{
   CDownloadItem *item = (CDownloadItem*)obj;
   long response_code;

   curl_easy_getinfo(item->curl, CURLINFO_RESPONSE_CODE, &response_code);

   // Check if we are being redirected
   if ( (response_code >= 300) && (response_code < 400) )
   {
      item->status = "Redirecting...";
      item->parent->update_progress();
      return 0;
   }

   if (dltotal != 0)
   {
      item->downloaded = dlnow;
      item->total = dltotal;

      item->parent->update_progress();
   }

   return 0;
}

void CDownload::update_progress()
{
   list<CDownloadItem*>::iterator it;

   // Make sure screen output is written coherent
   lock();

   // Move cursor to first active element
   Cursor.line_up(Cursor.get_ypos());

   for (it = this->active_downloads.begin(); it != this->active_downloads.end(); it++)
   {
      CDownloadItem *item = *it;
      item->print_progress();
   }

   Cursor.clear_below();

   unlock();
}


CDownloadItem::CDownloadItem(string url, string source_dir, CDownload *Download)
{
   list<CDownloadItem*>::iterator it;
   parent = Download;

   status = "Requesting file..";

   // Parse filename from URL
   size_t pos = url.find_last_of('/');

   if (pos == url.npos )
   {
      cout << "Error: " << url << " is invalid." << endl;
      exit(EXIT_FAILURE);
   }

   filename = url.substr(pos+1);

   // Check if filename already pending downloads
   for (it = parent->pending_downloads.begin();it != parent->pending_downloads.end();it++)
   {
      CDownloadItem *item = *it;

      if (item->filename == filename)
         return;
   }

   // Check that filename is valid
   if (filename.length() == 0)
   {
      cout << "Error: " << url << " is invalid." << endl;
      exit(EXIT_FAILURE);
   }

   this->source_dir = source_dir;
   this->alternative_url = false;

   // Download if file does not exist in source dir
   if (!FileExistSize(source_dir + "/" + filename, filesize))
   {
      this->tries = Config.download_retry;
      if (Config.download_mirror_first != "yes")
      {
         this->url = url;

         if (Config.download_mirror != "")
            this->mirror_url = Config.download_mirror + "/" + filename;
         else
            this->mirror_url = "";

      } else
      {
         if (Config.download_mirror != "")
         {
            string mirror_url;
            mirror_url = Config.download_mirror + "/" + filename;

            // Download from mirror url
            this->url = mirror_url;
            this->mirror_url = url;
         }

      }
      File();
   }
}

void CDownloadItem::File()
{
   /* name to store the file as if succesful */
   string dest;

   dest = source_dir + "/" + filename + ".part";

   file.filename = new char [dest.size()+1];
   strcpy (file.filename, dest.c_str());
   file.stream = NULL;

   // Reset progress bar
   downloaded = -1;

   // Initialize Curl
   curl = curl_easy_init();
   if (curl) 
   {
      curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

      // Define file write callback
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlFileWrite);

      // Define write data callback
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);

      curl_easy_setopt(curl, CURLOPT_PRIVATE, this);

      // Disable curls builtin progress indicator
      curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);

      // Set timeouts
      curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, Config.download_timeout);
      curl_easy_setopt(curl, CURLOPT_FTP_RESPONSE_TIMEOUT, Config.download_timeout);

      // Fail on http error (400+)
      curl_easy_setopt(curl, CURLOPT_FAILONERROR, true);

      // Follow URL redirections
      curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, true);

      // Define progress indication callback
      curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, CDownload::progress);
      curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, this);

      // Set resume option if file already exists
      if (FileExistSize(file.filename, filesize))
      {
         // Resume
         curl_easy_setopt(curl, CURLOPT_RESUME_FROM, filesize);
         status = "Partial download detected. Resuming..";
      }

      // Enable curl debug output
      //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

      parent->pending_downloads.push_back(this);
   }
}

void CDownloadItem::print_progress()
{
   ostringstream line;
   char *url;
   double percent;
   int spaces, elements;
   int i;

   // Beautify download start output
   if (parent->first)
   {
      cout << endl << flush;
      parent->first=false;
   }

   curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &url);
   line << "   " << "Downloading '" << url << "'";

   // Update number of columns in terminal
   Cursor.update_num_cols();

   cout << "\r";
   Cursor.clear_rest_of_line();

   if (line.str().size() > Cursor.no_cols)
   {
      string short_line;

      unsigned int offset;

      offset = line.str().size() - Cursor.no_cols + 4;

      // Avoid out_of_range exception if terminal is shrinked
      if (offset > 0 && offset <= string(url).size())
         short_line = "   Downloading '..." + string(url).substr(offset) + "'";

      line.str("");
      line << short_line;
   }

   cout << endl << line.str();
   Cursor.ypos_add(1);
   line.str("");
   Cursor.clear_rest_of_line();
   cout << endl << flush;
   Cursor.ypos_add(1);

   if (downloaded == -1) {

      cout << "   " << status;

      Cursor.clear_rest_of_line();
      cout << endl << flush;
      Cursor.ypos_add(1);

   } else
   {

      percent = downloaded * 100.0 / total;

      spaces = 20;

      // Progress bar:
      // [====================]100%   (= ~ 5%)

      // Calculate how many bar elements to draw
      elements = ((int) percent) * spaces / 100;

      line << "   Progress [";

      for (i=0; i<elements; i++)
         line << "=" ;
      for (i=(spaces-elements);i != 0; i--)
         line << " " ;

      line << right << "]"
         << setw(4) << (int) percent << setw(2) << left << "%";
      if (downloaded == -1)
         line << setw(9) << 0;
      else
         line << setw(9) << (unsigned long) downloaded;
      line << " / ";
      line << setw(9) << (unsigned long) total << setw(6) << " bytes   ";

      cout << line.str();

      Cursor.clear_rest_of_line();
      cout << endl << flush;
      Cursor.clear_rest_of_line();
      Cursor.ypos_add(1);
   }

}

void CDownload::lock()
{
   pthread_mutex_lock(&cout_mutex);
}

void CDownload::unlock()
{
   pthread_mutex_unlock(&cout_mutex);
}

CDownload::CDownload()
{
   this->first = true;
   this->error = false;
}

bool CDownload::activate_download()
{
   if (pending_downloads.size() > 0)
   {
      CDownloadItem *item = pending_downloads.front();
      pending_downloads.pop_front();

      lock();

      item->print_progress();

      unlock();

      active_downloads.push_back(item);
      curl_multi_add_handle(curlm, item->curl);

      return true;
   }

   return false;
}

