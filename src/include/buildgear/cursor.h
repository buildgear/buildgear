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

#ifndef CURSOR_H
#define CURSOR_H

void cursor_restore();

using namespace std;

class CCursor
{
   public:
      CCursor();
      void line_up(int);
      void line_down(int);
      void clear_rest_of_line();
      void clear_below();
      void show();
      void hide();
      void restore();
      int get_ypos();
      void ypos_add(int);
      void update_num_cols();
      void enable_wrap();
      void disable_wrap();
      void reset_ymaxpos();

      unsigned int no_lines;
      unsigned int no_cols;
   private:
      char *UP;
      char PC;
      char *DO;
      char *ce;
      char *vi;
      char *ve;
      char *ll;
      char *cd;
      char *SA;
      char *RA;
      int ypos;
      int max_ypos;
};

extern CCursor Cursor;

#endif
