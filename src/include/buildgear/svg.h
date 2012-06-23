#ifndef SVG_H
#define SVG_H

using namespace std;

class CSvg
{
   public:
      void open(string filename);
      void close(void);
      void addHeader(float distance);
      void addArrow(float x1, float y1, float x2, float y2);
      void addCircle(float x, float y, string name, string color, int stroke_dash);
      void addFooter(void);

   private:
      FILE *file;
};

#endif
