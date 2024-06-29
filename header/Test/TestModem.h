#ifndef _SM_TEST_TESTMODEM
#define _SM_TEST_TESTMODEM
#include "IO/Writer.h"
#include "IO/GSMModemController.h"

namespace Test
{
	class TestModem
	{
	public:
		static UOSInt ListPorts(NN<IO::Writer> writer); //return default port
		static void GSMModemTest(NN<IO::Writer> writer, NN<IO::GSMModemController> modem, Bool quick);
	};
}
#endif
