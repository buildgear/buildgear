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
#include <string>
#include <cstring>
#include <stdexcept>
#include "buildgear/config.h"
#include "buildgear/log.h"

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
