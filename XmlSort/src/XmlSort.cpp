
#include <fstream>
#include <iostream>
#include <string>



#include "MyGetOpt.h"
#include "ProgramVersion.h"
#include "Usage.h"

// NamedXml package that can sort elements. Also includes tinyxml2
#include "NXmlElem.h"

using namespace std;


/**
 * @file XmlSort.cpp
 *
 * Sort an XML file by tag name and sort the attributes by name.
 * If XML element has the attribute 'name' then the value of that attribute
 * will be used in sorting.
 *
 * @author Brian Bousman
 * @version 0.1.0
 */





int main(int argc, char**argv)
{
   const char* progName = *argv;  // name of executable

   // set defaults for run-time settings.  Look at RunSettings constructor to see what is what
   RunSettings runSettings(false, false);
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

   if (runSettings.unswitchedSize() != 1)
   {
      UsageBasic(progName);
      return 1;
   }


   const char* filename1 = runSettings.getUnswitched(0).c_str();


   XMLDocument doc1;
   doc1.LoadFile(filename1);
   if (doc1.Error())
   {
      cout << "Error opening file '" << filename1 << "': " << doc1.ErrorName() << endl;
      return 1;
   }

   // get first line of xml file which should be the xml declaration
   ifstream in(filename1);
   if (in)
   {
      // look for start of xml declaration.  if found, find end of it since there might be
      // more xml on that same physical line and output only up to that line
      std::string declaration;
      getline(in, declaration);
      if (declaration.substr(0,5) == "<?xml")
      {
         size_t pos = 0;
         if ((pos = declaration.find("?>", pos)) != std::string::npos)
         {
            cout << declaration.substr(0, pos + 2) << endl;
         }
      }
   }

   // load into NamedXml which will sort it
   XMLElement* element1 = doc1.FirstChildElement();
   NXmlElem elem(element1);
   cout << elem;

   return 0;
}
