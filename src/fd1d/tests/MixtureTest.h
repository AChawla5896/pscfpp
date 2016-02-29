#ifndef FD1D_MIXTURE_TEST_H
#define FD1D_MIXTURE_TEST_H

#include <test/UnitTest.h>
#include <test/UnitTestRunner.h>

#include <fd1d/Mixture.h>

#include <fstream>

using namespace Util;
using namespace Pscf;
using namespace Pscf::Fd1d;

class MixtureTest : public UnitTest 
{

public:

   void setUp()
   {}

   void tearDown()
   {}

  
   void testConstructor()
   {
      printMethod(TEST_FUNC);
      Mixture mix;
   }

   void testReadParameters()
   {
      printMethod(TEST_FUNC);

      std::ifstream in;
      openInputFile("in/Mixture", in);

      Mixture mix;
      mix.readParam(in);

      Domain domain;
      domain.readParam(in);
      mix.setDomain(domain);

      std::cout << "\n";
      mix.writeParam(std::cout);
      domain.writeParam(std::cout);
   }

   void testSolve()
   {
      printMethod(TEST_FUNC);

      std::ifstream in;
      openInputFile("in/Mixture", in);

      Mixture mix;
      Domain domain;
      mix.readParam(in);
      domain.readParam(in);
      mix.setDomain(domain);

      std::cout << "\n";
      mix.writeParam(std::cout);
      domain.writeParam(std::cout);

      int nMonomer = mix.nMonomer();
      DArray<Mixture::WField> wFields;
      DArray<Mixture::CField> cFields;
      wFields.allocate(nMonomer);
      cFields.allocate(nMonomer);
      double nx = (double)domain.nx();
      for (int i = 0; i < nMonomer; ++i) {
         wFields[i].allocate(nx);
         cFields[i].allocate(nx);
      }

      double cs;
      for (int i = 0; i < nx; ++i) {
         cs = cos(Constants::Pi*(double(i)+0.5)/nx);
         wFields[0][i] = 0.5 + cs;
         wFields[1][i] = 0.7 - cs;
      }
      mix.compute(wFields, cFields);

      // Test if same Q is obtained from different methods
      std::cout << mix.polymer(0).propagator(0, 0).computeQ() << "\n";
      std::cout << mix.polymer(0).propagator(1, 0).computeQ() << "\n";
      std::cout << mix.polymer(0).propagator(1, 1).computeQ() << "\n";
      std::cout << mix.polymer(0).propagator(0, 1).computeQ() << "\n";

      // Test spatial integral of block concentration
      double sum0 = 0.0;
      double sum1 = 0.0;
      for (int i = 0; i < nx; ++i) {
         sum0 += cFields[0][i];
         sum1 += cFields[1][i];
      }
      sum0 = sum0/double(nx);
      sum1 = sum1/double(nx);
      std::cout << "Volume fraction of block 0 = " << sum0 << "\n";
      std::cout << "Volume fraction of block 1 = " << sum1 << "\n";
      
   }
};

TEST_BEGIN(MixtureTest)
TEST_ADD(MixtureTest, testConstructor)
TEST_ADD(MixtureTest, testReadParameters)
TEST_ADD(MixtureTest, testSolve)
TEST_END(MixtureTest)

#endif
