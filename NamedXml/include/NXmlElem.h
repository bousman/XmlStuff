
#ifndef NXmlElem_58753_h
#define NXmlElem_58753_h 1

/**
 * @file NXmlElem.h
 * @brief contains prototypes and class declarations for class NXmlElem
 *
 */

#include <iostream>
#include <list>
#include <map>

#include "tinyxml2.h"

using namespace tinyxml2;


/**
 * @class NXmlElem
 * @brief contains an XML element tree but sorted by name.
 *
 * The siblings are sorted by tag name or if there is an attribute 'name="string"' it will
 * use "string" as the name to sort by.  Attributes are also sorted by name in the element.
 *
 */

class NXmlElem
{

public:
   //! Default constructor - initialize member variables to default values.
   /*!
    * Use member functions to set member variables later as required.
    */
   NXmlElem();
   NXmlElem(XMLElement* elem);

   //! Copy Constructor initializes this object using contents of another one.
   /*!
    * @param aSrc the object to copy from
    */
   NXmlElem( const NXmlElem& p );

   //! Destructor
   ~NXmlElem();

   //! comparison operator needed for sorting
   bool operator< (const NXmlElem& p)
   {
      return mName < p.Name();
   }

   //! Assignment Operator makes a copy
   /*!
    * @param aSrc the object to copy from
    */
   NXmlElem& operator = ( const NXmlElem& p );

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
   friend std::ostream& operator << ( std::ostream& o, const NXmlElem& w )
       { w.show(o); return o; }

   //! stream operator writes the object to stream in human-readable format
   /*!
    * Output operator will use the show() function to write the object to the stream
    * @param aOut the output stream to write to
    * @param aSrc object pointer, points to object to write the contents of (if not null)
    */
   friend std::ostream& operator << ( std::ostream& o, const NXmlElem* w );

   //! Get the tag name
   const std::string& TagName() const { return mTagName; }
   //! Set the tag name
   void TagName(const std::string& p) { mTagName = p; }

   //! Get the value of variable mName
   const std::string& Name() const { return mName; }
   //! Set the value of variable mName
   void Name( const std::string& p ) { mName = p; }

   const std::string& Content() const { return mContent; }
   void Content(const std::string& p);

   //! Get all the attribute name/value pairs
   std::map<std::string, std::string> Attribs() const { return mAttribs; }
   //! Replace all the attribute pairs in this element with new ones
   void Attribs( const std::map<std::string, std::string>& p ) { mAttribs = p; }
   //! Add a new name/value attribute - will sort them automatically.
   //! If name same as existing attribute will overwrite it.  Also if attribute name
   //! is literally "name" then value will be used as new name of this NXmlElem object.
   void addAttrib( const std::string& name, const std::string& value);

   //! Get the child elements of this one
   const std::list<NXmlElem>& ChildElems() const { return mChildElems; }
   //! Add another child element and sort the list
   void addChildElem(const NXmlElem& p);

   //! Add the child element(s) - will add this child and all it's siblings
   void addChildren(XMLElement* child);

   static void setIndentLevel(int indent) { indentLevel = (indent >= 0 ? indent : 0); }


protected:
   static void outputXmlString(std::ostream& stream, const std::string& aIn);
   static int indentLevel;
   static void indentStream(std::ostream& o);
   static void increaseIndent() { ++NXmlElem::indentLevel; }
   static void decreaseIndent() { --NXmlElem::indentLevel; if (NXmlElem::indentLevel < 0) NXmlElem::indentLevel = 0; }


private:
   //! Free up memory used by all member data items and set them to zero (this class only, not parent)
   /*!
    * Function is used by destructor and copy constructor and any other function to free up memory
    * for any member variables that dynamically allocates memory.
    */
   void destroy();

   // member variables
   std::string mTagName;   //!< element tag name
   std::string mName;      //!< name of this element - will be name of tag or if attribute name=""
   std::map<std::string,std::string> mAttribs;      //!< attribute name/value pairs
   std::list<NXmlElem> mChildElems;                 //!< child elements of this one (if any)
   std::string mContent;                            //!< contents of the element
};


#endif

