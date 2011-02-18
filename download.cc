#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>
#include "buildgear/config.h"
#include "buildgear/download.h"

pthread_mutex_t progress_mutex = PTHREAD_MUTEX_INITIALIZER;

struct File
{
   const char *filename;
   FILE *stream;
};

static size_t file_write(void *buffer, size_t size, size_t nmemb, void *stream)
{
   struct File *out=(struct File *)stream;
   if(out && !out->stream)
   {
      /* open file for writing */
      out->stream=fopen(out->filename, "wb");
      if(!out->stream)
         return -1; /* failure, can't open file to write */
   }
   
   return fwrite(buffer, size, nmemb, out->stream);
}

int file_exist(string filename, unsigned int &filesize )
{   
   struct stat buffer;
   
   if (stat(filename.c_str(), &buffer ))
   {
      filesize = 0;
      return false;
   }
   
   // File exists
   filesize = buffer.st_size;
   return true;
}

static int old_progress = 0;
static int space_counter = 19;

int progress(void *v,
             double dltotal,
             double dlnow,
             double ultotal,
             double ulnow)
{
   pthread_mutex_lock(&progress_mutex);
   
   double percent;
   int i;
   
   if (dltotal != 0)
   {
      percent = dlnow * 100.0 / dltotal;
      //cout << dlnow << " / " << dltotal << "(" << percent << "%)" << endl;
      
      // Progress bar:
      // [====================]100%   (= ~ 5%)
      
      if ( ((((int) percent) % 5) == 0) && ( ((int)percent) != old_progress))
      {
         cout << "=" << flush;
         for (i=space_counter; i != 0; i--)
            cout << " " << flush;
            
         cout << "]"
              << setw(4) 
              << (int) percent 
              << "%" << setw(15) 
              << (unsigned long) dlnow 
              << " bytes" 
              << flush;
         
         for (i=(space_counter+1+4+1+15+6); i != 0; i--)
            cout << "\b" << flush;
         space_counter--;
         old_progress = (int) percent;
      }
   }

   pthread_mutex_unlock(&progress_mutex);
   return 0;
}

void CDownload::URL(string url)
{
   // If filename exists in build/source
      // Do not download
   // else
      // Download to build/source/.temp/<type>/<name>/<filename>
      // Do sha256 checksum check
}
void CDownload::File(string url, string directory)
{
   string filename;
   unsigned int filesize;
   
   // Reset progress bar
   old_progress = 0;
   space_counter = 19;
   
   // Parse filename from url
   size_t pos = url.find_last_of('/');

   if (pos == url.npos )
   {
      cout << "Error: " << url << " is invalid." << endl;
         exit(EXIT_FAILURE);
   }

   filename=url.substr(pos+1);

   // Check that filename is valid
   if (filename.length() == 0)
   {
      cout << "Error: " << url << " is invalid." << endl;
         exit(EXIT_FAILURE);
   }
   
   filename = directory + "/" + filename;
      
   CURL *curl;
   CURLcode res;
   struct File file = 
   {
      filename.c_str(), /* name to store the file as if succesful */
      NULL
   };

   curl_global_init(CURL_GLOBAL_DEFAULT);

   curl = curl_easy_init();
   if(curl) {

      curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  
      /* Define our callback to get called when there's data to be written */
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, file_write);
  
      /* Set a pointer to our struct to pass to the callback */
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &file);
      
      /* Disable builtin progress display */
      curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);

      /* Define our callback for progress indication */
      curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progress);

      /* Set resume option if file already found */
      if (file_exist(filename, filesize))
      {
         // Resume
         cout << filename << " exists with size " << filesize << endl;
         curl_easy_setopt(curl, CURLOPT_RESUME_FROM, filesize);
      }

      /* Switch on full protocol/debug output */
      //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

      cout << "   Progress [";

      res = curl_easy_perform(curl);

      cout << endl;

      /* always cleanup */
      curl_easy_cleanup(curl);

      if(CURLE_OK != res)
      {
         /* we failed */
         cout << "curl told us " << res << endl;
      }
   }

   if(file.stream)
      fclose(file.stream); /* close the local file */

   curl_global_cleanup();
}
