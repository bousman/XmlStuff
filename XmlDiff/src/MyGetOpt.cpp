
#include <iostream>
#include <string>
#include <cstdio>
#include <vector>

#include "MyGetOpt.h"
#include "MU_StringUtil.h"





/*****************************************************************************

Function: MyOptArg( int argc, char **argv, int minargs, int maxargs, char **optlist )

Check for command line for arguments to switches.  At the point of this call,
argv points to the switch command.  Parse the rest of the command line to get
the right amount of arguments and return pointers to them.

Inputs:
argc - number of command line arguments left (including switch)
argv - array of command line arguments
minargs - minimum number of switch arguments
maxargs - maximum number of switch arguments

Returns:
optlist - vector of strings that are arguments to the command switch (typically 1)
so if user typed "-arg val1 val2" it would return "val1" and "val2"
the number of arguments to the switch (contained in optlist) or -1 if an error.

*****************************************************************************/

int MyOptArg( int &argc, char **&argv, int minargs, int maxargs,
           std::vector<std::string>& optlist )
{
   // save off pointer to the argument being processed
   char *optptr = *argv;
   optlist.clear();

   if ( argc <= minargs )
   {
      std::cout << "Not enough arguments specified for option " << optptr << "\n"
                << "This option requires at least " << minargs << " argument(s)\n"
                << "Use  --h' for help on program usage" << std::endl;
      return -1;
   }

   // run through the rest of the command-line arguments and store them off
   // until we hit the maximum number of arguments or run out of them
   int narg = 0;
   while ( argc > 0 && narg < maxargs )
   {
      --argc; ++argv;

      size_t argLen = 0;
      if ( argv && *argv )
         argLen = strlen( *argv );
      if ( argLen > 1 && *argv[0] == '-' && *argv[1] == '-' )
      // if parameter starts with - then assume it is the next argument
      //if ( *argv[0] == '-' )
         break;

      std::string optval = *argv;
      optlist.push_back( optval );
      narg++;
   }

   if ( narg < minargs )
   {
      std::cout << "Not enough arguments specified for option " << optptr << "\n"
                << "This option requires at least " << minargs << " argument(s)\n"
                << "Use  --h' for help on program usage" << std::endl;
      return -1;
   }

   return narg;
}




void MyGetOpt( int argc, char **argv, RunSettings& runSettings )
{
   --argc; ++argv;  // skip over program name

   // arguments to the command switch
   std::vector<std::string> optlist;

   while ( argc > 0 )
   {
      size_t argLen = 0;
      if ( argv && *argv )
         argLen = strlen( *argv );
      if ( argLen > 0 && (*argv)[0] == '-' ||
          (argLen > 1 && (*argv)[0] == '-' && (*argv)[1] == '-' ))
      {
         if ( MU_StringUtil::Strcasecmp( *argv, "--h" ) || MU_StringUtil::Strcasecmp( *argv, "--help" ) )
         {
            runSettings.showUsage(true);
         }
         else if (MU_StringUtil::Strcasecmp(*argv, "--version") || MU_StringUtil::Strcasecmp(*argv, "--v"))
         {
            runSettings.showVersion(true);
         }
         else if (MU_StringUtil::Strcasecmp(*argv, "--delta"))
         {
            if (MyOptArg(argc, argv, 1, 1, optlist) < 0)
            {
               exit(0);
            }
            else
            {
               runSettings.setDelta(atof(optlist[0].c_str()));
               //std::cout << "mdelta = " << runSettings.getDelta() << std::endl;
            }
         }
         else if (MU_StringUtil::Strcasecmp(*argv, "--case"))
         {
            if (MyOptArg(argc, argv, 1, 1, optlist) < 0)
            {
               exit(0);
            }
            else
            {
               runSettings.setCase(optlist[0]);
            }
         }
         else if (MU_StringUtil::Strcasecmp(*argv, "--config") || MU_StringUtil::Strcasecmp(*argv, "-c"))
         {
            if (MyOptArg(argc, argv, 1, 1, optlist) < 0)
            {
               exit(0);
            }
            else
            {
               if (runSettings.setConfig(optlist[0]))
                  exit(0);
            }
         }
         else if (MU_StringUtil::Strcasecmp(*argv, "--delim"))
         {
            if (MyOptArg(argc, argv, 1, 1, optlist) < 0)
            {
               exit(0);
            }
            else
            {
               // string has to be processed to turn input '\','n' into newline
               //std::string outString = MU_StringUtil::ToEscapedString(optlist[0]);
               runSettings.setDelim(MU_StringUtil::ToEscapedString(optlist[0]));
            }
         }
         else if (MU_StringUtil::Strcasecmp(*argv, "--ref"))
         {
            runSettings.setReformat(true);
         }
         else if (MU_StringUtil::Strcasecmp(*argv, "--side"))
         {
            runSettings.setSideBySide(true);
         }
         else if (MU_StringUtil::Strcasecmp(*argv, "--total"))
         {
            if (MyOptArg(argc, argv, 1, 1, optlist) < 0)
            {
               exit(0);
            }
            else
            {
               runSettings.totalFile(optlist[0]);
            }
         }
         else
         {
            runSettings.showUsage(true);
         }
      }
      else
      {
         runSettings.addUnswitched(*argv);
         //std::cout << "Unknown argument '" << *argv << "'" << std::endl;
         //exit(0);
      }

      --argc; ++argv;  // next argument in list
   }

}


