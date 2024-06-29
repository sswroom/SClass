#ifndef _SM_TEST_TESTSIM7000
#define _SM_TEST_TESTSIM7000
#include "IO/Writer.h"
#include "IO/Device/SIM7000.h"

namespace Test
{
	class TestSIM7000
	{
	public:
		static void SIM7000Info(NN<IO::Writer> writer, NN<IO::Device::SIM7000> modem);
	};
}
#endif
