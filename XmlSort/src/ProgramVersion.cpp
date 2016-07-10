

#include <iostream>

#include "ProgramVersion.h"



float       ProgramVersion::versionNumber_ = (float)0.11;
std::string ProgramVersion::versionString_ = "0.1.1g";


/*****************************************************************************

Function: printVersion( )

Display program version.

*****************************************************************************/

void ProgramVersion::printVersion( )
{
	std::cout << "\nXmlSort. version " << ProgramVersion::versionString().c_str()
             << "\nCompiled " << __DATE__ << " at " << __TIME__ << std::endl; 
}

