#ifndef _SM_TEST_TESTMODEM
#define _SM_TEST_TESTMODEM
#include "IO/IWriter.h"
#include "IO/GSMModemController.h"

namespace Test
{
	class TestModem
	{
	public:
		static Int32 ListPorts(IO::IWriter *writer); //return default port
		static void GSMModemTest(IO::IWriter *writer, IO::GSMModemController *modem, Bool quick);
	};
};
#endif
