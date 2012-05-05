#ifndef SVG_H
#define SVG_H

using namespace std;

class CSvg
{
   public:
      void open(string filename);
      void close(void);
      void add_header(float radius);
      void add_arrow(float x1, float y1, float x2, float y2);
      void add_circle(float x, float y, string name, string color);
      void add_footer(void);

   private:
      FILE *file;
};

#endif
