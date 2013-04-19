#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Common;

namespace Fast_kNCN_Test
{	

	TEST_CLASS(MemoryAllocationTest) {
	private:

		TEST_CLASS_INITIALIZE(MemoryAllocationTestSetUp) {	

		}


		TEST_CLASS_CLEANUP(MemoryAllocationTestTearDown) {

		}

	public:
		TEST_METHOD(AllocateAlignedMemoryTest) {
			SampleDim* dims = allocateSampleDimsMemory(32, __FILE__, __LINE__);

			for (int i = 0; i < 32; i++) {
				Assert::AreEqual(dims[i], 0.0f); 
			}

			freeSampleDimsMemory(dims, __FILE__, __LINE__);
		}

		TEST_METHOD(AllocateUnalignedMemoryTest) {
			SampleDim* dims = allocateSampleDimsMemory(44, __FILE__, __LINE__);

			for (int i = 0; i < 48; i++) {
				Assert::AreEqual(dims[i], 0.0f); 
			}

			freeSampleDimsMemory(dims, __FILE__, __LINE__);
		}
	};
}