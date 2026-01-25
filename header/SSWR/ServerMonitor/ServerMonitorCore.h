#ifndef _SM_SSWR_SERVERMONITOR_SERVERMONITORCORE
#define _SM_SSWR_SERVERMONITOR_SERVERMONITORCORE
#include "DB/DBConn.h"
#include "IO/ConsoleWriter.h"
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"
#include "Net/TCPClientFactory.h"

namespace SSWR
{
	namespace ServerMonitor
	{
		class ServerMonitorCore
		{
		private:
			IO::ConsoleWriter console;
			NN<Net::SocketFactory> sockf;
			NN<Net::TCPClientFactory> clif;
			Optional<Net::SSLEngine> ssl;
			Optional<DB::DBConn> db;

		public:
			ServerMonitorCore();
			~ServerMonitorCore();

			Bool IsError() const;
			Bool Run();
		};
	}
}
#endif