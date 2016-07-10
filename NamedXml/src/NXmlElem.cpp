
/**
 *
 * @file NXmlElem.cpp
 * @brief This file contains the member function definitions for class NXmlElem
 */

#include <string>
#include <vector>

#include "NXmlElem.h"


using namespace std;

//static
int NXmlElem::indentLevel = 0;



// ==========================================================================
NXmlElem::NXmlElem()
   : mTagName("tag")
   , mName("unknown")
   , mAttribs()
   , mChildElems()
   , mContent()
{
}




// ==========================================================================
NXmlElem::NXmlElem(XMLElement* elem)
   : mTagName("tag")
   , mName("unknown")
   , mAttribs()
   , mChildElems()
   , mContent()
{
   if (elem)
   {
      TagName(elem->Value());
      Name(elem->Value());   // use tag name as name of element

      const XMLAttribute* attrib = elem->FirstAttribute();
      while (attrib)
      {

         string attribName = attrib->Name();
         addAttrib(attribName, attrib->Value());

         if (attribName == "name")
            Name(attrib->Value());   // if attribute name is literally "name" use that as element name

         attrib = attrib->Next();
      }

      if (elem->GetText())
         Content(elem->GetText());

      // if a child, add it plus any sibling children
      XMLElement* child = elem->FirstChildElement();
      if (child)
         addChildren(child);
   }
}




// ==========================================================================
NXmlElem::~NXmlElem()
{
   destroy();
}




// ==========================================================================
NXmlElem::NXmlElem(const NXmlElem& p)
   : mTagName(p.mTagName)
   , mName(p.mName)
   , mAttribs(p.mAttribs)
   , mChildElems(p.mChildElems)
   , mContent(p.mContent)
{
}




// ==========================================================================
NXmlElem& NXmlElem::operator = (const NXmlElem& p)
{
   // check for argument being same as itself...
   if (&p != this)
   {
      // delete any objects that are dynamically allocated
      destroy();

      // now copy contents
      mTagName = p.mTagName;
      mName = p.mName;
      mAttribs = p.mAttribs;
      mChildElems = p.mChildElems;
      mContent = p.mContent;
   }

   return *this;
}




// ==========================================================================
void NXmlElem::show(std::ostream& stream) const
{
   //stream << "** tag " << mName << " **" << std::endl;
   NXmlElem::indentStream(stream);
   stream << "<" << mTagName;
   if (!mAttribs.empty())
   {
      for (auto n = mAttribs.begin(); n != mAttribs.end(); ++n)
      {
         //stream << " " << n->first << "=\"" << n->second << "\"";
         stream << " " << n->first << "=\"";
         outputXmlString(stream, n->second);
         stream << "\"";
      }
   }

   // if no children or content can close the tag now
   if (mChildElems.empty() && mContent.empty())
   {
      stream << " />" << std::endl;   // close empty tag
   }
   else
   {
      stream << ">";     // close the tag before writing children or content
      if (!mChildElems.empty())
      {
         stream << endl;
         increaseIndent();
         for (auto child = mChildElems.begin(); child != mChildElems.end(); ++child)
         {
            stream << *child;
         }
         decreaseIndent();
      }

      if (!mContent.empty())
      {
         outputXmlString(stream, mContent);
      }

      stream << "</" << mTagName << ">" << std::endl;
   }
}




// ==========================================================================
std::ostream& operator << (std::ostream& o, const NXmlElem* w)
{
   if (w)
      w->show(o);
   else
      o << "NULL NXmlElem";
   return o;
}




// ==========================================================================
void NXmlElem::destroy()
{
   mAttribs.clear();
   mChildElems.clear();
}




// ==========================================================================
void NXmlElem::addAttrib(const std::string& name, const std::string& value)
{
   if (!value.empty())
   {
      if (name == "name")
      {
         Name(value);   // use attribute value as new element name
      }
      mAttribs[name] = value;
   }
}




// ==========================================================================
void NXmlElem::addChildElem(const NXmlElem& p)
{
   mChildElems.push_back(p);
   mChildElems.sort();
}




// ==========================================================================
void NXmlElem::addChildren(XMLElement* child)
{
   XMLElement* child1 = child;
   while (child1)
   {
      // this is rather inefficient since the child element may be a big tree
      // which will get built and then copied whole into THIS. Probably need
      // to make the children of THIS be pointers instead of objects
      NXmlElem elem(child1);
      addChildElem(elem);

      child1 = child1->NextSiblingElement();
   }
}




void NXmlElem::Content(const std::string& p)
{
   mContent = p;
}




void NXmlElem::outputXmlString(std::ostream& stream, const std::string& aIn)
{
   const string searches = { "<>&'\"" };
   const vector<string> replaces = { "&lt;", "&gt;", "&amp;", "&apos;", "&quot;" };

   size_t pos;
   for (string::const_iterator it = aIn.begin(); it != aIn.end(); ++it)
   {
      unsigned char c = *it;
      pos = searches.find(c);
      // is character one of the restricted xml characters that must be substituted?
      if (pos == string::npos)
      {
         // how about non-printable character?  Using ASCII here and not Unicode or double-wide
         if (c < 9 || (c > 13 && c < 32) || c > 126)
         {
            stream << "&#" << (unsigned int)c << ";";
         }
         else
         {
            stream << c;
         }
      }
      else
      {
         stream << replaces[pos];
      }
   }
}




//static
void NXmlElem::indentStream(std::ostream& o)
{
   for (int i = 0; i < NXmlElem::indentLevel; ++i)
      o << ' ';
}
