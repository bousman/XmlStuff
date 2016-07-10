
#ifndef ProgramVersion_h
#define ProgramVersion_h

#include <string>


class ProgramVersion
{
public:
	static void printVersion();
   static float version()             { return ProgramVersion::versionNumber_; }
   static std::string versionString() { return ProgramVersion::versionString_; }

private:
   static float       versionNumber_;   // general version # - as in 2.01
   static std::string versionString_;   // full revision #   - as in 2.01.199a
};


#endif
