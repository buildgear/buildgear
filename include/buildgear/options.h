#ifndef OPTIONS_H
#define OPTIONS_H

using namespace std;

class COptions
{
   public:
      void Parse(int argc, char *argv[], CConfig *config);
      void CorrectName(string);
   private:
      void ShowHelp(char *argv[]);
      void ShowVersion(void);
};

#endif
