
#ifndef RunSettings_h
#define RunSettings_h 1

/**
 * @file RunSettings.h
 * @brief contains run-time settings for the program
 *
 */

#include <iostream>
#include <vector>
#include <string>

#include "tinyxml2.h"


using namespace tinyxml2;



/**
 * @class RunSettings
 * @brief container class that holds flags to indicate if various things should
 *        be displayed
 *
 */

class RunSettings
{

public:
   //! Default constructor - initialize member variables to default values.
   /*!
    * Use member functions to set member variables later as required.
    */
   RunSettings();

   RunSettings(double aDelta, bool aCase, const std::string& aDelim, bool ref,
      bool aSide, bool aVersion, bool aUsage, const std::string& aTotal);

   //! Copy Constructor initializes this object using contents of another one.
   /*!
    * @param p the object to copy from
    */
   RunSettings( const RunSettings& p );

   //! Destructor
   ~RunSettings();

   //! Assignment Operator makes a copy
   /*!
    * @param p the object to copy from
    */
   RunSettings& operator = ( const RunSettings& p );

   //! Function displays the contents of an object in human-readable format.
   /*!
    * @param stream the output stream to write to
    */
   void show( std::ostream& stream = std::cout ) const;

   //! stream operator writes the object to stream in human-readable format
   /*!
    * Output operator will use the show() function to write the object to the stream
    * @param o the output stream to write to
    * @param w the object to write the contents of
    */
   friend std::ostream& operator << ( std::ostream& o, const RunSettings& w )
       { w.show(o); return o; }

   //! stream operator writes the object to stream in human-readable format
   /*!
    * Output operator will use the show() function to write the object to the stream
    * @param o the output stream to write to
    * @param w object pointer, points to object to write the contents of (if not null)
    */
   friend std::ostream& operator << ( std::ostream& o, const RunSettings* w );


   // get or set the number comparison delta (typically 1e-7)
   double getDelta() const { return mDelta; }
   void setDelta(double d) { mDelta = d; }

   // get or set case-sensitive string comparison flag
   void setCase(const std::string& aCase);
   bool getCase() const { return mCaseSensitive; }
   bool caseSet() const { return mCaseSensitiveSet; }

   void setDelim(const std::string& aDelim);
   std::string getDelim() const { return mDelimiters; }
   bool delimSet() const { return mDelimitersSet; }

   void setReformat(bool r) { mReformat = r; }
   bool getReformat() const { return mReformat; }

   void setSideBySide(bool r) { mSideBySide = r; }
   bool getSideBySide() const { return mSideBySide; }

   // get or set config file (XML)
   void setConfig(const XMLDocument& doc);
   bool setConfig(const std::string& filename);   // return true if error
   const XMLDocument* getConfig() const { return &mConfigXml; }

   // Get the value of flag that indicates if the version number is to be shown
   bool showVersion() const { return mShowVersion; }
   void showVersion( const bool p ) { mShowVersion = p; }

   // Get the value of the flag that indicates if the program usage should be shown
   bool showUsage() const { return mShowUsage; }
   void showUsage( const bool p ) { mShowUsage = p; }

   // get or set the "append total to file" flag
   void totalFile(const std::string& filename) { mTotalFile = filename; }
   std::string totalFile() const { return mTotalFile; }

   const std::string& getUnswitched(unsigned int i) const { return mUnswitched[i]; }
   void addUnswitched(const std::string& s) { mUnswitched.push_back(s); }
   unsigned int unswitchedSize() const { return mUnswitched.size(); }


protected:


private:
   //! Free up memory used by all member data items and set them to zero (this class only, not parent)
   /*!
    * Function is used by destructor and copy constructor and any other function to free up memory
    * for any member variables that dynamically allocates memory.
    */
   void destroy();

   // member variables
   double mDelta;                   //!< delta value if (a-b)<mDelta then numbers are the same
   bool mCaseSensitive;             //!< do case-sensitive string comparisons
   bool mCaseSensitiveSet;          //!< true if case-sensitive switch was set on command-line
   std::string mDelimiters;         //!< delimiter characters to use when tokenizing strings
   bool mDelimitersSet;             //!< true if delimiters has been set
   bool mReformat;                  //!< output the reformatted XML document to files
   bool mSideBySide;                //!< show inputs side by side
   XMLDocument mConfigXml;          //!< configuration file
   bool mShowVersion;               //!< show version number and quit
   bool mShowUsage;                 //!< show program usage and quit
   std::string mTotalFile;          //!< append total differences to this file
   std::vector<std::string> mUnswitched;      //!< unswitched arguments
};


#endif

