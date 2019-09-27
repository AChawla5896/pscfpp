/*
* PSCF - Polymer Self-Consistent Field Theory
*
* Copyright 2016, The Regents of the University of Minnesota
* Distributed under the terms of the GNU General Public License.
*/

#include <pssp/System.h>

/**
* \page pscf_ps3d_page pscf_ps3d
*
* Polymer Self-Consistent Field Theory - Three-dimensional, Pseudo-spectral
*
* Usage:
*
*    pscf_ps3d [-e] [-p file] [-c file] [-i prefix] [-o prefix]
*
*
*    Note: Normal usage requires both a parameter file (-p option) and
*    a command file (-c option). The other options are truly optional.
*
* Options:
*
*   -e  
*
*    Enable echoing of parameter file to log file as it is read. This
*    option is often useful for debugging the parameter file.
*
*  -p file
*
*   Set the parameter file name, given by the argument "file". 
*   The -p and -r options are incompatible. 
*
*  -c file
*
*   Set the command file name, given by the argument "file". The
*   command file may also be specified in the parameter file.
*
*  -i prefix
*
*   Set the input file path prefix, given by the argument "prefix".
*
*  -o prefix
*
*   Set the output file path prefix, given by the argument "prefix".
*
*/

int main(int argc, char **argv)
{
   Pscf::Pssp::System<3> system;

   // Process command line options
   system.setOptions(argc, argv);

   // Read parameters from default parameter file
   system.readParam();

   // Read command script to run system
   system.readCommands();

   return 0;
}
