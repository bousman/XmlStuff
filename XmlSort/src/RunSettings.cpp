
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
   : mShowVersion(false)
   , mShowUsage(false)
   , mUnswitched()
{
}




// ==========================================================================
RunSettings::RunSettings(bool aVersion, bool aUsage)
   : mShowVersion(aVersion)
   , mShowUsage(aUsage)
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
   : mShowVersion(p.mShowVersion)
   , mShowUsage(p.mShowUsage)
   , mUnswitched(p.mUnswitched)
{
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
      mShowVersion   = p.mShowVersion;
      mShowUsage     = p.mShowUsage;
      mUnswitched    = p.mUnswitched;
   }

   return *this;
}




// ==========================================================================
void RunSettings::show(std::ostream& stream) const
{
   stream << "<RunSettings>";
   stream << "<version>" << mShowVersion << "</version>";
   stream << "<usage>" << mShowUsage << "</usage>";
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
