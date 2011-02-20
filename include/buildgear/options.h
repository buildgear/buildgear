#ifndef OPTIONS_H
#define OPTIONS_H

using namespace std;

class COptions
{
   public:
      string name;
      bool download;
      bool build;
      bool info;
      string ignore_checksum;
      string update_checksum;
      COptions();
      void Parse(int argc, char *argv[]);
      void CorrectName(string);
   private:
      void ShowHelp(char *argv[]);
      void ShowVersion(void);
};

#endif
