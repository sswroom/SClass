#ifndef _SM_TEST_TESTMODEM
#define _SM_TEST_TESTMODEM
#include "IO/Writer.h"
#include "IO/GSMModemController.h"

namespace Test
{
	class TestModem
	{
	public:
		static UOSInt ListPorts(IO::Writer *writer); //return default port
		static void GSMModemTest(IO::Writer *writer, IO::GSMModemController *modem, Bool quick);
	};
}
#endif
