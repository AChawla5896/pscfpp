/*
* PSCF - Polymer Self-Consistent Field Theory
*
* Copyright 2016, The Regents of the University of Minnesota
* Distributed under the terms of the GNU General Public License.
*/

#include "System.h"
#include "Iterator.h"
#include <pscf/Interaction.h>
#include <pscf/ChiInteraction.h>
#include <util/format/Str.h>
#include <util/format/Int.h>
#include <util/format/Dbl.h>
#ifdef PSCF_GSL
#include "NrIterator.h"
#endif

#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>
#include <unistd.h>

namespace Pscf {
namespace Fd1d
{

   using namespace Util;

   /*
   * Constructor.
   */
   System::System()
    : mixture_(),
      domain_(),
      fileMaster_(),
      interactionPtr_(0),
      iteratorPtr_(0),
      hasMixture_(0),
      hasDomain_(0),
      hasFields_(0)
   {  
      setClassName("System"); 

      #ifdef PSCF_GSL
      interactionPtr_ = new ChiInteraction(); 
      iteratorPtr_ = new NrIterator(); 
      #endif
   }

   /*
   * Destructor.
   */
   System::~System()
   {}

   /*
   * Process command line options.
   */
   void System::setOptions(int argc, char **argv)
   {
      bool eflag = false;  // echo
      bool pFlag = false;  // param file 
      bool cFlag = false;  // command file 
      bool iFlag = false;  // input prefix
      bool oFlag = false;  // output prefix
      char* pArg = 0;
      char* cArg = 0;
      char* iArg = 0;
      char* oArg = 0;
   
      // Read program arguments
      int c;
      opterr = 0;
      while ((c = getopt(argc, argv, "er:p:c:i:o:f")) != -1) {
         switch (c) {
         case 'e':
            eflag = true;
            break;
         case 'p': // parameter file
            pFlag = true;
            pArg  = optarg;
            break;
         case 'c': // command file
            cFlag = true;
            cArg  = optarg;
            break;
         case 'i': // input prefix
            iFlag = true;
            iArg  = optarg;
            break;
         case 'o': // output prefix
            iFlag = true;
            oArg  = optarg;
            break;
         case '?':
           Log::file() << "Unknown option -" << optopt << std::endl;
           UTIL_THROW("Invalid command line option");
         }
      }
   
      // Set flag to echo parameters as they are read.
      if (eflag) {
         Util::ParamComponent::setEcho(true);
      }

      // If option -p, set parameter file name
      if (pFlag) {
         fileMaster().setParamFileName(std::string(pArg));
      }

      // If option -c, set command file name
      if (cFlag) {
         fileMaster().setCommandFileName(std::string(cArg));
      }

      // If option -i, set path prefix for input files
      if (iFlag) {
         fileMaster().setInputPrefix(std::string(iArg));
      }

      // If option -o, set path prefix for output files
      if (oFlag) {
         fileMaster().setOutputPrefix(std::string(oArg));
      }

   }

   /*
   * Read parameters and initialize.
   */
   void System::readParameters(std::istream& in)
   {
      readParamComposite(in, mixture());
      hasMixture_ = true;

      interaction().setNMonomer(mixture().nMonomer());
      readParamComposite(in, interaction());

      readParamComposite(in, domain());
      hasDomain_ = true;
      allocateFields();

      // Initialize iterator
      iterator().setSystem(*this);
      readParamComposite(in, iterator());
   }

   /*
   * Read default parameter file.
   */
   void System::readParam(std::istream& in)
   {
      readBegin(in, className().c_str());  
      readParameters(in);  
      readEnd(in);  
   }

   /*
   * Read default parameter file.
   */
   void System::readParam()
   {  readParam(fileMaster().paramFile()); }

   /*
   * Read parameters and initialize.
   */
   void System::allocateFields()
   {
      // Preconditions
      UTIL_CHECK(hasMixture_);
      UTIL_CHECK(hasDomain_);

      mixture().setDomain(domain());
      int nMonomer = mixture().nMonomer();
      wFields_.allocate(nMonomer);
      cFields_.allocate(nMonomer);
      int nx = domain().nx();
      for (int i = 0; i < nMonomer; ++i) {
         wField(i).allocate(nx);
         cField(i).allocate(nx);
      }
      hasFields_ = true;
   }

   /*
   * Read and execute commands from a specified command file.
   */
   void System::readCommands(std::istream &in)
   {
      //if (!isInitialized_) {
      //    UTIL_THROW("McSimulation is not initialized");
      //}

      std::string command;
      std::string filename;
      std::ifstream inputFile;
      std::ofstream outputFile;

      std::istream& inBuffer = in;

      bool readNext = true;
      while (readNext) {

         inBuffer >> command;
         Log::file() << command;

         if (command == "FINISH") {
            Log::file() << std::endl;
            readNext = false;
         } else
         if (command == "READ_WFIELDS") {
            inBuffer >> filename;
            Log::file() << Str(filename, 15) << std::endl;
            fileMaster().openInputFile(filename, inputFile);
            readWFields(inputFile);
            inputFile.close();
         } else
         if (command == "WRITE_WFIELDS") {
            inBuffer >> filename;
            Log::file() << Str(filename, 15) << std::endl;
            fileMaster().openOutputFile(filename, outputFile);
            writeFields(outputFile, wFields_);
            outputFile.close();
         } else
         if (command == "WRITE_CFIELDS") {
            inBuffer >> filename;
            Log::file() << Str(filename, 15) << std::endl;
            fileMaster().openOutputFile(filename, outputFile);
            writeFields(outputFile, cFields_);
            outputFile.close();
         } else
         if (command == "ITERATE") {
            Log::file() << std::endl;
            iterator().solve();
         } else 
         {
            Log::file() << "  Error: Unknown command  " << std::endl;
            readNext = false;
         }

      }
   }

   /*
   * Read and execute commands from the default command file.
   */
   void System::readCommands()
   {  
      if (fileMaster().commandFileName().empty()) {
         UTIL_THROW("Empty command file name");
      }
      readCommands(fileMaster().commandFile()); 
   }

   void System::readWFields(std::istream &in)
   {
      UTIL_CHECK(hasDomain_);

      // Read grid dimensions
      std::string label;
      int nx, nm;
      in >> label;
      UTIL_CHECK(label == "nx");
      in >> nx;
      UTIL_CHECK(nx == domain().nx());
      in >> label;
      UTIL_CHECK (label == "nm");
      in >> nm;
      UTIL_CHECK(nm == mixture().nMonomer());

      // Read fields
      int i,j, idum;
      for (i = 0; i < nx; ++i) {
         in >> idum;
         UTIL_CHECK(idum == i);
         for (j = 0; j < nm; ++j) {
            in >> wFields_[j][i];
         }
      }
      double shift = wFields_[nm - 1][nx-1];
      for (i = 0; i < nx; ++i) {
         for (j = 0; j < nm; ++j) {
            wFields_[j][i] -= shift;
         }
      }

   }

   void System::writeFields(std::ostream &out, Array<Field> const &  fields)
   {
      int i, j;
      int nx = domain().nx();
      int nm = mixture().nMonomer();
      out << "nx     "  <<  nx              << std::endl;
      out << "nm     "  <<  nm              << std::endl;

      // Write fields
      for (i = 0; i < nx; ++i) {
         out << Int(i, 5);
         for (j = 0; j < nm; ++j) {
            out << "  " << Dbl(fields[j][i]);
         }
         out << std::endl;
      }
   }

} // namespace Fd1d
} // namespace Pscf
