
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
      << progName << " - check for differences between two XML files\n"
      << "\n"
      << "Usage:  " << progName << " [options] <file1> <file2>\n"
      << "\n"
      << "Optional arguments (not case sensitive) are:\n"
      << "   --case true|false   -> Set string comparison to be case sensitive or insensitive.\n"
      << "                          This applies to tags, attribute name, and attribute value.\n"
      << "                          It also is used when matching against filters in the\n"
      << "                          config file. [false]\n"
      << "   --config file[.xml] -> Use XML config file to get settings\n"
      << "   --delim <string>    -> Use characters in <string> as delimiters when breaking up\n"
      << "                          XML attribute value and content into tokens [defaults to a\n"
      << "                          space but commas and tabs are commonly used also \" ,\\t\"]\n"
      << "   --delta d           -> Use d as delta value when comparing numbers [1e-7]\n"
      << "   --ref               -> Reformat the input files and print as 'xmldiff_file1.xml\n"
      << "                          and 'xmldiff_file2.xml\n"
      << "   --side              -> Display file1 and file2 side by side during the comparison\n"
      << "   --total <file>      -> Append total number of differences to this file\n"
      << "   --version           -> Print program version and exit\n"
      << "   --v                 -> Same as --version\n"
      << "   --help              -> output this help\n"
      << "\n"
      << "Config file\n"
      << "===========\n";
   text = "The config file is an XML-compliant file to set flags and filters.  The big use"
      " of the config file is to set filters for XML elements to ignore during the"
      " difference. The tags to ignore are matched by the tag name and the attribute name and value.";
   marginOutput(cout, text);
   std::cout
      << "Example:\n"
      << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
      << "<XmlDiff>\n"
      << "   <Case>false</Case>\n"
      << "   <Delim>|{, \\n</Delim>\n"
      << "   <Delta>1.0e-8</Delta>\n"
      << "   <Ignore>\n"
      << "      <FilterText name=\"test\" type=\"double\"/>\n"
      << "      <VarTag name = \"MyVar\"/>\n"
      << "   </Ignore>\n"
      << "</XmlDiff>\n"
      << "\n";

   text = "The above example sets the values for the --case, --delim, and --delta flags and two element"
      " filters that are part of the 'ignore' list. Case-sensitivity in string comparisons is turned"
      " off. Token delimiters are set to vertical bar, left brace, comma, space, and newline. The delta"
      " value for comparing two numbers as the same is 1.0e-8, and two filters are added to the list of"
      " tags to ignore.\n";
   marginOutput(cout, text);
   text = "Any elements in file1 that"
      " match one of these filter elements will not be used for differencing."
      " To match, the element in the file must match <filtertext> with name=test and type="
      " double, or <vartag> with name=myvar.  The element in the file can have additional"
      " attributes but it must have the ones that are in the config file in order for it to"
      " be considered a match. The config file filter can have zero or more attributes.  I"
      " 'case' is set true the match is done case sensitive.";
   marginOutput(cout, text);

   exit(0);
}




void UsageBasic(const char* progName)
{
   std::cout
      << progName << ": Try '" << progName << " --help' for more information."
      << std::endl;
}

