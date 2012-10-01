#include "config.h"
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <errno.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "buildgear/config.h"
#include "buildgear/filesystem.h"
#include "buildgear/debug.h"

string CFileSystem::GetWorkingDir(void)
{
   char temp[PATH_MAX];

   if (getcwd(temp, sizeof(temp)) != 0)
      return std::string(temp);

   throw std::runtime_error(strerror(errno));
}

void CFileSystem::FindRoot(string dirname)
{
   string cwd;
   string temp_dirname;

   cwd = GetWorkingDir();
   
   temp_dirname = cwd + "/" + dirname;
   
   while ((cwd != "/") && chdir(temp_dirname.c_str()) != 0)
   {
      if (chdir("..") != 0)
         throw std::runtime_error(strerror(errno));
      
      cwd = GetWorkingDir();
      
      temp_dirname = cwd + "/" + dirname;
      
      Debug << "Backstepping to " << cwd << endl;
   }
   
   if (cwd == "/")
   {
      cerr << "Build Gear root directory (.buildgear) is not found!" << endl;
      exit(EXIT_FAILURE);
   }
   
   CFileSystem::root = cwd;
   Config.root = cwd;
   
   // Change to buildgear root dir and stay there
   if (chdir(CFileSystem::root.c_str()) != 0)
      throw std::runtime_error(strerror(errno));
   
   Debug << "Build root: " << CFileSystem::root << endl;
}

void CFileSystem::FindFile(string dirname, string filename, list<CBuildFile*> *buildfiles) 
{
   DIR           *d;
   struct dirent *dir;
   string cwd;

   d = opendir(dirname.c_str());
   if (d == NULL)
      throw std::runtime_error(strerror(errno));

   if (chdir(dirname.c_str()) != 0)
      throw std::runtime_error(strerror(errno));
   
   while((dir = readdir(d)))
   {
      if(strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
         continue;
      if(dir->d_type == DT_DIR)
      {
         if (chdir(dir->d_name) != 0)
            throw std::runtime_error(strerror(errno));
         CFileSystem::FindFile(".", filename, buildfiles);
         if (chdir("..") != 0)
            throw std::runtime_error(strerror(errno));
      } else 
      {
         if(strcmp(dir->d_name, filename.c_str()) == 0 )
         {
            cwd = GetWorkingDir();
            // IMPROVE: maybe not do full path? (make it all relative)
            CBuildFile *b = new CBuildFile(cwd + "/" + dir->d_name);
            buildfiles->push_back(b);
            continue;
         }
      }
   }
  closedir(d);
}

void CFileSystem::FindFiles(string dirname, string filename, list<CBuildFile*> *buildfiles)
{
   if (CFileSystem::DirExists(dirname))
      CFileSystem::FindFile(dirname,
                            filename, buildfiles);
   
   /* Return to buildgear root */
   if (chdir(CFileSystem::root.c_str()) != 0)
      throw std::runtime_error(strerror(errno));
}

bool CFileSystem::DirExists(string dirname)
{
   struct stat st;

   if (stat(dirname.c_str(), &st) != 0)
      return false;
   else if (!S_ISDIR(st.st_mode))
      return false;
      
   return true;
}

bool CFileSystem::FileExist(string filename)
{
   struct stat st;

   if (stat(filename.c_str(), &st) != 0)
      return false;
   else if (!S_ISREG(st.st_mode))
      return false;
      
   return true;
}

bool CFileSystem::FileExistSize(string filename, unsigned int &filesize )
{
   struct stat buffer;

   if (stat(filename.c_str(), &buffer ))
   {
      filesize = 0;
      return false;
   }

   filesize = buffer.st_size;
   return true;
}

long CFileSystem::Age(string filename)
{
   struct stat st;

   if (stat(filename.c_str(), &st) != 0)
      return -1;
   
   return st.st_mtime;
}

void CFileSystem::CreateDirectory(string dirname)
{
   int status;
   string command = "mkdir -p " + dirname;
   
   status = system(command.c_str());
   
   if (status != 0)
      throw std::runtime_error(strerror(errno));
}

void CFileSystem::Move(string source, string destination)
{
   int status;
   string command = "mv " + source + " " + destination;
   
   status = system(command.c_str());
   
   if (status != 0)
      throw std::runtime_error(strerror(errno));
}

void CFileSystem::Cat(string filename)
{
   int status;
   string command = "cat " + filename;

   status = system(command.c_str());

   if (status != 0)
      throw std::runtime_error(strerror(errno));
}

void CFileSystem::Tail(string filename)
{
   int status;
   string command = "tail -f " + filename;

   status = system(command.c_str());
}

void CFileSystem::CopyFile(string source, string destination)
{
   int status;
   string command = "cp " + source + " " + destination;

   status = system(command.c_str());

   if (status != 0)
      throw std::runtime_error(strerror(errno));
}

void CFileSystem::InitRoot(void)
{
   if (DirExists(ROOT_DIR))
      cout << "Build Gear area already exists." << endl;
   else
   {
      CreateDirectory(ROOT_DIR);
      CreateDirectory(BUILD_FILES_DIR);
      CreateDirectory(BUILD_FILES_NATIVE_DIR);
      CreateDirectory(BUILD_FILES_CROSS_DIR);

      CopyFile(TEMPLATE_CONFIG, BUILD_FILES_CONFIG);
      CopyFile(TEMPLATE_README, BUILD_FILES_README);
      CopyFile(TEMPLATE_LOCAL_CONFIG, LOCAL_CONFIG_FILE);

      cout << "Initialized empty build area in "
           << GetWorkingDir()
           << "." << endl;
   }
}
