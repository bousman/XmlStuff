
/**
 *
 * @file RunSettings.cpp
 * @brief This file contains the member function definitions for class RunSettings
 */

#include <string>
#include <fstream>

#include "RunSettings.h"
#include "MU_StringUtil.h"

using namespace std;


// ==========================================================================
RunSettings::RunSettings()
   : mDelta(1.0e-7)
   , mCaseSensitive(true)
   , mCaseSensitiveSet(false)
   , mDelimiters()
   , mDelimitersSet(false)
   , mReformat(false)
   , mSideBySide(false)
   , mConfigXml()
   , mShowVersion(false)
   , mShowUsage(false)
   , mTotalFile()
   , mUnswitched()
{
}




// ==========================================================================
RunSettings::RunSettings(double aDelta, bool aCase, const std::string& aDelim,
   bool aReformat, bool aSide, bool aVersion, bool aUsage, const std::string& aTotal)
   : mDelta(aDelta)
   , mCaseSensitive(aCase)
   , mCaseSensitiveSet(false)
   , mDelimiters(aDelim)
   , mDelimitersSet(true)
   , mReformat(aReformat)
   , mSideBySide(aSide)
   , mConfigXml()
   , mShowVersion(aVersion)
   , mShowUsage(aUsage)
   , mTotalFile(aTotal)
   , mUnswitched()
{
}




// ==========================================================================
RunSettings::~RunSettings()
{
   destroy();
}




// ==========================================================================
RunSettings::RunSettings(const RunSettings& p)
   : mDelta(p.mDelta)
   , mCaseSensitive(p.mCaseSensitive)
   , mCaseSensitiveSet(p.mCaseSensitiveSet)
   , mDelimiters(p.mDelimiters)
   , mDelimitersSet(p.mDelimitersSet)
   , mReformat(p.mReformat)
   , mSideBySide(p.mSideBySide)
   , mConfigXml()
   , mShowVersion(p.mShowVersion)
   , mShowUsage(p.mShowUsage)
   , mTotalFile(p.mTotalFile)
   , mUnswitched(p.mUnswitched)
{
   setConfig(p.mConfigXml);
}




// ==========================================================================
RunSettings& RunSettings::operator = (const RunSettings& p)
{
   // check for argument being same as itself...
   if (&p != this)
   {
      // delete any objects that are dynamically allocated
      destroy();

      // now copy contents
      mDelta         = p.mDelta;
      mCaseSensitive = p.mCaseSensitive;
      mCaseSensitiveSet = p.mCaseSensitiveSet;
      mDelimiters    = p.mDelimiters;
      mDelimitersSet = p.mDelimitersSet;
      mReformat      = p.mReformat;
      mSideBySide    = p.mSideBySide;
      mShowVersion   = p.mShowVersion;
      mShowUsage     = p.mShowUsage;
      mTotalFile     = p.mTotalFile;
      mUnswitched    = p.mUnswitched;
      setConfig(p.mConfigXml);
   }

   return *this;
}




// ==========================================================================
void RunSettings::show(std::ostream& stream) const
{
   stream << "<RunSettings>";
   stream << "<delta>" << mDelta << "</delta>";
   stream << "<case>" << (mCaseSensitive ? "true" : "false") << "</case>";
   stream << "<delim>" << mDelimiters << "</delim>";
   stream << "<reformat>" << (mReformat ? "true" : "false") << "</reformat>";
   stream << "<side>" << (mReformat ? "true" : "false") << "</side>";
   XMLPrinter printer;
   mConfigXml.Print(&printer);
   stream << "<config>" << printer.CStr() << "</config>";
   stream << "<version>" << mShowVersion << "</version>";
   stream << "<usage>" << mShowUsage << "</usage>";
   stream << "<Total>" << mTotalFile << "</Total>";
   stream << "</RunSettings>";
}




// ==========================================================================
std::ostream& operator << (std::ostream& o, const RunSettings* w)
{
   if (w)
      w->show(o);
   else
      o << "NULL RunSettings";
   return o;
}




// ==========================================================================
void RunSettings::destroy()
{
}




// ==========================================================================
void RunSettings::setConfig(const XMLDocument& doc)
{
   mConfigXml.Clear();

   for (const XMLNode* node = doc.FirstChild(); node; node = node->NextSibling())
   {
      XMLNode* copy = node->ShallowClone(&mConfigXml);
      mConfigXml.InsertEndChild(copy);
   }
}




// ==========================================================================
bool RunSettings::setConfig(const std::string& filename)
{
   string fullFile = filename;
   ifstream testIn(fullFile);
   if (!testIn)
   {
      fullFile += ".xml";
      ifstream testIn2(fullFile);
      if (!testIn2)
      {
         return true;
      }
      testIn2.close();
   }


   mConfigXml.Clear();

   mConfigXml.LoadFile(fullFile.c_str());
   if (mConfigXml.Error())
   {
      std::cout << "Error opening config file " << filename << " : " << mConfigXml.ErrorName() << std::endl;
      return true;
   }
   return false;
}




// ==========================================================================
void RunSettings::setCase(const std::string& aCase)
{
   if (MU_StringUtil::Strcasecmp(aCase, "true") || MU_StringUtil::Strcasecmp(aCase, "t") || MU_StringUtil::Strcasecmp(aCase, "yes"))
   {
      mCaseSensitive = true;
      mCaseSensitiveSet = true;
   }
   else if (MU_StringUtil::Strcasecmp(aCase, "false") || MU_StringUtil::Strcasecmp(aCase, "f") || MU_StringUtil::Strcasecmp(aCase, "no"))
   {
      mCaseSensitive = false;
      mCaseSensitiveSet = true;
   }
}




// ==========================================================================
void RunSettings::setDelim(const std::string& aDelim)
{
   mDelimiters = aDelim;
   mDelimitersSet = true;
}
