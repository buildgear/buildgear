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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "config.h"
#include <curses.h>
#include <term.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include "buildgear/cursor.h"

CCursor::CCursor()
{
   char *term_type;
   char *temp;

   term_type = getenv("TERM");

   if (!term_type) {
      cout << "Error: A terminal type must be specified in TERM environment variable." << endl << flush;
      exit(EXIT_FAILURE);
   }

   //Get the termcap descriptions
   tgetent(NULL, getenv("TERM"));

   //Get number of lines in terminal
   no_lines = tgetnum("li");

   //Get number of cols in terminal
   no_cols = tgetnum("co");

   //Request padding character
   temp = tgetstr("pc", NULL);
   PC = temp ? *temp : 0;

   //Get string for moving cursor #1 lines up
   UP =  tgetstr("UP", NULL);

   //Get string for moving cursor #1 lines down
   DO =  tgetstr("DO", NULL);

   //Get string to clear from cursor to end of line
   ce =  tgetstr("ce", NULL);

   //Get string to make cursor invisible
   vi =  tgetstr("vi", NULL);

   //Get string to make cursor visible
   ve =  tgetstr("ve", NULL);

   //Get string to move cursor to lower left corner
   ll =  tgetstr("ll", NULL);

   //Get string to clear lines below cursor
   cd =  tgetstr("cd", NULL);

   //Relative cursor placement
   xpos = 0;
   ypos = 0;
}

void CCursor::line_down(int num)
{
   char *down;

   if (num == 0)
      return;

   down = tparm(DO, num);
   putp(down);
   ypos += num;
}

void CCursor::line_up(int num)
{
   char *up;

   if (num == 0)
      return;

   up = tparm(UP, num);
   putp(up);
   ypos -= num;
}

void CCursor::clear_rest_of_line()
{
   putp(ce);
}

void CCursor::clear_below()
{
   putp(cd);
}

void CCursor::show()
{
   putp(ve);
}

void CCursor::hide()
{
   putp(vi);
}

void CCursor::out()
{
   line_down(no_lines);
   show();
   fflush(stdout);
}

