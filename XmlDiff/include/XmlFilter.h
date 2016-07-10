
#ifndef XmlFilter_46979_h
#define XmlFilter_46979_h 1

/**
 * @file XmlFilter.h
 * @brief contains prototypes and class declarations for class XmlFilter
 *
 */

#include <iostream>
#include <string>
#include <cstring>
#include <map>

#include "tinyxml2.h"
#include "MU_StringUtil.h"


using namespace tinyxml2;
using namespace std;

#ifdef _MSC_VER 
//not #if defined(_WIN32) || defined(_WIN64) because we have strncasecmp in mingw
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#endif



/**
 * @class XmlFilter
 * @brief Holds XML information (tag, attributes) to be used in filtering.
 *
 * An XmlFilter object contains an XML element tag and zero or more attributes
 * with name=value that can be matched against. The main function to use is
 * 'match()' which will check the element tag name and any attributes to see
 * if they match.  The text will be matched case-insensitive unless the case
 * sensitivity flag is set with setCase(true).  To match, the element being
 * checked must have the same tag name and all of the attributes that are in
 * the filter. The element can have additioinal attributes and still be a match.
 *
 * For speed, this function keeps the map of attributes both as a case-
 * insensitive and case-sensitive map so either one can be used.  This is
 * something to consider improving when time permits.
 *
 */

class XmlFilter
{

public:
   //! Default constructor - initialize member variables to default values.
   /*!
    * Use member functions to set member variables later as required.
    */
   XmlFilter(bool aCaseSensitive = false);

   //! initialize member variables to with an XML element
   /*!
   * Use the element tag name and any attribute pairs to initialize this filter
   */
   XmlFilter(const XMLElement* aElem, bool aCaseSensitive = false);

   //! Copy Constructor initializes this object using contents of another one.
   /*!
    * @param aSrc the object to copy from
    */
   XmlFilter( const XmlFilter& p );

   //! Destructor
   ~XmlFilter();

   //! Assignment Operator makes a copy
   /*!
    * @param aSrc the object to copy from
    */
   XmlFilter& operator = ( const XmlFilter& p );

   //! Function displays the contents of an object in human-readable format.
   /*!
    * @param aOut the output stream to write to
    */
   void show( std::ostream& stream = std::cout ) const;

   //! stream operator writes the object to stream in human-readable format
   /*!
    * Output operator will use the show() function to write the object to the stream
    * @param aOut the output stream to write to
    * @param aSrc the object to write the contents of
    */
   friend std::ostream& operator << ( std::ostream& o, const XmlFilter& w )
       { w.show(o); return o; }

   //! stream operator writes the object to stream in human-readable format
   /*!
    * Output operator will use the show() function to write the object to the stream
    * @param aOut the output stream to write to
    * @param aSrc object pointer, points to object to write the contents of (if not null)
    */
   friend std::ostream& operator << ( std::ostream& o, const XmlFilter* w );

   //! Get the value of variable mTagName
   const std::string& TagName() const { return mTagName; }
   //! Set the value of variable mTagName
   void TagName( const std::string& p ) { mTagName = p; }

   //! are comparisons with this object case sensitive?
   bool caseSensitive() const { return mCaseSensitive;  }
   //! set the case sensitivity for matches (true=yes it is case sensitive)
   void caseSensitive(bool aFlag) { mCaseSensitive = aFlag; }

   //! does input tag match this one.  will do case-sensitive or case-insensitive depending on member
   bool matchTag(const std::string& aTag) const {
      return (mCaseSensitive ? aTag == mTagName : MU_StringUtil::Strcasecmp(aTag, mTagName));
   }

   //! does this attribute match one of the filter attributes? (name & value)
   bool matchAttrib(const std::string& attrib, const std::string& attribValue) const;
   //! check to see if any in a list (map) of attributes match in the filter
   bool matchAnyAttrib(const std::map<std::string, std::string>& attribs) const;

   //! check to see if all attributes in filter are also in the attributes being passed in
   bool matchAllAttrib(const std::map<std::string, std::string>& attribs) const;

   //! Does the XML tag and attributes match this filter?  (Must match all attributes in filter)
   bool match(const std::string& aTag, const std::map<std::string, std::string>& attribs) const
   {
      return matchTag(aTag) && matchAllAttrib(attribs);
   }



protected:
   // comparison function for map to be case sensitive
   struct ciLessSen : public std::binary_function<string, string, bool> {
      bool operator()(const string& lhs, const string& rhs) const {
         return strcmp(lhs.c_str(), rhs.c_str()) < 0;
      }
   };

   // comparison function for map to be case insensitive
   struct ciLessInsen : public std::binary_function<string, string, bool>
   {
      bool operator() (const string& lhs, const string& rhs) const {
         return strcasecmp(lhs.c_str(), rhs.c_str()) < 0;
      }
   };

   typedef map<string, string, ciLessSen> mapCaseSen_t;       //!< map attribute name to value, case sensitive
   typedef map<string, string, ciLessInsen> mapCaseInsen_t;   //!< map attribute name to value, case insensitive


private:
   //! Free up memory used by all member data items and set them to zero (this class only, not parent)
   /*!
    * Function is used by destructor and copy constructor and any other function to free up memory
    * for any member variables that dynamically allocates memory.
    */
   void destroy();

   // member variables
   std::string mTagName;          //!< xml element tag e.g. <tag>
   mapCaseSen_t mAttributes;      //!< attributes to filter on, name and value e.g.  "type","double".  case sensitive
   mapCaseInsen_t mAttributesLC;  //!< attributes to filter on, name and value e.g.  "type","double" but case insensitive
   bool mCaseSensitive;           //!< if true all string compares are case sensitive
};


#endif

