/*
 * Copyright (C) 2011-2012  Martin Lund
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
#include <stdexcept>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <linux/limits.h>
#include <ncurses.h>
#include "buildgear/config.h"
#include "buildgear/options.h"
#include "buildgear/filesystem.h"
#include "buildgear/buildfiles.h"
#include "buildgear/dependency.h"
#include "buildgear/source.h"
#include "buildgear/download.h"
#include "buildgear/cursor.h"

string bytes2str(double bytes)
{
   int i;
   ostringstream result;
   string unit[] = {"B", "kB", "MB", "GB", "TB"};

   for (i=0;bytes > 1024;i++)
   {
      bytes /= 1024;
   }

   result << setprecision(2) << fixed << bytes << unit[i];

   return result.str();
}

string seconds2str(double seconds)
{
   ostringstream result;
   int min = 0;
   int hr  = 0;
   int day = 0;

   while (seconds > 60)
   {
      seconds -= 60;
      min++;

      if (min == 60)
      {
         min = 0;
         hr++;
      }

      if (hr == 24)
      {
         hr = 0;
         day++;
      }
   }

   if (day > 0)
      result << " " << day << " days";
   if (hr > 0)
      result << " " << hr << " hr";
   if (min > 0)
      result << " " << min << " min";
      result << " " << setprecision(2) << fixed << seconds << " sec";

   return result.str();
}

int CSource::Remote(string item)
{
   int i;
   string protocol[4] = { "http://",
                           "ftp://",
                         "https://",
                          "ftps://"  };
   
   for (i=0; i<4; i++)
   {
      if (item.find(protocol[i]) != item.npos)
         return true;
   }
   
   return false;
}

void CSource::Download(list<CBuildFile*> *buildfiles, string source_dir)
{
   CDownload Download;
   
   list<CBuildFile*>::iterator it;
   string command;
   
   int active_downloads = -1;
   CURLMsg *msg;
   int nMsg;
   int i;

   int maxfd;
   fd_set read, write, exc;
   struct timeval tv;
   long timeout, response;
   double total_time;
   double speed;

   ostringstream temp;

   /* Make sure that source dir exists */
   CreateDirectory(source_dir);

   /* Initialize the multi curl element */
   curl_global_init(CURL_GLOBAL_ALL);
   Download.curlm = curl_multi_init();

   if (Config.download_connections) {
      curl_multi_setopt(Download.curlm, CURLMOPT_MAXCONNECTS, Config.download_connections);
   }

   /* Traverse buildfiles download list */
   for (it=buildfiles->begin(); it!=buildfiles->end(); it++)
   {
      istringstream iss((*it)->source);
      string item;
      
      // For each source item      
      while ( getline(iss, item, ' ') )
      {
         // Download item if it is a remote URL
         if (CSource::Remote(item))
            new CDownloadItem(item, source_dir, &Download);
      }
   }

   // Add download_connections downloads to multi stack
   for (i=0;i<Config.download_connections;i++)
   {
      // Stop if no more downloads are pending
      if (Download.pending_downloads.size() == 0)
         break;

      CDownloadItem *item = Download.pending_downloads.front();
      Download.pending_downloads.pop_front();

      Download.lock();

      item->print_progress();

      Download.unlock();

      Download.active_downloads.push_back(item);
      curl_multi_add_handle(Download.curlm, item->curl);
   }

   while (active_downloads)
   {
      curl_multi_perform(Download.curlm, &active_downloads);
      if (active_downloads) {
         FD_ZERO(&read);
         FD_ZERO(&write);
         FD_ZERO(&exc);

         if (curl_multi_fdset(Download.curlm, &read, &write, &exc, &maxfd))
         {
            cout << "Error: curl_multi_fdset" << endl << flush;
            exit(EXIT_FAILURE);
         }

         if (curl_multi_timeout(Download.curlm, &timeout))
         {
            cout << "Error: curl_multi_timeout" << endl << flush;
            exit(EXIT_FAILURE);
         }

         if (timeout == -1)
            timeout = 100;

         if (maxfd == -1) {
            sleep(timeout / 1000);
         } else {
            tv.tv_sec = timeout / 1000;
            tv.tv_usec = (timeout % 1000) * 1000;

            if (0 > select(maxfd+1, &read, &write, &exc, &tv))
            {
               cout << "Errror: select" << endl << flush;
               exit(EXIT_FAILURE);
            }
         }
      }

      while ((msg = curl_multi_info_read(Download.curlm, &nMsg)))
      {
         if (msg->msg == CURLMSG_DONE)
         {
            CDownloadItem *item;
            curl_easy_getinfo(msg->easy_handle, CURLINFO_PRIVATE, &item);
            curl_easy_getinfo(msg->easy_handle, CURLINFO_RESPONSE_CODE, &response);

            // The easy handle did not return CURLE_OK
            if (msg->data.result != CURLE_OK)
            {
               char *used_url;
               curl_easy_getinfo(msg->easy_handle, CURLINFO_EFFECTIVE_URL, &used_url);

               // The server does not support resume
               if (msg->data.result == CURLE_RANGE_ERROR)
               {
                  curl_multi_remove_handle(Download.curlm, msg->easy_handle);

                  // If the temp file exists we delete it
                  if (FileExist(item->source_dir + "/" + item->filename + ".part"))
                  {
                    string command;
                    command = "rm -f " + item->source_dir + "/"
                              + item->filename + ".part";

                    if (system(command.c_str()) < 0)
                       perror("error");
                  }

                  curl_easy_setopt(item->curl, CURLOPT_RESUME_FROM, 0);

                  // Start transfer again from beginning of file
                  curl_multi_add_handle(Download.curlm, item->curl);

                  // Make sure loop does not end
                  active_downloads++;
                  continue;
               }

               if (item->tries-- > 0)
               {
                  // Restart download by readding the easy handle
                  curl_multi_remove_handle(Download.curlm, msg->easy_handle);
                  curl_multi_add_handle(Download.curlm, msg->easy_handle);

                  // Make sure loop does not end
                  active_downloads++;
                  continue;
               }

               if (!item->alternative_url)
               {
                  if (item->mirror_url == "")
                  {
                     cout << "Error: Could not download " << used_url
                          << "(" << curl_easy_strerror(msg->data.result) << ")" << endl << flush;
                     exit(EXIT_FAILURE);
                  }

                  curl_multi_remove_handle(Download.curlm, msg->easy_handle);
                  temp.str("");
                  temp << "Error (" << curl_easy_strerror(msg->data.result) << ") trying alternative URL..";
                  item->status = temp.str();
                  item->downloaded = -1;

                  Download.lock();

                  Cursor.line_up(Cursor.get_ypos());
                  item->print_progress();

                  Cursor.ypos_add(-DOWNLOAD_LINE_SIZE);

                  Download.unlock();

                  item->alternative_url = true;
                  item->status = "Requesting file (Alternative)..";

                  item->tries = Config.download_retry;

                  curl_easy_setopt(item->curl, CURLOPT_URL, item->mirror_url.c_str());

                  curl_multi_add_handle(Download.curlm, item->curl);

                  // Prevent while loop from stopping prematurely
                  active_downloads++;
                  continue;
               } else
               {
                  curl_easy_getinfo(msg->easy_handle, CURLINFO_EFFECTIVE_URL, &used_url);
                  cout << "Error: Could not download " << used_url << " ("
                       << curl_easy_strerror(msg->data.result) << ")" << endl << flush;
                  exit(EXIT_FAILURE);
               }
            }

            if (item->file.stream)
               fclose(item->file.stream);

            if (FileExist(item->source_dir + "/" + item->filename + ".part"))
            {
               Move(item->source_dir + "/" + item->filename + ".part",
                     item->source_dir + "/" + item->filename);

               // We force update of downloaded/total
               curl_easy_getinfo(item->curl, CURLINFO_SIZE_DOWNLOAD, &item->downloaded);
               curl_easy_getinfo(item->curl, CURLINFO_TOTAL_TIME, &total_time);
               curl_easy_getinfo(item->curl, CURLINFO_SPEED_DOWNLOAD, &speed);

               temp.str("");
               temp << "Download OK (" << bytes2str(item->downloaded) << " in" << seconds2str(total_time)
                    << " at " << bytes2str(speed) << "/s)";
               item->status = temp.str();
               item->downloaded = -1;

               Download.lock();

               // Move cursor to first active element
               Cursor.line_up(Cursor.get_ypos());

               item->print_progress();

               // Remove the download
               Download.active_downloads.remove(item);

               // Dont overwrite the last output
               Cursor.ypos_add(-DOWNLOAD_LINE_SIZE);

               Download.unlock();

            } else {
               cout << "Error: " << item->source_dir + "/" + item->filename + ".part" << " not found" << endl << flush;
               exit(EXIT_FAILURE);
            }

            curl_multi_remove_handle(Download.curlm, msg->easy_handle);
            curl_easy_cleanup(msg->easy_handle);

            // Check if there are more downloads pending
            if (Download.pending_downloads.size() > 0)
            {
               CDownloadItem *item = Download.pending_downloads.front();
               Download.pending_downloads.pop_front();

               // Print out the new download
               Download.lock();

               item->print_progress();

               Download.unlock();

               Download.active_downloads.push_back(item);
               curl_multi_add_handle(Download.curlm, item->curl);

               // Prevent while loop to end prematurely
               active_downloads++;
            }
         } else {
            cout << "Error: CURLMsg (" << msg->msg << ")" << endl << flush;
            exit(EXIT_FAILURE);
         }
      }
   }
   curl_multi_cleanup(Download.curlm);
   curl_global_cleanup();

   // Reset to avoid extra newlines on exit
   Cursor.reset_ymaxpos();

   // Beautify download finish output
   if (!Download.first)
      cout << endl;
}
