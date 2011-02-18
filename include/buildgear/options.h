#ifndef OPTIONS_H
#define OPTIONS_H

using namespace std;

class COptions
{
   public:
      COptions();
      void Parse(int argc, char *argv[]);
      string name;
      bool download;
      bool build;
      bool info;
      string ignore_checksum;
      string update_checksum;
   private:
      void ShowHelp(char *argv[]);
      void ShowVersion(void);
};

#endif
