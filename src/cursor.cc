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
#include <stdexcept>
#include <curses.h>
#include <errno.h>
#include <term.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include "buildgear/cursor.h"

void cursor_restore()
{
   Cursor.restore();
   Cursor.enable_wrap();

   // Make sure terminal echo is reenabled
   Cursor.enable_echo();
}

CCursor::CCursor()
{
   char *temp;
   int err;

   // Setup terminfo database based on the TERM environment variable
   if (setupterm(NULL, 1, &err) == ERR)
   {
      cout << "Error: Terminfo setupterm failed (" << err << ")";
      exit(EXIT_FAILURE);
   }

   // Get number of lines in terminal
   no_lines = tgetnum("li");

   // Get number of cols in terminal
   no_cols = tgetnum("co");

   // Request padding character
   temp = tgetstr("pc", NULL);
   PC = temp ? *temp : 0;

   // Get string for moving cursor #1 lines up
   UP =  tgetstr("UP", NULL);

   // Get string for moving cursor #1 lines down
   DO =  tgetstr("DO", NULL);

   // Get string to clear from cursor to end of line
   ce =  tgetstr("ce", NULL);

   // Get string to make cursor invisible
   vi =  tgetstr("vi", NULL);

   // Get string to make cursor visible
   ve =  tgetstr("ve", NULL);

   // Get string to move cursor to lower left corner
   ll =  tgetstr("ll", NULL);

   // Get string to clear lines below cursor
   cd =  tgetstr("cd", NULL);

   // Get string to disable auto margin
   RA = tgetstr("RA", NULL);

   // Get string to enable auto margin
   SA = tgetstr("SA", NULL);

   // Relative cursor placement
   ypos = 0;
}

void CCursor::line_down(int num)
{
   char *down;

   if (num == 0)
      return;

   down = tparm(DO, num);

   putp(down);
   fflush(stdout);

   ypos += num;

   if (ypos > max_ypos)
      max_ypos = ypos;

}

void CCursor::line_up(int num)
{
   char *up;

   if (num == 0)
      return;

   up = tparm(UP, num);

   putp(up);
   fflush(stdout);

   ypos -= num;
}

void CCursor::clear_rest_of_line()
{
   putp(ce);
   fflush(stdout);
}

void CCursor::clear_below()
{
   putp(cd);
   fflush(stdout);
}

void CCursor::show()
{
   putp(ve);
   fflush(stdout);
}

void CCursor::hide()
{
   putp(vi);
   fflush(stdout);
}

void CCursor::restore()
{
   line_down(max_ypos - ypos);
   show();
   fflush(stdout);
}

void CCursor::ypos_add(int num)
{
   ypos += num;

   if (ypos > max_ypos)
      max_ypos = ypos;
}

int CCursor::get_ypos()
{
   return ypos;
}

void CCursor::update_num_cols()
{
   struct winsize w;

   ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

   no_cols = w.ws_col;
}

void CCursor::enable_wrap()
{
   putp(SA);
   fflush(stdout);
}

void CCursor::disable_wrap()
{
   putp(RA);
   fflush(stdout);
}

void CCursor::reset_ymaxpos()
{
   max_ypos = 0;
}

void CCursor::enable_echo()
{
   if (system("stty echo") != 0)
      throw std::runtime_error(strerror(errno));
}

void CCursor::disable_echo()
{
   if (system("stty -echo") != 0)
      throw std::runtime_error(strerror(errno));
}
