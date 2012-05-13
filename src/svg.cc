#include "config.h"
#include <iostream>
#include <iomanip>
#include <time.h>
#include "buildgear/config.h"
#include "buildgear/svg.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void CSvg::open(string filename)
{
   file = fopen(filename.c_str(), "w");
}

void CSvg::close(void)
{
   fclose(file);
}

void CSvg::addHeader(float distance)
{
   fprintf (file, 
   "<svg version='1.1' xmlns='http://www.w3.org/2000/svg'>\n"
   "<!-- Created with Build Gear (http://www.buildgear.org/) -->\n"
   "<defs>\n"
   "  <marker id='endArrow' viewBox='0 0 10 10' refX='80' refY='5' markerUnits='strokeWidth' orient='auto' markerWidth='5' markerHeight='4'>\n"
   "     <polyline points='0,0 10,5 0,10 1,5' fill='black' />\n"
   "  </marker>\n"
   "  <marker id='endArrow2' viewBox='0 0 10 10' refX='7' refY='5' markerUnits='strokeWidth' orient='auto' markerWidth='5' markerHeight='4'>\n"
   "     <polyline points='0,0 10,5 0,10 1,5' fill='black' />\n"
   "  </marker>\n"
   "</defs>\n"
   "<rect x='0' y='0' width='%f' height='%f' style='fill:white'/>\n"
   "<text x='10' y='10' fill='black' font-family='Verdana' font-size='7' font-weight='bold' text-anchor='left'>Build Gear v%s</text>\n"
   "<g transform='translate(%f,%f)'>\n"
   "   <rect x='0' y='0' width='92' height='6' stroke='black' fill='none' stroke-width='0.3'/>"
   "   <circle cx='8' cy='3' r='2' fill='%s'/>\n"
   "   <text x='11' y='4' fill='black' font-family='Verdana' font-size='3' text-anchor='left'>native build</text>\n"
   "   <circle cx='36' cy='3' r='2' fill='%s'/>\n"
   "   <text x='39' y='4' fill='black' font-family='Verdana' font-size='3' text-anchor='left'>cross build</text>\n"
   "   <line x1='61' y1='3' x2='65' y2='3' stroke='black' stroke-width='0.5' marker-end='url(#endArrow2)'/>\n"
   "   <text x='68' y='4' fill='black' font-family='Verdana' font-size='3' text-anchor='left'>dependency</text>\n"
   "</g>\n"
   "<g transform='translate(%f,%f)'>\n"
   , 2*distance, 2*distance 
   , VERSION
   , distance-46, 2*distance-10
   , SVG_COLOR_NATIVE
   , SVG_COLOR_CROSS
   , distance, distance
   );
}

void CSvg::addArrow(float x1, float y1, float x2, float y2)
{
   fprintf (file, 
   "   <line x1='%f' y1='%f' x2='%f' y2='%f' stroke='black' stroke-width='0.5' marker-end='url(#endArrow)'/>\n"
   , x1, y1, x2, y2
   );
}

void CSvg::addCircle(float x, float y, string name, string color, float stroke_width)
{
   fprintf (file,
   "   <circle cx='%f' cy='%f' r='14' stroke='black' stroke-width='%f' fill='%s'/>\n"
   "   <text x='%f' y='%f' fill='black' font-family='Verdana' font-size='3' text-anchor='middle'>%s</text>\n"
   , x, y, stroke_width, color.c_str()
   , x, y, name.c_str()
   );
}

void CSvg::addFooter(void)
{
   fprintf(file,
   "</g>\n"
   "</svg>\n"
   );
}