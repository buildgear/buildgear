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

#ifndef SVG_H
#define SVG_H

using namespace std;

class CSvg
{
   public:
      void open(string filename);
      void close(void);
      void addHeader(float distance);
      void addNaked(string content);
      void addArrow(float x1, float y1, float x2, float y2);
      void addCircle(float x, float y, string name, string version, string color, int stroke_dash);
      void addRectangle(string x, string y, string width, string height, string color);
      void addRectangle(float x, float y, float width, float height, string color);
      void addText(string text, string x, string y, string color, string params);
      void addText(string text, float x, float y, string color, string params);
      void addText(string text, string x, string y, string color);
      void addText(string text, float x, float y, string color);
      void addLine(string x1, string y1, string x2, string y2, string style);
      void addLine(float x1, float y1, float x2, float y2, string style);
      void addFooter(void);

   private:
      FILE *file;
};

#endif
