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

static pthread_mutex_t progress_mutex = PTHREAD_MUTEX_INITIALIZER;
static unsigned int filesize;
static int old_progress;

struct File
{
   const char *filename;
   FILE *stream;
};

size_t CDownload::CurlFileWrite(void *buffer, size_t size, size_t nmemb, void *stream)
{
   struct File *out=(struct File *)stream;
   if(out && !out->stream)
   {
      // open file for writing
      out->stream=fopen(out->filename, "a");
      if(!out->stream)
         return -1; // failure, can't open file to write
   }
   
   return fwrite(buffer, size, nmemb, out->stream);
}

int CDownload::progress(void *v,
                        double dltotal,
                        double dlnow,
                        double ultotal,
                        double ulnow)
{
   double percent;
   int elements;
   int spaces;
   ostringstream total;
   int i;
   
   total << (unsigned long) dltotal << " bytes";
   
   if (dltotal != 0)
   {
      percent = dlnow * 100.0 / dltotal;
      spaces = 20;
      
      // Progress bar:
      // [====================]100%   (= ~ 5%)
      
      // Calculate how many bar elements to draw
      elements = ((int) percent) / 5;
      
      if (((int)percent) != old_progress)
      {
         for (i=0; i<elements; i++)
            cout << "=" << flush;
         for (i=(spaces-elements); i != 0; i--)
            cout << " " << flush;
         
         cout << right <<"]"
              << setw(4) << (int) percent << "%" 
              << setw(12) << (unsigned long) dlnow
              << " / " 
              << setw(16) << left << total.str()
              << right << flush;
         
         for (i=(spaces+1+4+1+12+3+16); i != 0; i--)
            cout << "\b" << flush;
         old_progress = (int) percent;
      }
   }

   return 0;
}

void CDownload::URL(string url, string source_dir)
{
   string filename;
      
   // Parse filename from URL
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
   
   // Download if file does not exist in source dir
   if (!FileExistSize(source_dir + "/" + filename, filesize))
   {
      int result = CURLE_OPERATION_TIMEDOUT;
      int retry = Config.download_retry;
      string mirror_url = Config.download_mirror + "/" + filename;
      string part_file = source_dir + "/" + filename + ".part";

      if (Config.download_mirror_first != "yes")
      {
         // Download from original url
         cout << endl << "   Downloading '" << url << "'" << endl;
         while ((retry != 0) && (result == CURLE_OPERATION_TIMEDOUT))
         {
            result = File(url, part_file);
            retry--;
         }

         if (result == CURLE_OK)
            goto download_success;

         if (Config.download_mirror != "")
         {
            // Retry download from mirror url
            retry = Config.download_retry;
            result = CURLE_OPERATION_TIMEDOUT;
            cout << endl << "   Downloading '" << mirror_url << "'" << endl;
	    while ((retry != 0) && (result == CURLE_OPERATION_TIMEDOUT))
            {
               // Download from mirror url
               result = File(mirror_url, part_file);
            }
         }
      } else
      {
         if (Config.download_mirror != "")
         {
            // Download from mirror url
            cout << endl << "   Downloading '" << mirror_url << "'" << endl;
            while ((retry != 0) && (result == CURLE_OPERATION_TIMEDOUT))
            {
               result = File(mirror_url, part_file);
               retry--;
            }

            if (result == CURLE_OK)
               goto download_success;
         }

         // Retry download from original url
         retry = Config.download_retry;
         result = CURLE_OPERATION_TIMEDOUT;
         cout << endl << "   Downloading '" << url << "'" << endl;
	 while ((retry != 0) && (result == CURLE_OPERATION_TIMEDOUT))
         {
            // Download from original url
            result = File(url, part_file);
         }
      }

      if (result != CURLE_OK)
         exit(EXIT_FAILURE);

      download_success:
         // Remove .part extension
         Move(source_dir + "/" + filename + ".part",
              source_dir + "/" + filename);
   }
}

int CDownload::File(string url, string filename)
{
   CURL *curl;
   CURLcode result = CURLE_OK;
   struct File file = 
   {
      filename.c_str(), /* name to store the file as if succesful */
      NULL
   };

   // Reset progress bar
   old_progress = -1;

   // Initialize Curl
   curl_global_init(CURL_GLOBAL_DEFAULT);
   curl = curl_easy_init();
   if (curl) 
   {
      curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  
      // Define file write callback
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlFileWrite);
  
      // Define write data callback
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &file);
      
      // Disable curls builtin progress indicator
      curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
      
      // Set timeouts
      curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, Config.download_timeout);
      curl_easy_setopt(curl, CURLOPT_FTP_RESPONSE_TIMEOUT, Config.download_timeout);
      
      // Fail on http error (400+)
      curl_easy_setopt(curl, CURLOPT_FAILONERROR, true);

      // Define progress indication callback
      curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progress);

      // Set resume option if file already exists
      if (FileExistSize(filename, filesize))
      {
         // Resume
         cout << "   Partial download detected - resuming..." << endl;
         curl_easy_setopt(curl, CURLOPT_RESUME_FROM, filesize);
      }

      // Enable curl debug output
      //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

      cout << "   Requesting file..\r" << flush;
      cout << "   Progress [";

      result = curl_easy_perform(curl);

      // Curl cleanup
      curl_easy_cleanup(curl);

      if(result != CURLE_OK)
      {
         // Download failure
         cout << "\r   Error: " << curl_easy_strerror(result) << endl;
      }
      
      cout << endl;
   }

   // close download file
   if(file.stream)
      fclose(file.stream);

   // Curl cleanup
   curl_global_cleanup();
   
   return result;
}
