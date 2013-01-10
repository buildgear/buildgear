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

void log_output(CStreamDescriptor *stream)
{
   fd_set rdfs;
   char line_buffer[LINE_MAX];
   int result;

   while (1)
   {
      FD_ZERO(&rdfs);
      FD_SET(stream->fd, &rdfs);

      result = select(stream->fd + 1, &rdfs, NULL, NULL, NULL);

      if (fgets(line_buffer, LINE_MAX, stream->fp) == NULL)
      {
         // DONE notify the build thread
         stream->done_flag = true;
         stream->done_cond.notify_all();
         break;
      }

      if (result > 0 && FD_ISSET(stream->fd, &rdfs))
      {
         if (stream->output_mutex.try_lock())
         {
            if (!stream->log_buffer.empty())
            {
               Log.write(stream->log_buffer.data(), stream->log_buffer.size());
               stream->log_buffer.clear();
            }

            Log.write(line_buffer, strlen(line_buffer));
            BuildManager.BuildOutputTick(stream->buildfile);
            stream->output_mutex.unlock();
         } else
         {
            stream->log_buffer.insert(stream->log_buffer.end(), line_buffer, line_buffer + strlen(line_buffer));
            BuildManager.BuildOutputTick(stream->buildfile);
         }
      }
   }

   // Wait until stream is granted access
   stream->output_mutex.lock();

   if (!stream->log_buffer.empty())
   {
      Log.write(stream->log_buffer.data(), stream->log_buffer.size());
   }

   Log.log_streams_mutex.lock();
   Log.log_streams.remove(stream);

   // If there are more streams give output access to the first
   if (!Log.log_streams.empty())
      Log.log_streams.front()->output_mutex.unlock();

   Log.log_streams_mutex.unlock();

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

CStreamDescriptor* CLog::add_stream(FILE *fp, CBuildFile *buildfile)
{
   CStreamDescriptor *stream = new CStreamDescriptor(fp, buildfile);

   log_streams_mutex.lock();

   // If streams are already added, do not allow ouput
   if (!log_streams.empty())
      stream->output_mutex.lock();
   log_streams.push_back(stream);

   log_streams_mutex.unlock();

   thread log_output_thread(log_output, stream);
   log_output_thread.detach();

   return stream;
}

CStreamDescriptor::CStreamDescriptor(FILE *fp, CBuildFile *buildfile)
{
   this->fp = fp;
   this->fd = fileno(fp);
   this->buildfile = buildfile;
   this->log_buffer.reserve(LOG_BUFFER_SIZE);
   this->done_flag = false;
}
