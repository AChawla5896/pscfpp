#ifndef PSSP_ITERATOR_TEST_H
#define PSSP_ITERATOR_TEST_H

#include <test/UnitTest.h>
#include <test/UnitTestRunner.h>

#include <pssp/iterator/AmIterator.h>
#include <pscf/mesh/MeshIterator.h>
#include <util/math/Constants.h>

#include <util/format/Dbl.h>

#include <pssp/System.h>

using namespace Util;
using namespace Pscf;
using namespace Pscf::Pssp;

class IteratorTest : public UnitTest
{

public:

	void setUp()
	{}

	void tearDown()
	{}

	void testConstructor1()
	{
		AmIterator<3> AmItr;

		System<3> sys;
	}

	void testReadParam()
	{
		//building system object
		System<3> sys;
		char* argv[3];
		argv[0] = (char *) "myName";
		argv[1] = (char *) "-p";
		argv[2] = (char *) "in/param";
		optind = 1;
		sys.setOptions(3, argv);
		sys.readParam();

		TEST_ASSERT(eq(sys.iterator().epsilon(), 1e-3));
		TEST_ASSERT(sys.iterator().maxHist() == 3);
		TEST_ASSERT(sys.iterator().maxItr() == 40);
	}

	void testAllocate()
	{
		System<3> sys;
		char* argv[5];

		argv[0] = (char*) "diffName";
		argv[1] = (char*) "-p";
		argv[2] = (char*) "in/param";
		argv[3] = (char*) "-c";
		argv[4] = (char*) "in/command";

		optind = 1;
		sys.setOptions(5, argv);
		sys.readParam();

		//consider this doen automatically in system.tpp
		sys.iterator().allocate();	
	}

	void testComputeDeviation()
	{
		System<3> sys;
		char* argv[5];

		argv[0] = (char*) "diffName";
		argv[1] = (char*) "-p";
		argv[2] = (char*) "in/param";
		argv[3] = (char*) "-c";
		argv[4] = (char*) "in/command";

		optind = 1;
		sys.setOptions(5, argv);
		sys.readParam();

		//set systemPtr_->wFields()
		MeshIterator<3> iter(sys.mesh().dimensions());
		double twoPi = 2.0*Constants::Pi;
		for (iter.begin(); !iter.atEnd(); ++iter){
         sys.wFieldGrid(0)[iter.rank()] = cos(twoPi * 
                        (double(iter.position(0))/double(sys.mesh().dimension(0)) + 
                         double(iter.position(1))/double(sys.mesh().dimension(1)) + 
                         double(iter.position(2))/double(sys.mesh().dimension(2)) ) );
         sys.wFieldGrid(1)[iter.rank()] = sin(twoPi * 
                        (double(iter.position(0))/double(sys.mesh().dimension(0)) + 
                         double(iter.position(1))/double(sys.mesh().dimension(1)) + 
                         double(iter.position(2))/double(sys.mesh().dimension(2)) ) );
      }
      for (int i = 0; i < sys.mixture().nMonomer(); i++) {
      	sys.fft().forwardTransform(sys.wFieldGrid(i), sys.wFieldDft(i));
      	sys.basis().convertFieldDftToComponents(sys.wFieldDft(i),sys.wField(i));
      }

		//set systemPtr_->cField();
		DArray<double> wTemp;
		DArray<double> cTemp;
		double sum1;
		wTemp.allocate(sys.mixture().nMonomer());
		cTemp.allocate(sys.mixture().nMonomer());
		for ( int i = 0; i < sys.basis().nStar(); ++i) {
			sum1 = 0;
			for (int j = 0; j < sys.mixture().nMonomer(); ++j) {
				wTemp[j] = sys.wField(j)[i];
				sum1 += sys.wField(j)[i];
			}

			sum1 /= sys.mixture().nMonomer();


			sys.cField(0)[i] = sys.interaction().chiInverse(1,0) * 
										(-sum1 + wTemp[1]);
		   sys.cField(1)[i] = sys.interaction().chiInverse(1,0) *
		   							(-sum1 + wTemp[0]);

		}
		
		//calculate deviation by hand();
		sys.iterator().allocate();	
		sys.iterator().computeDeviation();
		
		//print devHists_
      /*for(int i = 0; i < sys.mixture().nMonomer();i++){
         std::cout<<"THis is devfield of "<<i<<std::endl;
         for(int j = 0; j < sys.basis().nStar();j++){
            std::cout<<Dbl(sys.iterator().devHists_[0][i][j])<<std::endl;
         }
      }*/       
	}

	void testIsConverged1()
	{
		System<3> sys;
		char* argv[5];

		argv[0] = (char*) "diffName";
		argv[1] = (char*) "-p";
		argv[2] = (char*) "in/param";
		argv[3] = (char*) "-c";
		argv[4] = (char*) "in/command";

		optind = 1;
		sys.setOptions(5, argv);
		sys.readParam();

		//set systemPtr_->wFields()
		MeshIterator<3> iter(sys.mesh().dimensions());
		double twoPi = 2.0*Constants::Pi;
		for (iter.begin(); !iter.atEnd(); ++iter){
         sys.wFieldGrid(0)[iter.rank()] = cos(twoPi * 
                        (double(iter.position(0))/double(sys.mesh().dimension(0)) + 
                         double(iter.position(1))/double(sys.mesh().dimension(1)) + 
                         double(iter.position(2))/double(sys.mesh().dimension(2)) ) );
         sys.wFieldGrid(1)[iter.rank()] = sin(twoPi * 
                        (double(iter.position(0))/double(sys.mesh().dimension(0)) + 
                         double(iter.position(1))/double(sys.mesh().dimension(1)) + 
                         double(iter.position(2))/double(sys.mesh().dimension(2)) ) );
      }
      for (int i = 0; i < sys.mixture().nMonomer(); i++) {
      	sys.fft().forwardTransform(sys.wFieldGrid(i), sys.wFieldDft(i));
      	sys.basis().convertFieldDftToComponents(sys.wFieldDft(i),sys.wField(i));
      }

		//set systemPtr_->cField();
		DArray<double> wTemp;
		DArray<double> cTemp;
		double sum1;
		wTemp.allocate(sys.mixture().nMonomer());
		cTemp.allocate(sys.mixture().nMonomer());
		for ( int i = 0; i < sys.basis().nStar(); ++i) {
			sum1 = 0;
			for (int j = 0; j < sys.mixture().nMonomer(); ++j) {
				wTemp[j] = sys.wField(j)[i];
				sum1 += sys.wField(j)[i];
			}

			sum1 /= sys.mixture().nMonomer();


			sys.cField(0)[i] = sys.interaction().chiInverse(1,0) * 
										(-sum1 + wTemp[1]);
		   sys.cField(1)[i] = sys.interaction().chiInverse(1,0) *
		   							(-sum1 + wTemp[0]);

		}
		
		//calculate deviation
		sys.iterator().allocate();	
		sys.iterator().computeDeviation();
		
		//should be zero so converged automatically
		TEST_ASSERT(sys.iterator().isConverged());    
	}

	void testIsConverged2()
	{
		System<3> sys;
		char* argv[5];

		argv[0] = (char*) "diffName";
		argv[1] = (char*) "-p";
		argv[2] = (char*) "in/param";
		argv[3] = (char*) "-c";
		argv[4] = (char*) "in/command";

		optind = 1;
		sys.setOptions(5, argv);
		sys.readParam();

		//set systemPtr_->wFields()
		MeshIterator<3> iter(sys.mesh().dimensions());
		double twoPi = 2.0*Constants::Pi;
		for (iter.begin(); !iter.atEnd(); ++iter){
         sys.wFieldGrid(0)[iter.rank()] = cos(twoPi * 
                        (double(iter.position(0))/double(sys.mesh().dimension(0)) + 
                         double(iter.position(1))/double(sys.mesh().dimension(1)) + 
                         double(iter.position(2))/double(sys.mesh().dimension(2)) ) );
         sys.wFieldGrid(1)[iter.rank()] = sin(twoPi * 
                        (double(iter.position(0))/double(sys.mesh().dimension(0)) + 
                         double(iter.position(1))/double(sys.mesh().dimension(1)) + 
                         double(iter.position(2))/double(sys.mesh().dimension(2)) ) );
      }
      for (int i = 0; i < sys.mixture().nMonomer(); i++) {
      	sys.fft().forwardTransform(sys.wFieldGrid(i), sys.wFieldDft(i));
      	sys.basis().convertFieldDftToComponents(sys.wFieldDft(i),sys.wField(i));
      }

		//set systemPtr_->cField();
		DArray<double> wTemp;
		DArray<double> cTemp;
		double xi;
		wTemp.allocate(sys.mixture().nMonomer());
		cTemp.allocate(sys.mixture().nMonomer());
		for ( int i = 0; i < sys.basis().nStar(); ++i) {
			for (int j = 0; j < sys.mixture().nMonomer(); ++j) {
				wTemp[j] = sys.wField(j)[i];
			}

			sys.interaction().computeC(wTemp, cTemp, xi);

			for (int j = 0; j < sys.mixture().nMonomer(); ++j) {
				sys.cField(j)[i] = cTemp[j];
			}
		}
		
		//calculate deviation by hand();
		sys.iterator().allocate();	
		sys.iterator().computeDeviation();
		
		//dev is not zero. check calculation by hand
		//print stuff
		sys.iterator().isConverged();
	}

	void testSolve()
	{
		System<3> sys;
		char* argv[5];

		argv[0] = (char*) "diffName";
		argv[1] = (char*) "-p";
		argv[2] = (char*) "in/param";
		argv[3] = (char*) "-c";
		argv[4] = (char*) "in/command";

		optind = 1;
		sys.setOptions(5, argv);
		sys.readParam();

		//set systemPtr_->wFields()
		MeshIterator<3> iter(sys.mesh().dimensions());
		double twoPi = 2.0*Constants::Pi;
		for (iter.begin(); !iter.atEnd(); ++iter){
         sys.wFieldGrid(0)[iter.rank()] = cos(twoPi * 
                        (double(iter.position(0))/double(sys.mesh().dimension(0)) + 
                         double(iter.position(1))/double(sys.mesh().dimension(1)) + 
                         double(iter.position(2))/double(sys.mesh().dimension(2)) ) );
         sys.wFieldGrid(1)[iter.rank()] = sin(twoPi * 
                        (double(iter.position(0))/double(sys.mesh().dimension(0)) + 
                         double(iter.position(1))/double(sys.mesh().dimension(1)) + 
                         double(iter.position(2))/double(sys.mesh().dimension(2)) ) );
      }
      for (int i = 0; i < sys.mixture().nMonomer(); i++) {
      	sys.fft().forwardTransform(sys.wFieldGrid(i), sys.wFieldDft(i));
      	sys.basis().convertFieldDftToComponents(sys.wFieldDft(i),sys.wField(i));
      }

		sys.iterator().allocate();	
      int success = sys.iterator().solve();
      std::cout<<std::endl;
      std::cout<<"Iterator success? "<<success<<std::endl;
	}
};


TEST_BEGIN(IteratorTest)
TEST_ADD(IteratorTest, testConstructor1)
TEST_ADD(IteratorTest, testReadParam)
TEST_ADD(IteratorTest, testAllocate)
TEST_ADD(IteratorTest, testComputeDeviation)
TEST_ADD(IteratorTest, testIsConverged1)
//TEST_ADD(IteratorTest, testIsConverged2)
TEST_ADD(IteratorTest, testSolve)
TEST_END(IteratorTest)

#endif