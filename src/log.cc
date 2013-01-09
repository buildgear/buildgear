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
#include <unistd.h>
#include <string>
#include <cstring>
#include <stdexcept>
#include <thread>
#include <limits.h>
#include "buildgear/config.h"
#include "buildgear/log.h"
#include "buildgear/buildmanager.h"

pthread_mutex_t log_output_mutex = PTHREAD_MUTEX_INITIALIZER;

void log_output(void)
{
   fd_set rfds;
   list<CStreamDescriptor*>::iterator it;
   CStreamDescriptor *stream;
   int result, fd, max_fd = 0;
   int count;
   char line_buffer[LINE_MAX];

   /* Log output from build script (buildgear.sh) */
   while (1)
   {
      pthread_mutex_lock(&log_output_mutex);

      // If there are no more log streams we stop the thread
      if (Log.log_streams.size() < 1)
         break;

      // Make the first stream the active output
      Log.log_streams.front()->active = true;

      // Set up FD set of streams to monitor
      FD_ZERO(&rfds);

      for (it = Log.log_streams.begin(); it != Log.log_streams.end(); it++)
      {
         if ((*it)->get_done())
         {
            if (!(*it)->log_buffer.empty() && (*it)->active)
            {
               Log.write((*it)->log_buffer.data(), (*it)->log_buffer.size());
               Log.log_streams.remove((*it));
               Log.log_streams.front()->active = true;
            }
            continue;
         }

         if ((*it)->fd > max_fd)
            max_fd = (*it)->fd;

         FD_SET((*it)->fd, &rfds);
      }

      pthread_mutex_unlock(&log_output_mutex);

      result = select(max_fd + 1, &rfds, NULL, NULL, NULL);

      if (result > 0)
      {
         pthread_mutex_lock(&log_output_mutex);

         // Loop through the streams to find those ready for read
         for (it = Log.log_streams.begin(); it != Log.log_streams.end(); it++)
         {

            if (FD_ISSET((*it)->fd, &rfds))
            {
               // Write contents to log
               count = read((*it)->fd, line_buffer, LINE_MAX);

               // If read failed the stream is done
               if (count < 0)
               {
                  (*it)->set_done(true);
                  continue;
               }

               //NULL terminate the string
               line_buffer[count] = 0;

               BuildManager.BuildOutputTick((*it)->buildfile);

               // Check if we have the active stream
               if ((*it)->active)
               {
                  if (!(*it)->log_buffer.empty())
                  {
                     Log.write((*it)->log_buffer.data(), (*it)->log_buffer.size());
                     (*it)->log_buffer.clear();
                  }

                  Log.write(line_buffer, strlen(line_buffer));

               } else
               {
                  (*it)->log_buffer.insert((*it)->log_buffer.end(), line_buffer, line_buffer + strlen(line_buffer));
               }
            }
         }

         pthread_mutex_unlock(&log_output_mutex);
      }
   }

   Log.running = false;
}

void CLog::open(string filename)
{
   log_file.open(filename);
   if (!log_file.is_open())
      throw std::runtime_error(strerror(errno));
}

void CLog::write(char *buffer, int length)
{
   log_file.write(buffer, length);
   log_file.flush();
}

void CLog::close()
{
   log_file.close();
}

bool CStreamDescriptor::get_done()
{
   return this->done;
}

void CStreamDescriptor::set_done(bool done)
{
   this->done = done;
}

CStreamDescriptor* CLog::add_stream(FILE *fp, CBuildFile *buildfile)
{
   CStreamDescriptor *stream = new CStreamDescriptor(fp);
   stream->buildfile = buildfile;
   stream->set_done(false);

   pthread_mutex_lock(&log_output_mutex);
   log_streams.push_back(stream);

   // Start the log_output thread
   if (!running)
   {
      thread log_output_thread(log_output);
      log_output_thread.detach();
      running = true;
   }

   pthread_mutex_unlock(&log_output_mutex);

   return stream;
}

CStreamDescriptor::CStreamDescriptor(FILE *fp)
{
   this->fp = fp;
   this->fd = fileno(fp);
   this->log_buffer.reserve(LOG_BUFFER_SIZE);
}
