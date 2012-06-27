#include "config.h"
#include <string>
#include "buildgear/config.h"
#include "buildgear/utility.h"

void CUtility::stripChar(string &str, char c)
{
   unsigned int i;

   for (i=0; i<str.length(); i++)
      if (str[i]==c)
      {
         str.erase(i,1);
         i--;
      }
}
