
#include "MU_StringUtil.h"
#include <ctype.h>
#include <algorithm>
#include <strstream>

using std::istrstream;
using std::ostrstream;
using std::ends;
using namespace std;

MU_StringUtil::MU_StringUtil()
{
}

MU_StringUtil::~MU_StringUtil()
{
}


// static
void MU_StringUtil::Tokenize(const string& str,
                      vector<string>& tokens,
                      const string& delimiters = " ")
{
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}


// static
unsigned int MU_StringUtil::Parse(const string&       aInput,
                                 vector<string>&     aOutput,
                                 char                aDelimiter)
{
   vector<string>::size_type numParms   = 0;
   vector<string>::size_type outputSize = aOutput.size();
   string::size_type         length     = aInput.size();
   string::size_type         parmStart  = 0;

   // Go through input string and look for the delimiter
   if(length)
   {
      for(string::size_type j = 0 ; j <= length ; ++j)
      {
         if(aInput[j] == aDelimiter || j == length)
         {
            // Found something to parse
            string newWord = string(aInput, parmStart, j - parmStart);
            MU_StringUtil::TrimWhiteSpace( newWord );
            if ( !newWord.empty() )
            {
            if(numParms < outputSize)
            {
                  //aOutput[numParms].assign(aInput, parmStart, j - parmStart);
                  aOutput[numParms] = newWord;
            }
            else
            {
                  //aOutput.push_back(string(aInput, parmStart, j - parmStart));
                  aOutput.push_back(newWord);
            }
            ++numParms;
            }
            parmStart = j + 1;
         }
      }
   }
   return static_cast<unsigned int>(numParms);
}


// static
unsigned int MU_StringUtil::ParseL(const string&       aInput,
                                 list<string>&       aOutput,
                                 char                aDelimiter)
{
   list<string>::size_type   numParms   = 0;
   //list<string>::size_type   outputSize = aOutput.size();
   string::size_type         length     = aInput.size();
   string::size_type         parmStart  = 0;

   aOutput.clear();

   // Go through input string and look for the delimiter
   if(length)
   {
      for(string::size_type j = 0 ; j <= length ; ++j)
      {
         if(aInput[j] == aDelimiter || j == length)
         {
            // Found something to parse
            aOutput.push_back(string(aInput, parmStart, j - parmStart));

            ++numParms;
            parmStart = j + 1;
         }
      }
   }
   return static_cast<unsigned int>(numParms);
}

// static
unsigned int MU_StringUtil::Parse_AllowEmpty(const string&       aInput,
                                 vector<string>&     aOutput,
                                 char                aDelimiter)
{
   vector<string>::size_type numParms   = 0;
   vector<string>::size_type outputSize = aOutput.size();
   string::size_type         length     = aInput.size();
   string::size_type         parmStart  = 0;

   // Go through input string and look for the delimiter
   if(length)
   {
      for(string::size_type j = 0 ; j <= length ; ++j)
      {
         if(aInput[j] == aDelimiter || j == length)
         {
            // Found something to parse
            if(numParms < outputSize)
            {
               aOutput[numParms].assign(aInput, parmStart, j - parmStart);
            }
            else
            {
               aOutput.push_back(string(aInput, parmStart, j - parmStart));
            }
            ++numParms;
            parmStart = j + 1;
         }
      }
   }
   return static_cast<unsigned int>(numParms);
}


list<string> MU_StringUtil::WrapText(const string& input, size_t width)
{
   size_t curpos = 0;
   size_t nextpos = 0;

   list<string> lines;
   string substr = input.substr(curpos, width + 1);

   while (substr.length() == width + 1 && (nextpos = substr.rfind(' ')) != input.npos) {
      lines.push_back(input.substr(curpos, nextpos));
      curpos += nextpos + 1;
      substr = input.substr(curpos, width + 1);
   }

   if (curpos != input.length())
      lines.push_back(input.substr(curpos, input.npos));

   return lines;
}


list<string> MU_StringUtil::WrapTextHard(const string& input, size_t width)
{
   size_t curpos = 0;
   size_t nextpos = 0;

   list<string> lines;
   string substr = input.substr(curpos, width + 1);

   while (substr.length() == width + 1) {
      if ((nextpos = substr.rfind(' ')) == input.npos)
      {
         lines.push_back(input.substr(curpos, width));
         curpos += width;
      }
      else
      {
         lines.push_back(input.substr(curpos, nextpos));
         curpos += nextpos + 1;
      }
      substr = input.substr(curpos, width + 1);
   }

   if (curpos != input.length())
      lines.push_back(input.substr(curpos, input.npos));

   return lines;
}


void MU_StringUtil::TrimWhiteSpace(string& aString)
{
   string::iterator iter(aString.begin());

   while(iter != aString.end())
   {
      if(!isspace(*iter))
      {
         break;
      }
      ++iter;
   }
   
   if(iter != aString.begin())
   {
      // There were some leading white spaces, so trim them.
      aString.erase(aString.begin(), iter);
   }
   
   string::reverse_iterator rev_iter(aString.rbegin());
   while(rev_iter != aString.rend())
   {
      if(!isspace(*rev_iter))
      {
         break;
      }
      ++rev_iter;
   }
   
   if(rev_iter != aString.rbegin())
   {
      // There were some trailing white spaces, so trim them.
      aString.erase(rev_iter.base(), aString.end());
   }
}

// The original code used toupper and tolower in the 'transform' calls, but 
// compilation failed on some systems because toupper and tolower are macros.
// Therefore, we define inline methods to invoke the real routine or macro.

namespace
{
   inline int ToLowerChar(int c) { return tolower(c); }
   inline int ToUpperChar(int c) { return toupper(c); }
}

// static
void MU_StringUtil::ToLower(string& aString)
{
   transform(aString.begin(), aString.end(), aString.begin(), ToLowerChar);
}

// static
void MU_StringUtil::ToUpper(string& aString)
{
   transform(aString.begin(), aString.end(), aString.begin(), ToUpperChar);
}

// static
void MU_StringUtil::AddNewLines(string& aString, unsigned int aMaxLineLength)
{
   string::size_type lastSpace = 0;
   string::size_type maxLine   = static_cast<string::size_type>(aMaxLineLength);
   for(string::size_type j = 0 ; j < aString.size() ; ++j)
   {
      if(lastSpace && j > maxLine)
      {
         maxLine = lastSpace + static_cast<string::size_type>(aMaxLineLength) + 1;
         aString[lastSpace] = '\n';
         lastSpace = 0;
      }
      if(aString[j] == ' ' || aString[j] == '\t')
      {
         lastSpace = j; 
      }
      else if(aString[j] == '\n')
      {
         lastSpace = 0;
         maxLine = j + static_cast<string::size_type>(aMaxLineLength) + 1;
      }
   }
}

// static
size_t MU_StringUtil::HashFun(const string& aString)
{
   size_t result = 0;

   string::const_iterator iter(aString.begin());
   while(iter != aString.end())
   {
      result = result*5 + *iter++;
   }

   return result;
}

std::string MU_StringUtil::ToString(double aDouble)
{
   ostrstream oss;
   oss << aDouble << ends;
   const std::string& str = oss.str();
   oss.freeze(0);
   return str;
}

std::string MU_StringUtil::ToString(double aDouble,size_t digits)
{
   ostrstream oss;
   oss.precision(digits);
   oss << std::fixed << aDouble << ends;
   const std::string& str = oss.str();
   oss.freeze(0);
   return str;
}

std::string MU_StringUtil::ToString(size_t aInt)
{
   ostrstream oss;
   oss << aInt << ends;
   const std::string& str = oss.str();
   oss.freeze(0);
   return str;
}

std::string MU_StringUtil::ToString(int aInt)
{
   ostrstream oss;
   oss << aInt << ends;
   const std::string& str = oss.str();
   oss.freeze(0);
   return str;
}

std::string MU_StringUtil::ToString(unsigned long aInt)
{
   ostrstream oss;
   oss << aInt << ends;
   const std::string& str = oss.str();
   oss.freeze(0);
   return str;
}

class BadConversion : public std::runtime_error {
public:
   BadConversion(const std::string& s)
      : std::runtime_error(s)
   { }
};

double MU_StringUtil::ToDouble(const std::string& aString)
{
   double value = 0.0;
   istrstream iss(aString.c_str());
   iss >> value;
   return value;
}

double MU_StringUtil::ToDoubleEx(const std::string& aString)
{
   double value = 0.0;
   istrstream iss(aString.c_str());
   // iss >> value;
   if (!(iss >> value))
      throw BadConversion("ToDouble(\"" + aString + "\")");
   return value;
}

int MU_StringUtil::ToInt(const std::string& aString)
{
   int value = 0;
   istrstream iss(aString.c_str());
   iss >> value;
   return value;
}

std::string MU_StringUtil::ToEscapedString(const std::string& aString)
{
   const char newline = '\n';
   const char tab = '\t';
   //const char backspace = '\b';
   const char backslash = '\\';
   const char question = '?';
   //const char nullChar = '\0';
   const char squote = '\'';
   const char dquote = '\"';

   bool escaped = false;     // flags current escape condition - true means '\' was found
   string retString = "";
#if (_MSC_VER < 1800)
   for (string::const_iterator c_it = aString.begin(); c_it != aString.end(); ++c_it)
   {
      const char& c = *c_it;
#else
   for (char c : aString)
   {
#endif
      if (escaped)
      {
         switch (c)
         {
         case 'n':
            retString += newline;
            escaped = false;
            break;
         case 't':
            retString += tab;
            escaped = false;
            break;
         case '\'':
            retString += squote;
            escaped = false;
            break;
         case '\"':
            retString += dquote;
            escaped = false;
            break;
         case '\\':
            retString += backslash;
            escaped = false;
            break;
         case '?':
            retString += question;
            escaped = false;
            break;
         default:
            retString += "<error>";
            escaped = false;
            break;
         }
      }
      else
      {
         if (c == backslash)
         {
            escaped = true;
         }
         else
         {
            retString += c;
         }
      }
   }

   return retString;
}

std::string MU_StringUtil::Basename(const std::string& aFilename)
{
   // search backwards from end to find first backslash
   size_t pos = aFilename.find_last_of('\\');

   // if no backslash then return entire file name as there is no path
   if (pos == string::npos)
   {
      return aFilename;
   }

   return aFilename.substr(pos + 1);
}

std::string MU_StringUtil::Filehead(const std::string& aFilename)
{
   // search backwards from end to find first '.'
   size_t pos = aFilename.find_last_of('.');

   // if no period then return entire file name as there is no path
   if (pos == string::npos)
   {
      return aFilename;
   }

   return aFilename.substr(0,pos);
}

/* extract the tail part from a filename (part behind the period) */
/* returns a new character string, NULL if no tail part is found*/
std::string MU_StringUtil::Filetail(const std::string& aFilename)
{
   // search backwards from end to find first '.'
   size_t pos = aFilename.find_last_of('.');

   // if no period then return entire file name as there is no path
   if (pos == string::npos)
   {
      return aFilename;
   }

   return aFilename.substr(pos + 1);
}

char* MU_StringUtil::Strdup(const char *str)
{
   /*
   duplicate a string
   replaces the <string.h> Strdup() function
   uses new() for allocation  so  delete() is valid for the pointer.
   handles NULL string pointer without comment
   */
   int  L = 0;
   char *dup = NULL;

   if (str)
   {
      L = MU_StringUtil::Strlen(str) + 1;
      dup = new char[L];
      if (dup)
      {
         MU_StringUtil::Strcpy(dup, str);
      }
   }
   return dup;
}

char* MU_StringUtil::Strcpy(char *newstr, const char *str)
{
   /*
   copy a string
   replaces the <string.h> strncpy() function
   handles NULL pointer in from or to string without comment
   */
   char *retstr = newstr;
   if (newstr)
   {
      if (str)
      {
         while (*str)
         {
            *newstr++ = *str++;
         }
      }
      *newstr = '\0';
   }
   return retstr;
}

char* MU_StringUtil::Strncpy(char *newstr, const char *str, int nchars)
{
   /*
   copy a string
   replaces the <string.h> strncpy() function
   handles NULL pointer in from or to string without comment
   */
   char *retstr = newstr;
   if (newstr)
   {
      if (str)
      {
         while (*str && (nchars-- > 0))
         {
            *newstr++ = *str++;
         }
      }
      *newstr = '\0';
   }
   return retstr;
}
