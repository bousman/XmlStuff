
#include <iostream>
#include <string>
#include <vector>
#include "MU_StringUtil.h"


using namespace std;




void marginOutput(ostream &os, string& text)
{
   list<string> lines = MU_StringUtil::WrapText(text, 80);
   
   for (list<string>::const_iterator s = lines.begin(); s != lines.end(); ++s)
   {
      os << *s << endl;
   }
}




/*****************************************************************************

Function: Usage( )

Display text showing the command line arguments to this program and exit.

*****************************************************************************/

void Usage(const char* progName)
{
   string text;

   std::cout
      << "\n"
      << progName << " - sort an XML file by tag and attribute\n"
      << "\n"
      << "Usage:  " << progName << " [options] <XML file>\n"
      << "\n"
      << "Optional arguments (not case sensitive) are:\n"
      << "   --version           -> Print program version and exit\n"
      << "   -v                  -> Same as --version\n"
      << "   --help              -> output this help\n"
      << "\n";

   exit(0);
}




void UsageBasic(const char* progName)
{
   std::cout
      << progName << ": Try '" << progName << " --help' for more information."
      << std::endl;
}

