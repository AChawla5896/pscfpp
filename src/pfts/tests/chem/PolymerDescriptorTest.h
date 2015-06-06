#ifndef POLYMER_DESCRIPTOR_TEST_H
#define POLYMER_DESCRIPTOR_TEST_H

#include <test/UnitTest.h>
#include <test/UnitTestRunner.h>

#include <pfts/chem/PolymerDescriptor.h>
#include <pfts/chem/Vertex.h>
#include <pfts/chem/Block.h>

#include <fstream>

using namespace Pfts;
//using namespace Util;

class PolymerDescriptorTest : public UnitTest 
{

public:

   void setUp()
   {}

   void tearDown()
   {}

  
   void testConstructor()
   {
      printMethod(TEST_FUNC);
      PolymerDescriptor p;
   } 

   void testReadParam() {
      printMethod(TEST_FUNC);
      printEndl();

      std::ifstream in;
      openInputFile("in/PolymerDescriptor", in);

      PolymerDescriptor p;
      p.readParam(in);
      p.writeParam(std::cout);

      for (int i = 0; i < p.nVertex(); ++i) {
         std::cout << p.vertex(i).size() << "\n";
      }
      
   }

};

TEST_BEGIN(PolymerDescriptorTest)
TEST_ADD(PolymerDescriptorTest, testConstructor)
TEST_ADD(PolymerDescriptorTest, testReadParam)
TEST_END(PolymerDescriptorTest)

#endif
