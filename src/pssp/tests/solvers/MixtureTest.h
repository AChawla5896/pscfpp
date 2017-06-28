#ifndef PSSP_MIXTURE_TEST_H
#define PSSP_MIXTURE_TEST_H

#include <test/UnitTest.h>
#include <test/UnitTestRunner.h>

#include <pssp/solvers/Mixture.h>
#include <pssp/solvers/Polymer.h>
#include <pssp/solvers/Block.h>
#include <pssp/solvers/Propagator.h>
#include <pscf/mesh/Mesh.h>
#include <pscf/crystal/UnitCell.h>
#include <pscf/math/IntVec.h>
#include <util/math/Constants.h>

#include <fstream>

using namespace Util;
using namespace Pscf;
using namespace Pscf::Pssp;

class MixtureTest : public UnitTest
{

public:

   void setUp()
   {}

   void tearDown()
   {}

   void testConstructor1D()
   {
      printMethod(TEST_FUNC);
      Mixture<1> mixture;
   }

   void testReadParameters1D()
   {
      printMethod(TEST_FUNC);
      Mixture<1> mixture;

      std::ifstream in;
      openInputFile("in/Mixture", in);
      mixture.readParam(in);
      in.close();
   }
 

   void testSolver1D()
   {
      printMethod(TEST_FUNC);
      Mixture<1> mixture;

      std::ifstream in;
      openInputFile("in/Mixture", in);
      mixture.readParam(in);
      UnitCell<1> unitCell;
      in >> unitCell;
      IntVec<1> d;
      in >> d;
      in.close();

      Mesh<1> mesh;
      mesh.setDimensions(d);
      mixture.setMesh(mesh);
      mixture.setupUnitCell(unitCell);

      std::cout << "\n";
      mixture.writeParam(std::cout);
      std::cout << "unitCell  " << unitCell << std::endl;
      std::cout << "mesh      " << mesh.dimensions() << std::endl;

      int nMonomer = mixture.nMonomer();
      DArray<Mixture<1>::WField> wFields;
      DArray<Mixture<1>::CField> cFields;
      wFields.allocate(nMonomer);
      cFields.allocate(nMonomer);
      double nx = (double)mesh.size();
      for (int i = 0; i < nMonomer; ++i) {
         wFields[i].allocate(nx);
         cFields[i].allocate(nx);
      }

      double cs;
      for (int i = 0; i < nx; ++i) {
         //cs = cos(2.0*Constants::Pi*(double(i)+0.5)/nx);
         //cs = cos(2.0*Constants::Pi*double(i)/double(nx-1));
         cs = cos(2.0*Constants::Pi*double(i)/double(nx));
         wFields[0][i] = 0.5 + cs;
         wFields[1][i] = 0.5 - cs;
      }

      mixture.compute(wFields, cFields);

      // Test if same Q is obtained from different methods
      std::cout << "Propagator(0,0), Q = " 
                << mixture.polymer(0).propagator(0, 0).computeQ() << "\n";
      std::cout << "Propagator(1,0), Q = " 
                << mixture.polymer(0).propagator(1, 0).computeQ() << "\n";
      std::cout << "Propagator(1,1), Q = " 
                << mixture.polymer(0).propagator(1, 1).computeQ() << "\n";
      std::cout << "Propagator(0,1), Q = " 
                << mixture.polymer(0).propagator(0, 1).computeQ() << "\n";

      #if 0
      // Test spatial integral of block concentration
      double sum0 = domain.spatialAverage(cFields[0]);
      double sum1 = domain.spatialAverage(cFields[1]);
      std::cout << "Volume fraction of block 0 = " << sum0 << "\n";
      std::cout << "Volume fraction of block 1 = " << sum1 << "\n";
      #endif
      
   }

   void testSolver2D()
   {
      printMethod(TEST_FUNC);
      Mixture<2> mixture;

      std::ifstream in;
      openInputFile("in/Mixture2d", in);
      mixture.readParam(in);
      UnitCell<2> unitCell;
      in >> unitCell;
      IntVec<2> d;
      in >> d;
      in.close();

      Mesh<2> mesh;
      mesh.setDimensions(d);
      mixture.setMesh(mesh);
      mixture.setupUnitCell(unitCell);

      std::cout << "\n";
      mixture.writeParam(std::cout);
      std::cout << "unitCell  " << unitCell << std::endl;
      std::cout << "mesh      " << mesh.dimensions() << std::endl;

      int nMonomer = mixture.nMonomer();
      DArray<Mixture<2>::WField> wFields;
      DArray<Mixture<2>::CField> cFields;
      wFields.allocate(nMonomer);
      cFields.allocate(nMonomer);
      double nx = (double)mesh.size();
      for (int i = 0; i < nMonomer; ++i) {
         wFields[i].allocate(nx);
         cFields[i].allocate(nx);
      }

      double cs;
      for (int i = 0; i < nx; ++i) {
         //cs = cos(2.0*Constants::Pi*(double(i)+0.5)/nx);
         //cs = cos(2.0*Constants::Pi*double(i)/double(nx-1));
         cs = cos(2.0*Constants::Pi*double(i)/double(nx));
         wFields[0][i] = 0.5 + cs;
         wFields[1][i] = 0.5 - cs;
      }

      mixture.compute(wFields, cFields);

      // Test if same Q is obtained from different methods
      std::cout << "Propagator(0,0), Q = " 
                << mixture.polymer(0).propagator(0, 0).computeQ() << "\n";
      std::cout << "Propagator(1,0), Q = " 
                << mixture.polymer(0).propagator(1, 0).computeQ() << "\n";
      std::cout << "Propagator(1,1), Q = " 
                << mixture.polymer(0).propagator(1, 1).computeQ() << "\n";
      std::cout << "Propagator(0,1), Q = " 
                << mixture.polymer(0).propagator(0, 1).computeQ() << "\n";

      #if 0
      // Test spatial integral of block concentration
      double sum0 = domain.spatialAverage(cFields[0]);
      double sum1 = domain.spatialAverage(cFields[1]);
      std::cout << "Volume fraction of block 0 = " << sum0 << "\n";
      std::cout << "Volume fraction of block 1 = " << sum1 << "\n";
      #endif
      
   }

   void testSolver3D()
   {
      printMethod(TEST_FUNC);
      Mixture<3> mixture;

      std::ifstream in;
      openInputFile("in/Mixture3d", in);
      mixture.readParam(in);
      UnitCell<3> unitCell;
      in >> unitCell;
      IntVec<3> d;
      in >> d;
      in.close();

      Mesh<3> mesh;
      mesh.setDimensions(d);
      mixture.setMesh(mesh);
      mixture.setupUnitCell(unitCell);

      std::cout << "\n";
      mixture.writeParam(std::cout);
      std::cout << "unitCell  " << unitCell << std::endl;
      std::cout << "mesh      " << mesh.dimensions() << std::endl;

      int nMonomer = mixture.nMonomer();
      DArray<Mixture<3>::WField> wFields;
      DArray<Mixture<3>::CField> cFields;
      wFields.allocate(nMonomer);
      cFields.allocate(nMonomer);
      double nx = (double)mesh.size();
      for (int i = 0; i < nMonomer; ++i) {
         wFields[i].allocate(nx);
         cFields[i].allocate(nx);
      }

      double cs;
      for (int i = 0; i < nx; ++i) {
         //cs = cos(2.0*Constants::Pi*(double(i)+0.5)/nx);
         //cs = cos(2.0*Constants::Pi*double(i)/double(nx-1));
         cs = cos(2.0*Constants::Pi*double(i)/double(nx));
         wFields[0][i] = 0.5 + cs;
         wFields[1][i] = 0.5 - cs;
      }

      mixture.compute(wFields, cFields);

      // Test if same Q is obtained from different methods
      std::cout << "Propagator(0,0), Q = " 
                << mixture.polymer(0).propagator(0, 0).computeQ() << "\n";
      std::cout << "Propagator(1,0), Q = " 
                << mixture.polymer(0).propagator(1, 0).computeQ() << "\n";
      std::cout << "Propagator(1,1), Q = " 
                << mixture.polymer(0).propagator(1, 1).computeQ() << "\n";
      std::cout << "Propagator(0,1), Q = " 
                << mixture.polymer(0).propagator(0, 1).computeQ() << "\n";

      #if 0
      // Test spatial integral of block concentration
      double sum0 = domain.spatialAverage(cFields[0]);
      double sum1 = domain.spatialAverage(cFields[1]);
      std::cout << "Volume fraction of block 0 = " << sum0 << "\n";
      std::cout << "Volume fraction of block 1 = " << sum1 << "\n";
      #endif
      
   }

};

TEST_BEGIN(MixtureTest)
TEST_ADD(MixtureTest, testConstructor1D)
TEST_ADD(MixtureTest, testReadParameters1D)
TEST_ADD(MixtureTest, testSolver1D)
TEST_ADD(MixtureTest, testSolver2D)
TEST_ADD(MixtureTest, testSolver3D)
TEST_END(MixtureTest)

#endif
