#include <iostream>
#include <iomanip>
#include <time.h>
#include "buildgear/svg.h"
#include <stdio.h>

void CSvg::open(string filename)
{
	file = fopen(filename.c_str(), "w");
}

void CSvg::close(void)
{
	fclose(file);
}

void CSvg::add_header(float radius)
{
	fprintf (file, 
	"<svg version='1.1' xmlns='http://www.w3.org/2000/svg'>\n"
	"<defs>\n"
      	"  <marker id='endArrow' viewBox='0 0 10 10' refX='10' refY='5' markerUnits='strokeWidth' orient='auto' markerWidth='5' markerHeight='4'>\n"
        "  <polyline points='0,0 10,5 0,10 1,5' fill='black' />\n"
        "  </marker>\n"
        "</defs>\n"
        "<rect x='0' y='0' width='100%%' height='100%%' style='fill:white'/>"
	"<g transform='translate(%f,%f)'>\n"
	, radius, radius
	);
}

void CSvg::add_arrow(float x1, float y1, float x2, float y2)
{
	fprintf (file, 
        "<line x1='%f' y1='%f' x2='%f' y2='%f' stroke='black' stroke-width='1' marker-end='url(#endArrow)'/>\n"
	, x1, y1, x2, y2
	);
}

void CSvg::add_circle(float x, float y, string name, string color)
{
	fprintf (file, 
        "<circle cx='%f' cy='%f' r='10' stroke='black' stroke-width='1' fill='%s'/>\n"
	"<text x='%f' y='%f' fill='black' font-size='3' text-anchor='middle'>%s</text>\n"
	, x, y, color.c_str()
	, x, y, name.c_str()
	);
}

void CSvg::add_footer(void)
{
	fprintf(file,
        "</g>\n"
        "</svg>\n"
	);
}
