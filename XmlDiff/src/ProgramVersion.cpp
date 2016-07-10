

#include <iostream>

#include "ProgramVersion.h"



float       ProgramVersion::versionNumber_ = (float)0.56;
std::string ProgramVersion::versionString_ = "0.5.6a";


/*****************************************************************************

Function: printVersion( )

Display program version.

*****************************************************************************/

void ProgramVersion::printVersion( )
{
	std::cout << "\nXmlDiff. version " << ProgramVersion::versionString().c_str()
             << "\nCompiled " << __DATE__ << " at " << __TIME__ << std::endl; 
}

