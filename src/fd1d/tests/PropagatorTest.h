#ifndef FD1D_PROPAGATOR_TEST_H
#define FD1D_PROPAGATOR_TEST_H

#include <test/UnitTest.h>
#include <test/UnitTestRunner.h>

#include <fd1d/Propagator.h>
#include <chem/Block.h>
#include <util/math/Constants.h>

#include <fstream>

using namespace Util;
using namespace Chem;
using namespace Fd1d;

class PropagatorTest : public UnitTest 
{

public:

   void setUp()
   {}

   void tearDown()
   {}

  
   void testConstructor()
   {
      printMethod(TEST_FUNC);
      Propagator propagator;
   }

   void testSolve1()
   {
      Block block;
      block.setId(0);
      double length = 2.0;
      block.setLength(length);
      block.setMonomerId(1);

      printMethod(TEST_FUNC);
      Propagator propagator;
      propagator.setBlock(block, 0);
       
      double step = sqrt(6.0);
      propagator.setKuhn(step);
      double xMin = 0.0;
      double xMax = 1.0;
      int nx = 11;
      int ns = 101;
      propagator.setGrid(xMin, xMax, nx, ns);
      DArray<double> w;
      w.allocate(nx);
      double wc = 0.3;
      for (int i = 0; i < nx; ++i) {
         w[i] = wc;
      }
      propagator.solve(w);

      std::cout << "\n Head:\n";
      for (int i = 0; i < nx; ++i) {
         std::cout << "  " << propagator.head()[i];
      }
      std::cout << "\n";

      std::cout << "\n Tail:\n";
      for (int i = 0; i < nx; ++i) {
         std::cout << "  " << propagator.tail()[i];
      }
      std::cout << "\n";
      std::cout << exp(-wc*block.length()) << "\n";
   }

   void testSolve2()
   {
      Block block;
      block.setId(0);
      double length = 0.5;
      block.setLength(length);
      block.setMonomerId(1);

      printMethod(TEST_FUNC);
      Propagator propagator;
      propagator.setBlock(block, 0);
       
      double step = 1.0;
      propagator.setKuhn(step);
      double xMin = 0.0;
      double xMax = 1.0;
      int nx = 33;
      int ns = 10001;
      propagator.setGrid(xMin, xMax, nx, ns);
      DArray<double> q, w;
      q.allocate(nx);
      w.allocate(nx);
      double wc = 0.5;
      for (int i = 0; i < nx; ++i) {
         q[i] = cos(Constants::Pi*(double(i)+0.5)/nx);
         w[i] = wc;
      }
      propagator.solve(w, q);

      std::cout << "\n Head:\n";
      for (int i = 0; i < nx; ++i) {
         std::cout << "  " << propagator.head()[i];
      }
      std::cout << "\n";

      std::cout << "\n Tail:\n";
      for (int i = 0; i < nx; ++i) {
         std::cout << "  " << propagator.tail()[i]/propagator.head()[i];
      }
      std::cout << "\n";

      double dx = (xMax - xMin)/double(nx - 1);
      double k = 2.0*sin(Constants::Pi/(2*nx))/dx;
      double f = k*k*step*step/6.0 + wc;
      std::cout << exp(-f*length) << "\n";
   }

};

TEST_BEGIN(PropagatorTest)
TEST_ADD(PropagatorTest, testConstructor)
TEST_ADD(PropagatorTest, testSolve1)
TEST_ADD(PropagatorTest, testSolve2)
TEST_END(PropagatorTest)

#endif
