
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <ostream>
#include <algorithm>
#include <string>

#include "tinyxml2.h"
#include "MU_StringUtil.h"

#include "MyGetOpt.h"
#include "ProgramVersion.h"
#include "Usage.h"
#include "XmlFilter.h"

using namespace tinyxml2;
using namespace std;


/**
 * @file XmlDiff.cpp
 *
 * This program is used to compare two XML files and report the differences.
 * One of the main features is that it will attempt to convert attribute values
 * and XML content text to numbers, and consider them equal if they are within
 * a small delta value of each other. This way, program output in two files
 * containing numeric values can be considered equal if they are close enough.
 *
 * The program can take in settings from a config file or command-line.
 * Command-line switches override those in a config file.
 *
 * @author Brian Bousman
 * @version 0.5.0
 */



// use delta when comparing two numbers to determine if they are equal.  if (a-b)<gNumberDelta then same
// this value can be changed by command-line switch or config file
static double gNumberDelta = 1.0e-07;
// determines if strings are matched with case sensitivity or not
// this value can be changed by command-line switch or config file
static bool gCaseSensitive = false;
// delimiter characters used to break text into tokens (attribute value and xml element content)
static string gDelimiters = " ";

//! config file sets filters for XML elements to ignore, based on tag name and attributes
static list<XmlFilter> gXmlFilters;

//! keep track of XML tree as we work down and across. it will contain the element tag name
//! unless the element has 'name="value"' as an attribute.  If so, the "value" will be used.
static list<string> gModelTree;



/**
 * @class XmlDifferences
 * @brief contains count on the number of differences between the two files
 */
class XmlDifferences
{
public:
   XmlDifferences() :
      totalElemCompared(0),
      totalDifferentTypeElem(0), extraElemFile1(0), extraElemFile2(0), elemWithAttribNameDiff(0), elemWithAttribValueDiff(0),
      elemWithTextDiff(0), totalTagNumbContentDiff(0), totalTagTextContentDiff(0)
   {}
   ~XmlDifferences() {}

   unsigned int Total() const                // return total number of differences
   {
      return totalDifferentTypeElem + extraElemFile1 + extraElemFile2 + elemWithAttribNameDiff + elemWithAttribValueDiff + elemWithTextDiff;
   }

   unsigned int totalElemCompared;           // total number of elements compared
   unsigned int totalDifferentTypeElem;      // encountered tags of different type (e.g. <var> vs <group>)
   unsigned int extraElemFile1;              // number of extra elements in file 1 after file 2 hits eof
   unsigned int extraElemFile2;              // number of extra elements in file 2 after file 1 hits eof
   unsigned int elemWithAttribNameDiff;      // number of tags with one or more attribute name differences
   unsigned int elemWithAttribValueDiff;     // number of tags with one or more attribute value differences
   unsigned int elemWithTextDiff;            // number of tags with inner text differs
   unsigned int totalTagNumbContentDiff;     // number of number token differences inside a tag
   unsigned int totalTagTextContentDiff;     // number of text token differences inside a tag

private:
   friend std::ostream& operator<<(std::ostream &os, const XmlDifferences& p);
};
std::ostream& operator<<(std::ostream &os, const XmlDifferences& p)
{
   os << "Total number of elements compared:        " << p.totalElemCompared << endl;
   if (p.extraElemFile1)
      os << "Extra elements in file 1: " << p.extraElemFile1 << endl;
   if (p.extraElemFile2)
      os << "Extra elements in file 2: " << p.extraElemFile2 << endl;
   os << "Number of tags with name differences:     " << p.totalDifferentTypeElem << endl;
   os << "# of tags with an attribute name diff:    " << p.elemWithAttribNameDiff << endl;
   os << "# of tags with an attribute value diff:   " << p.elemWithAttribValueDiff << endl;
   os << "Total # of tags with content differences: " << p.elemWithTextDiff << endl;
   os << "Total # of content diffs [as numbers]:    " << p.totalTagNumbContentDiff << endl;
   os << "Total # of content diffs [as text]:       " << p.totalTagTextContentDiff << endl;
   os << "Total differences:                        " << p.Total() << endl;
   return os;
}




//! function determines if the two strings match.  will use flags to see if case sensitivity is used
inline bool matchString(const char* a, const char* b)
{
   return (gCaseSensitive ? strcmp(a,b)==0 : strcasecmp(a, b)==0);
}




//! function determines if the two strings match.  will use flags to see if case sensitivity is used
inline bool matchString(const string& a, const string& b)
{
   return (gCaseSensitive ? a == b : MU_StringUtil::Strcasecmp(a, b));
}




/**
 * Push a new element name onto the model tree.  This is done right before
 * processing the child elements.
 * The tag name will be pushed unless attribute 'name="value"' exists
 * in which case "value" will be pushed.
 */
void pushToModelTree(const string& tagName,map<string,string>& attribs)
{
   map<string, string>::const_iterator a_it = attribs.find("name");
   if (a_it == attribs.end())
   {
      gModelTree.push_back(tagName);   // no attribute "name" so use element tag
   }
   else
   {
      gModelTree.push_back(a_it->second);  // push back value of name="value"
   }
}
/**
 * Pop a name off the model tree - used when finished with the child elements
 * and continuing on with the parent.
 * TODO: error checking if pop attempted when map is empty
 */
void popFromModelTree()
{
   gModelTree.pop_back();
}
/**
 * Output the model tree in form of level1.level2.[level3...].
 * This is used when outputting XML elements that show differences.
 */
void outputModelTree(std::ostream &os)
{
   unsigned int counter = 0;
#if (_MSC_VER < 1800)
   for (list<string>::const_iterator g_it=gModelTree.begin(); g_it != gModelTree.end(); ++g_it)
   {
      const string& s = *g_it;
#else
   for (const string s : gModelTree)
   {
#endif
      os << (counter++ == 0 ? "" : ".") << s;
      //os << s << ".";
   }
}




/**
 * Get a list of XML attributes from an XML element.
 *
 * @param elem the XML element to extract attributes from
 * @param attribs returns map with all attribute names and values in 'elem'
 */
void getXmlAttributesFromElem(const XMLElement* elem, map<string,string>& attribs)
{
   attribs.clear();
   const XMLAttribute* attrib = elem->FirstAttribute();
   while (attrib)
   {
      // save off attribute name as all lower case to make comparison easier
      string attribName = attrib->Name();
      MU_StringUtil::ToLower(attribName);
      attribs[attribName] = attrib->Value();

      attrib = attrib->Next();
   }
}




string elementToString(const XMLElement* elem1)
{
   if (elem1)
   {
      string tag = elem1->Value();
      string retStr = "<" + tag;

      map<string, string> attribs;
      getXmlAttributesFromElem(elem1, attribs);
      for (map<string, string>::const_iterator a_it = attribs.begin(); a_it != attribs.end(); ++a_it)
      {
         retStr += " ";
         retStr += a_it->first + "=\"" + a_it->second + "\"";
      }

      const char* text = elem1->GetText();
      if (text)
      {
         retStr += ">";
         retStr += text;
         retStr += "</" + tag + ">";
      }
      else
      {
         retStr += "/>";
      }

      return retStr;
   }

   return "";
}




void outputElementsSideBySide(const XMLElement* elem1, const XMLElement* elem2, size_t margin)
{
   size_t len;
   if (elem1 && elem2)
   {
      //XMLPrinter printer;
      //elem1->Accept(&printer);
      //string testStr = printer.CStr();

      string elem1Str = elementToString(elem1);
      string elem2Str = elementToString(elem2);
      std::replace(elem1Str.begin(), elem1Str.end(), '\n', ' ');  // replace '\n' with printable char
      std::replace(elem2Str.begin(), elem2Str.end(), '\n', ' ');
      list<string> lines1 = MU_StringUtil::WrapTextHard(elem1Str, margin);
      list<string> lines2 = MU_StringUtil::WrapTextHard(elem2Str, margin);
      //cout << "lines1 size=" << lines1.size() << ", lines2 size=" << lines2.size() << endl;

      list<string>::const_iterator l1 = lines1.begin();
      list<string>::const_iterator l2 = lines2.begin();
      while (l1 != lines1.end() || l2 != lines2.end())
      {
         cout << "|";
         size_t underLen = margin;
         if (l1 != lines1.end())
         {
            cout << *l1;
            if ((len = (*l1).length()) <= margin)
               underLen = margin - len;
            else
               underLen = 0;

            ++l1;
         }

         if (underLen > 0)
         {
            string spaces(underLen, ' ');
            cout << spaces;
         }
         cout << "|";

         underLen = margin;
         if (l2 != lines2.end())
         {
            cout << *l2;
            if ((len = (*l2).length()) <= margin)
               underLen = margin - len;
            else
               underLen = 0;

            ++l2;
         }

         if (underLen > 0)
         {
            string spaces(underLen, ' ');
            cout << spaces;
         }
         cout << "|";

         cout << endl;
      }
   }
}




/**
 * Find out if an XML element matches any of the filters in gXmlFilters.
 *
 * @param tagName the tag for the element
 * @param attribs a set of name/value pairs from the XML element
 * @return true if tagname and attributes match one of the filters
 */
bool checkXmlFilter(const string& tagName, const map<string,string>& attribs)
{
#if (_MSC_VER < 1800)
   for (list<XmlFilter>::const_iterator g_it=gXmlFilters.begin(); g_it != gXmlFilters.end(); ++g_it)
   {
      const XmlFilter& f = *g_it;
#else
   for (const XmlFilter& f : gXmlFilters)
   {
#endif
      if (f.match(tagName,attribs))
      {
         return true;
      }
   }
   return false;
}




void showXmlFilters(ostream& os)
{
#if (_MSC_VER < 1800)
   for (list<XmlFilter>::const_iterator g_it=gXmlFilters.begin(); g_it != gXmlFilters.end(); ++g_it)
   {
      const XmlFilter& f = *g_it;
#else
   for (const XmlFilter& f : gXmlFilters)
   {
#endif
      os << f;
   }
}




/**
* Set the case-sensitivity of the filters in gXmlFilters.
*
* @param aCaseSensitive true for case sensitive, false for case-INsensitive
*/
void setXmlFilterCase(bool aCaseSensitive)
{
#if (_MSC_VER < 1800)
   for (list<XmlFilter>::iterator g_it=gXmlFilters.begin(); g_it != gXmlFilters.end(); ++g_it)
   {
      XmlFilter& f = *g_it;
#else
   for (XmlFilter& f : gXmlFilters)
   {
#endif
      f.caseSensitive(aCaseSensitive);
   }
}




/**
 * Output the difference between the two files.
 * Currently this is done by outputing the XML tree, a title, and the two different
 * strings.  These are separated by commas.
 * @param os the stream to write to
 * @param title a title such as "content difference" or "attibute name"
 * @param d1 the string from file 1 that differs from d2
 * @param d2 the string from file 2 that differs from d1
 */
void outputDiff(ostream& os, const string& title, const string& d1, const string& d2)
{
   /*
   os << title << "\n"
      << "< " << d1 << "\n"
      << "---" << "\n"
      << "> " << d2 << endl;
      */
   outputModelTree(os);
   os << "," << title << "," << d1 << "," << d2 << endl;
}
void outputDiffcptr(ostream& os, const string& title, const char* d1, const char* d2)
{
   // turn null pointers into "" 
   outputDiff(os, title, (d1 ? d1 : ""), (d2 ? d2 : ""));
}



/**
 * Count the number of elements starting with this one and any siblings that follow.
 * It does not count any child elements.
 *
 * @param rootElement pointer to the first element to count
 * @return the number of elements counted
 */
unsigned int countElement(XMLElement* rootElement)
{
   unsigned int count = 0;
   XMLElement* element = rootElement;
   while (element)
   {
      ++count;
      element = element->NextSiblingElement();
   }
   return count;
}




/**
 * Count the number of text differences inside tag content.
 *
 * The text that is compared is that within tags as in <Tag>hello world</Tag>.  (text is "hello world").
 * The text is broken up into tokens based on the delimiters passed in.  Each token is compared
 * separately and will be compared as a number if possible.  Numbers are considered equal if they
 * are within some small tolerance of each other.
 * The differences are counted by incrementing within parameter totDiff: totalTagNumbContentDiff and
 * totalTagTextContentDiff are incremented.
 *
 * @param text1 string 1 to use in comparison
 * @param text2 string 2 to use in comparison against string 1
 * @param totDiff the number of differences is incremented in this object
 * @param delim string containing the characters to use as delimiters in text1 and text2 to break them into tokens (e.g. "{,\n ")
 * @param outDiffMsg a message to put in the output if differences are found
 * @param nNumberDiff returns the number of content differences compared as numbers
 * @param nTextDiff returns the number of content differences compared as strings
 * @return true if there are differences
 */
bool countTextAsNumberTokenDiff(const char* text1, const char* text2, const string& delim,
   const string& outDiffMsg, unsigned int& nNumberDiff, unsigned int& nTextDiff)
{
   //cout << "compare |" << text1 << "| with |" << text2 << "|" << endl;

   unsigned int diffCount = 0;
   nNumberDiff = nTextDiff = 0;

   vector<string> tokens1, tokens2;
   MU_StringUtil::Tokenize(text1, tokens1, delim);
   MU_StringUtil::Tokenize(text2, tokens2, delim);

   // get number of tokens in each string of text (using delimters)
   unsigned int n1 = tokens1.size();
   unsigned int n2 = tokens2.size();
   // if different number of delimiters it means there are abs(n1-n2) different tokens
   if (n1 != n2)
   {
      diffCount += abs((int)(n1-n2));
      //cout << "n1=" << n1 << " n2=" << n2 << "  increment diffCount by " << abs((int)(n1 - n2)) << endl;
   }
   unsigned int nmin = n1;   // nmin=min(n1,n2)
   if (n2 < nmin)
      nmin = n2;

   double d1, d2;
   bool isNumber;

   // loop through tokens comparing the two
   for (unsigned int n = 0; n < nmin; ++n)
   {
      string& s1 = tokens1[n];
      string& s2 = tokens2[n];
      bool isDiff = false;

      // try to convert token into a number, if so we can compare it as a number
      isNumber = false;
      try
      {
         d1 = MU_StringUtil::ToDoubleEx(s1);
         d2 = MU_StringUtil::ToDoubleEx(s2);
         isNumber = true;
      }
      catch (std::runtime_error e)
      {
         d1 = 0.0;
         d2 = 0.0;
      }

      if (isNumber)
      {
         if (abs(d1 - d2) > gNumberDelta)
         {
            ++nNumberDiff;
            isDiff = true;
         }
      }
      else
      {
         if (!matchString(s1,s2))
         {
            ++nTextDiff;
            isDiff = true;
         }
      }

      if (isDiff)
      {
         //string msg = "content difference";
         outputDiff(cout, outDiffMsg, s1, s2);
      }
   }

   return (nTextDiff > 0 || nNumberDiff > 0) ? true : false;
}




// focus on specific attributes and ignore others, based on the attribute name
bool isDesiredAttribute(const char* name)
{
   return (
         matchString(name, "name")
      || matchString(name, "type")
      //|| matchString(name, "units")
      );
}

// get the next attribute after this one but only if matches 'desired attribute name'
const XMLAttribute* getNextDesiredAttribute(const XMLAttribute* p)
{
   if (!p)
      return nullptr;

   const XMLAttribute* nextAttrib = p;
   while ((nextAttrib = nextAttrib->Next()) != nullptr)
   {
      // if good attribute can stop and return here
      if (isDesiredAttribute(nextAttrib->Name()))
         return nextAttrib;
   }
   // will get here only if nothing matches - return null
   return nextAttrib;
}

const XMLAttribute* getFirstDesiredAttribute(const XMLElement* elem)
{
   if (!elem)
      return nullptr;

   const XMLAttribute* attrib1 = elem->FirstAttribute();
   if (attrib1)
   {
      if (isDesiredAttribute(attrib1->Name()))
         return attrib1;
      // will return either a good attribute or null
      return getNextDesiredAttribute(attrib1);
   }
   // no attributes
   return attrib1;
}




/**
 * Compare the text within an XML element, count differences, and output those differences.
 * The element content, e.g. <tag>content</tag> is compared, not that tag name or attributes.
 * The content will be broken into tokens using 'delim' and each of those will get compared as
 * text or as numbers if possible.
 *
 * @param elem1 XML element 1 to use in comparison
 * @param elem2 XML element 2 to use in comparison against element 1
 * @param totDiff the number of differences is incremented in this object
 * @param delim string containing the characters to use as delimiters in text1 and text2 to break them into tokens (e.g. "{,\n ")
 */
void compareXmlText(XMLElement* elem1, XMLElement* elem2, XmlDifferences& totDiff, const string& delim)
{
   const char* subtext1 = elem1->GetText();
   const char* subtext2 = elem2->GetText();
   if (subtext1 && subtext2)
   {
      unsigned int nNumberDiff;
      unsigned int nTextDiff;
      if (countTextAsNumberTokenDiff(subtext1, subtext2, delim, "content difference", nNumberDiff, nTextDiff))
      {
         totDiff.totalTagNumbContentDiff += nNumberDiff;
         totDiff.totalTagTextContentDiff += nTextDiff;
         ++totDiff.elemWithTextDiff;           // increment by one number of xml elements that have text diffs

      }
   }
   else if (subtext1 || subtext2)
   {
      // here it means one element has text but not both
      outputDiffcptr(cout, "content difference", subtext1, subtext2);
      ++totDiff.elemWithTextDiff;
   }
}




/**
* Compare the attributes within an XML element, count differences, and output those differences.
* The value of attributes will be broken into tokens using 'delim' and each of those will get compared as
* text or as numbers if possible.
*
* @param elem1 XML element 1 to use in comparison
* @param elem2 XML element 2 to use in comparison against element 1
* @param totDiff the number of differences is incremented in this object
* @param delim string containing the characters to use as delimiters to break attribute value into tokens (e.g. "{,\n ")
*/
void compareXmlAttribs(XMLElement* elem1, XMLElement* elem2, XmlDifferences& totDiff, const string& delim)
{
   const XMLAttribute* attrib1 = getFirstDesiredAttribute(elem1);
   const XMLAttribute* attrib2 = getFirstDesiredAttribute(elem2);
   unsigned int attributeNameCount = 0;   // element has attributes with different names

   unsigned int nNumberDiff;
   unsigned int nTextDiff;

   string msg = " in element <";
   msg += elem1->Value();
   msg += ">";

   while (attrib1 && attrib2)
   {
      const char* attribName1 = attrib1->Name();
      if (!matchString(attribName1, attrib2->Name()))
      {
         outputDiff(cout, "attribute name", attribName1, attrib2->Name());
         ++attributeNameCount;
      }

      // compare attribute values
      string out = "attribute '";
      out += attribName1;
      out += "'=";
      if (countTextAsNumberTokenDiff(attrib1->Value(), attrib2->Value(), delim, out, nNumberDiff, nTextDiff))
      {
         ++totDiff.elemWithAttribValueDiff;    // this element had one or more differences in an attribute value contents
      }

      //attrib1 = attrib1->Next();
      //attrib2 = attrib2->Next();
      attrib1 = getNextDesiredAttribute(attrib1);
      attrib2 = getNextDesiredAttribute(attrib2);
   }

   // see if extra sibling elements for file 1 or 2 exist
   while (attrib1 || attrib2)
   {
      const char* attribName1 = (attrib1 ? attrib1->Name() : "");
      const char* attribName2 = (attrib2 ? attrib2->Name() : "");
      outputDiff(cout, "attribute name", attribName1, attribName2);
      ++attributeNameCount;

      if (attrib1)
         attrib1 = getNextDesiredAttribute(attrib1);
      if (attrib2)
         attrib2 = getNextDesiredAttribute(attrib2);
   }

   // if one or more different attribute names than add one to "tag with attribute name differences" counter
   if (attributeNameCount > 0)
   {
      ++totDiff.elemWithAttribNameDiff;
   }
}




/**
 * Compare two XML files for differences.
 * The two elements passed in are the starting point.  Child elements will be compared and then siblings.
 *
 * @param elem1 XML element 1 to use in comparison
 * @param elem2 XML element 2 to use in comparison against element 1
 * @param totDiff the number of differences is incremented in this object
 * @param delim string containing the characters to use as delimiters to break attribute value into tokens (e.g. "{,\n ")
 * @return true if there is a major difference that should require stopping
 */
bool compareXmlFiles(XMLElement* elem1, XMLElement* elem2, XmlDifferences& totDiff, bool sideBySide, const string& delim = " ")
{
   const bool stopOnMajorDiff = false;
   const size_t margin = 80;
   XMLElement* element1 = elem1;
   XMLElement* element2 = elem2;
   map<string, string> attribs;

   while (element1 && element2)
   {
      if (sideBySide)
         outputElementsSideBySide(element1, element2, margin);

      ++totDiff.totalElemCompared;
      const char* tagValue1 = element1->Value();   // tag 1 value: <tag>
      if (!matchString(tagValue1, element2->Value()))
      {
         string s1 = "<";
         s1 += tagValue1;
         s1 += ">";
         string s2 = "<";
         s2 += element2->Value();
         s2 += ">";
         outputDiff(cout, "XML Tag difference", s1, s2);

         ++totDiff.totalDifferentTypeElem;
         //if (stopOnMajorDiff)
         //   return true;  // return here will cause comparison to stop if different tags encountered
      }

      // see if element 1 matches the 'ignore' filters.  if so then skip the attribute and content checks
      getXmlAttributesFromElem(element1, attribs);
      // keep track of model tree
      pushToModelTree(tagValue1, attribs);
      //outputModelTree(cout);
      if (checkXmlFilter(tagValue1, attribs))
      {
         // matched filter so ignore further checks
      }
      else
      {
         // check attributes for differences
         compareXmlAttribs(element1, element2, totDiff, delim);

         // compare the text within the <tag>...</tag>
         compareXmlText(element1, element2, totDiff, delim);
      }

      // check out child elements.  if it returns true it means fatal error so stop
      if (compareXmlFiles(element1->FirstChildElement(), element2->FirstChildElement(), totDiff, sideBySide, delim) && stopOnMajorDiff)
         return true;

      // go to next sibling element and continue the comparison
      element1 = element1->NextSiblingElement();
      element2 = element2->NextSiblingElement();

      popFromModelTree();
   }

   // see if more sibling elements for file 1 or 2
   if (element1)
   {
      totDiff.extraElemFile1 = countElement(element1);
   }
   if (element2)
   {
      totDiff.extraElemFile2 = countElement(element2);
   }

   return false;
}



/**
 * Compare two XML documents for differences.
 * Starts with the first child of each document (ignores the root element).
 */
bool compareXmlFiles(XMLDocument& doc1, XMLDocument& doc2, XmlDifferences& totDiff, bool sideBySide, const string& delim = " ")
{
   XMLElement* element1 = doc1.FirstChildElement();
   XMLElement* element2 = doc2.FirstChildElement();
   return compareXmlFiles(element1, element2, totDiff, sideBySide, delim);
}




void writeXmlFile(const std::string& filename, XMLDocument& doc)
{
   doc.SaveFile(filename.c_str());
}




void writeXmlFiles(XMLDocument& doc1, XMLDocument& doc2)
{
   writeXmlFile("xmldiff_file1.xml", doc1);
   writeXmlFile("xmldiff_file2.xml", doc2);
}




/**
 * Output differences, both summary of all differences and optional total appended to a file.
 */
void outputDiff(const std::string& compare1, const std::string& compare2, XmlDifferences& totDiff, ostream& out, const std::string& totFile)
{
   out << totDiff;

   if (!totFile.empty())
   {
      ofstream aout(totFile, ios_base::app);
      if (aout)
      {
         aout << compare1 << "," << compare2 << "," << totDiff.Total() << endl;
         aout.close();
      }
      else
      {
         cout << "Error: unable to open total file '" << totFile << "'" << endl;
      }
   }
}




/**
 * Process the settings from optional config file (to change defaults, etc).
 *
 * @param configXml an XMLDocument containing the config file after reading it in
 */
void processConfigFile(const XMLDocument* configXml)
{
   if (!configXml)
      return;
   const XMLElement* root = configXml->FirstChildElement();
   if (!root)
      return;

   const XMLElement* elem = root->FirstChildElement();
   while (elem)
   {
      const char* tagValue = elem->Value();
      if (MU_StringUtil::Strcasecmp(tagValue, "delta"))
      {
         const char* text = elem->GetText();
         if (text)
         {
            try
            {
               gNumberDelta = MU_StringUtil::ToDoubleEx(text);
            }
            catch (std::runtime_error e)
            {
               // no need for anything here
            }
         }
      }
      else if (MU_StringUtil::Strcasecmp(tagValue, "case"))
      {
         const char* text = elem->GetText();
         if (text)
         {
            if (MU_StringUtil::Strcasecmp(text, "true") || MU_StringUtil::Strcasecmp(text, "t") || MU_StringUtil::Strcasecmp(text, "yes"))
               gCaseSensitive = true;
            else if (MU_StringUtil::Strcasecmp(text, "false") || MU_StringUtil::Strcasecmp(text, "f") || MU_StringUtil::Strcasecmp(text, "no"))
               gCaseSensitive = false;
         }
      }
      else if (MU_StringUtil::Strcasecmp(tagValue, "ignore"))
      {
         const XMLElement* ignElem = elem->FirstChildElement();
         while (ignElem)
         {
            XmlFilter filter(ignElem,gCaseSensitive);
            gXmlFilters.push_back(filter);

            ignElem = ignElem->NextSiblingElement();
         }
      }
      else if (MU_StringUtil::Strcasecmp(tagValue, "delim"))
      {
         const char* text = elem->GetText();
         if (text)
         {
            string inString(text);
            gDelimiters = MU_StringUtil::ToEscapedString(text);
         }

      }

      elem = elem->NextSiblingElement();
   }
}




int main(int argc, char**argv)
{
   const char* progName = *argv;  // name of executable

   // set defaults for run-time settings.  Look at RunSettings constructor to see what is what
   RunSettings runSettings(0.0, true, "|{, \n", false, false, false, false, "");
   // modify run-time settings based on user command-line arguments
   MyGetOpt(argc, argv, runSettings);

   if (runSettings.showVersion())
   {
      ProgramVersion::printVersion();
      return 1;
   }
   if (runSettings.showUsage())
   {
      Usage(progName);
      return 1;
   }

   if (runSettings.unswitchedSize() != 2)
   {
      UsageBasic(progName);
      return 1;
   }

   // read configuration file (if used) to get values.  try to do this
   // before any command-line switch values are used as the command-line
   // switches should override config file
   processConfigFile(runSettings.getConfig());

   if (runSettings.getDelta() != 0.0)
      gNumberDelta = runSettings.getDelta();

   if (runSettings.caseSet())
      gCaseSensitive = runSettings.getCase();
   setXmlFilterCase(gCaseSensitive);

   if (runSettings.delimSet())
      gDelimiters = runSettings.getDelim();

   const char* filename1 = runSettings.getUnswitched(0).c_str();
   const char* filename2 = runSettings.getUnswitched(1).c_str();

   XMLDocument doc1, doc2;
   doc1.LoadFile(filename1);
   if (doc1.Error())
   {
      cout << "Error opening file '" << filename1 << "': " << doc1.ErrorName() << endl;
      return 1;
   }
   doc2.LoadFile(filename2);
   if (doc2.Error())
   {
      cout << "Error opening file '" << filename2 << "': " << doc2.ErrorName() << endl;
      return 1;
   }

   if (runSettings.getReformat())
      writeXmlFiles(doc1, doc2);

   XmlDifferences totDiff;
   compareXmlFiles(doc1, doc2, totDiff, runSettings.getSideBySide(), gDelimiters);

   outputDiff(runSettings.getUnswitched(0), runSettings.getUnswitched(1), totDiff, cout, runSettings.totalFile());

   return 0;
}
