void svg_add_header(FILE *file, float radius)
{
	fprintf (file, 
	"<svg version='1.1' xmlns='http://www.w3.org/2000/svg'>\n"
	"<defs>\n"
      	"  <marker id='endArrow' viewBox='0 0 10 10' refX='10' refY='5' markerUnits='strokeWidth' orient='auto' markerWidth='5' markerHeight='4'>\n"
        "  <polyline points='0,0 10,5 0,10 1,5' fill='black' />\n"
        "  </marker>\n"
        "</defs>\n"
	"<g transform='translate(%f,%f)'>\n"
	, radius, radius
	);
}

void svg_add_arrow(FILE *file, float x1, float y1, float x2, float y2)
{
	fprintf (file, 
        "<line x1='%f' y1='%f' x2='%f' y2='%f' stroke='black' stroke-width='1' marker-end='url(#endArrow)'/>\n"
	, x1, y1, x2, y2
	);
}

void svg_add_circle(FILE *file, float x, float y, char *name)
{
	fprintf (file, 
        "<circle cx='%f' cy='%f' r='10' stroke='black' stroke-width='1' fill='green'/>\n"
	"<text x='%f' y='%f' fill='black' font-size='8' text-anchor='middle'>%s</text>\n"
	, x, y
	, x, y, name
	);
}



void svg_add_footer(FILE *file)
{
	fprintf(file,
        "</g>\n"
        "</svg>\n"
	);
}

