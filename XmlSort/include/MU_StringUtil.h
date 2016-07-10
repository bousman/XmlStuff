
#pragma once

#ifndef MU_STRING_UTIL_H
#define MU_STRING_UTIL_H


#include <list>
#include <string>
#include <vector>
#include <stddef.h>

using std::list;
using std::vector;
using std::string;


/**
 * @class MU_StringUtil
 * @brief A collection of string utilities.
 *
 * This collection of string utilities are static classes for parsing strings,
 * converting from numbers to strings and vice-versa, doing string compares, etc.
 *
 */
class MU_StringUtil
{
public:

      MU_StringUtil();
     ~MU_StringUtil();

     //! Split string up into tokens based on one or more character delimiters
     /**
      * Splits a string up by tokens such as ' ', ',', '\t' and stores them into
      * an array.  Delimiters at the beginning of the string are skipped over as
      * are two or more delimiters together.
      * Example:  Tokenize(",AB,CD,,EFG",tokens,","); will return an array of
      *           "AB", "CD", and "EFG".
    * @param str the input string of characters to parse
    * @param tokens array of parsed tokens from input string
    * @param delimiters the character(s) to use as delimiter when splitting input string
      */
      static void Tokenize(const std::string& str,
                      std::vector<std::string>& tokens,
                      const std::string& delimiters);

   //! Parse input string into tokens using a delimiter character
   /**
    * Function will fill vector with strings as it parses "aInput".
    * multiple delimiters are treated as one. In other words
    * "A,,B" returns two items "A" and "B".
    *
    * @param aInput the input string of characters to parse
    * @param aOutput array of parsed tokens from aInput
    * @param aDelimiter the character to use as delimiter when splitting aInput
    * @return returns the number of parsed tokens put into aOutput
    */
      static unsigned int Parse(const std::string&        aInput,
                                std::vector<std::string>& aOutput,
                                char                      aDelimiter = ':');
   //! Like Parse() but returns a std::list instead of std::vector
      static unsigned int ParseL(const std::string&       aInput,
                                std::list<std::string>&   aOutput,
                                char                      aDelimiter = ':');
   //! Like Parse() but allow for multiple delimiters to produce an empty field.
   //! In other words "A,,B" returns three items "A", "", and "B".
      static unsigned int Parse_AllowEmpty(const std::string&        aInput,
                                std::vector<std::string>& aOutput,
                                char                      aDelimiter = ':');

      //! Turn string of text into multiple lines by splitting it before a width at a space.
      //! If no space before margin will not split it.
      static list<string> WrapText(const string& input, size_t width);
      //! Turn string of text into multiple lines by splitting it before a width at a space.
      //! If no space before margin will hard-split it at the margin.
      static list<string> WrapTextHard(const string& input, size_t width);

   //! trim leading and/or trailing white space from string
   static void        TrimWhiteSpace(std::string& aString);
      static void        ToLower(std::string& aString);
      static void        ToUpper(std::string& aString);
      //! Break string into lines no longer than aMaxLineLength by replacing ' ' or '\t' with '\n'
      //! unless no spaces found in which case line will be longer
      static void        AddNewLines(std::string& aString, unsigned int aMaxLineLength);
      static size_t      HashFun(const std::string& aString);
      static std::string ToString(double aDouble);
   static std::string ToString(double aDouble, size_t digits);
      static std::string ToString(int aInt);
      static std::string ToString(unsigned long aInt);
      static std::string ToString(size_t aInt);
      static double      ToDouble(const std::string& aString);
   //! Like ToDouble() except it throws a BadConversion exception if aString is not a valid number
   static double      ToDoubleEx(const std::string& aString);
      static int         ToInt(const std::string& aString);


   //! extract the base name part from a filename (the part after the last \).
   //! Assumes backslashes and not slashes in path
      static std::string Basename(const std::string& aFilename);

   //! Extract the head part from a filename (the root file name without the extension)
   //! Only removes extension and not any leading path - use Basename() first
   //! if there is a full directory path attached that should be removed.
   //! Basically this will return everything before the last period in the string
   //! or the entire string if there is no period.
      static std::string Filehead(const std::string& aFilename);

   //! Extract the tail part from a filename (file extension) - whatever characters
   //! are after the last period.  If no period then whole string returned
      static std::string Filetail(const std::string& aFilename);


      //! process string with literal escaped characters (such as \n) and turn those escaped characters
   //! into actual bytes (e.g. \n or \x0b3 become single characters)
   /**
    * Just to be clear, this means a 3-character string of bytes such as 'a','\','n' will get turned in to a
    * 2-character string with 'a' and a newline.
    * This is useful when reading a string of ascii characters from a file.
    * Currently only handles two-character escape such as \n, \t, \', \", \\ but future
    * add-ons will include octal, hex, unicode...  \ooo \xhhh \uxxxx
    */
      static std::string ToEscapedString(const std::string& aString);


   //! return length of null-terminated string (replace strlen in string.h)
      static inline int Strlen(const char *str) { int L = 0; if (str) L = static_cast<int>(strlen(str)); return L; };
   //! duplicate a string (replace strdup in string.h).  Delete with "delete [] str"
      static char* Strdup(const char *str);
   //! Copy a null-terminated string. Checks that both pointers are non-null but
   //! does check that receiving string is long enough to hold the data.
      static char* Strcpy(char *newstr, const char *str);
   //! Copy a null-terminated string but stops at a limit.
   //! Copy stops when null character reached at source string or when nchars characters copied.
      static char* Strncpy(char *newstr, const char *str, int nchars);


      // **********************************************************************
      // String compare functions return true if strings are equal
      //***********************************************************************

      /* compare strings in case-sensitive manner (replace strcmp on WIN32 */
      static inline bool Strcmp(const char *strg1, const char *strg2)
      {
         return strcmp(strg1, strg2) == 0;
      };

      /* compare strings in case-sensitive manner (replace strcmp on WIN32 */
      static inline bool Strcmp(const std::string& strg1, const std::string& strg2)
      {
         return strcmp((strg1.empty() ? "" : strg1.c_str()), (strg2.empty() ? "" : strg2.c_str())) == 0;
      };


      /* compare strings ignoring case (replace strcasecmp and strncasecmp on WIN32 */
      static inline bool Strcasecmp(const char *strg1, const char *strg2)
      {
#ifdef WIN32
         return _stricmp(strg1, strg2) == 0;
#else  // WIN32
         return strcasecmp(strg1, strg2) == 0;
#endif // WIN32
};

      /* compare strings ignoring case (replace strcasecmp and strncasecmp on WIN32 */
      static inline bool Strcasecmp(const std::string& strg1, const char *strg2)
      {
#ifdef WIN32
         return _stricmp((strg1.empty() ? "" : strg1.c_str()), strg2) == 0;
#else  // WIN32
         return strcasecmp((strg1.empty() ? "" : strg1.c_str()), strg2) == 0;
#endif // WIN32
      };

      /* compare strings ignoring case (replace strcasecmp and strncasecmp on WIN32 */
      static inline bool Strcasecmp(const char* strg1, const std::string& strg2)
      {
#ifdef WIN32
         return _stricmp(strg1, (strg2.empty() ? "" : strg2.c_str())) == 0;
#else  // WIN32
         return strcasecmp(strg1, (strg2.empty() ? "" : strg2.c_str())) == 0;
#endif // WIN32
      };

      /* compare strings ignoring case (replace strcasecmp and strncasecmp on WIN32 */
      static inline bool Strcasecmp(const std::string& strg1, const std::string& strg2)
      {
#ifdef WIN32
         return _stricmp((strg1.empty() ? "" : strg1.c_str()), (strg2.empty() ? "" : strg2.c_str())) == 0;
#else  // WIN32
         return strcasecmp((strg1.empty() ? "" : strg1.c_str()), (strg2.empty() ? "" : strg2.c_str())) == 0;
#endif // WIN32
      };

};

#endif
