
#ifndef MyGetOpt_h
#define MyGetOpt_h 1


#include "RunSettings.h"


/**
 * Check for command-line arguments when the program is run.
 * The number and value of command-line arguments passed to main entry point
 * are parsed to look for command switches (--).  The runSettings object
 * should be initialized with default values for all settings.
 *
 * @param argc   number of command-line arguments as passed in to main()
 * @param argv   command-line arguments as passed in to main()
 * @param runSettings   object has values modified by all command-line arguments
 *
 */
void MyGetOpt( int argc, char **argv, RunSettings& runSettings );


#endif

