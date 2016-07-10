
/**
 *
 * @file XmlFilter.cpp
 * @brief This file contains the member function definitions for class XmlFilter
 */


#include "XmlFilter.h"
#include "MU_StringUtil.h"

using namespace std;


// ==========================================================================
XmlFilter::XmlFilter(bool aCaseSensitive)
: mTagName()
, mAttributes()
, mAttributesLC()
, mCaseSensitive(aCaseSensitive)
{
}




// ==========================================================================
XmlFilter::XmlFilter(const XMLElement* aElem, bool aCaseSensitive)
: mTagName()
, mAttributes()
, mAttributesLC()
, mCaseSensitive(aCaseSensitive)
{
   if (aElem)
   {
      mTagName = aElem->Value();
      const XMLAttribute* attrib = aElem->FirstAttribute();
      while (attrib)
      {
         std::string name = attrib->Name();
         std::string value = attrib->Value();
         mAttributes[name] = value;
         mAttributesLC[name] = value;

         attrib = attrib->Next();
      }
   }
}




// ==========================================================================
XmlFilter::~XmlFilter()
{
   destroy();
}




// ==========================================================================
XmlFilter::XmlFilter( const XmlFilter& p )
: mTagName(p.mTagName)
, mAttributes(p.mAttributes)
, mAttributesLC(p.mAttributesLC)
, mCaseSensitive(p.mCaseSensitive)
{
}




// ==========================================================================
XmlFilter& XmlFilter::operator = ( const XmlFilter& p )
{
   // check for argument being same as itself...
   if ( &p != this )
   {
      // delete any objects that are dynamically allocated
      destroy();

      // now copy contents
      mTagName       = p.mTagName;
      mAttributes    = p.mAttributes;
      mAttributesLC  = p.mAttributesLC;
      mCaseSensitive = p.mCaseSensitive;
   }

   return *this;
}




// ==========================================================================
void XmlFilter::show( std::ostream& stream ) const
{
   stream << "class XmlFilter" << endl;
   stream << "  TagName: " << mTagName << endl;
   stream << "  Case: " << (mCaseSensitive ? "true" : "false") << endl;
   for (mapCaseSen_t::const_iterator a_it = mAttributes.begin(); a_it != mAttributes.end(); ++a_it)
   {
      stream << "  attrib: " << a_it->first << "," << a_it->second << endl;
   }
}




// ==========================================================================
std::ostream& operator << ( std::ostream& o, const XmlFilter* w )
{
   if ( w )
      w->show(o);
   else
      o << "NULL XmlFilter";
   return o;
}




// ==========================================================================
void XmlFilter::destroy()
{
   //mTagName.clear();
   //mAttributes.clear();
}




// ==========================================================================
bool XmlFilter::matchAttrib(const std::string& attribName, const std::string& attribValue) const
{
   //cout << "XmlFilter::match on " << attribName << "=" << attribValue << endl;
   bool retFlag = false;
   mapCaseSen_t::const_iterator a_it = mAttributes.find(attribName);
   if (a_it != mAttributes.end())
   {
      //cout << "   found attribName, checking for value '" << a_it->second << "'=='" << attribValue << "'" << endl;
      retFlag = (mCaseSensitive ? a_it->second == attribValue : MU_StringUtil::Strcasecmp(a_it->second, attribValue));
   }

   return retFlag;
}




// ==========================================================================
bool XmlFilter::matchAnyAttrib(const std::map<std::string, std::string>& attribs) const
{
   for (std::map<std::string, std::string>::const_iterator a_it = attribs.begin();
      a_it != attribs.end(); ++a_it)
   {
      if (matchAttrib(a_it->first, a_it->second))
         return true;
   }
   return false;
}




// ==========================================================================
bool XmlFilter::matchAllAttrib(const std::map<std::string, std::string>& attribs) const
{
   // loop through all attributes in the filter list and see if they match against
   // attributes in the incoming list.  Every attribute in filter must find a match
   //in incoming list in order for this function to return true
   for (std::map<std::string, std::string>::const_iterator a_it = attribs.begin();
      a_it != attribs.end(); ++a_it)
   {
      if (mCaseSensitive)
      {
         mapCaseSen_t::const_iterator filter_it = mAttributes.find(a_it->first);
         if (filter_it != mAttributes.end())
         {
            //cout << "case sensitive found match " << a_it->first << " = " << filter_it->first << endl;
            if (a_it->second != filter_it->second)
               return false;
         }
      }
      else
      {
         mapCaseInsen_t::const_iterator filter_it = mAttributesLC.find(a_it->first);
         if (filter_it != mAttributesLC.end())
         {
            //cout << "case INsensitive found match " << a_it->first << " = " << filter_it->first << endl;
            if (!MU_StringUtil::Strcasecmp(a_it->second, filter_it->second))
               return false;
         }
      }
   }



   return true;
}

