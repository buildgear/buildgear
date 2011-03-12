#include <string>
#include "buildgear/buildfile.h"

CBuildFile::CBuildFile(string filename)
{
   CBuildFile::filename = filename;
   CBuildFile::build = false;
   CBuildFile::visited = false;
   CBuildFile::depth = 0;
}
