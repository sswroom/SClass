#ifndef _SM_NET_RAWANALYZER
#define _SM_NET_RAWANALYZER
#include "AnyType.h"
#include "Net/EthernetAnalyzer.h"
#include "Net/SocketFactory.h"
#include "Net/WebServer/WebListener.h"
#include "Net/WebServer/WebStandardHandler.h"

namespace Net
{
	class RAWAnalyzer
	{
	private:
		NN<Net::TCPClientFactory> clif;
		Optional<Net::WebServer::WebListener> listener;
		NN<Net::WebServer::WebStandardHandler> webHdlr;
		Optional<Socket> rawSock;
		NN<Net::EthernetAnalyzer> analyzer;
	
		Int32 threadCnt;
		Bool threadToStop;

		static UInt32 __stdcall RecvThread(AnyType obj);

	public:
		RAWAnalyzer(NN<Net::TCPClientFactory> clif, UInt16 infoPort, Optional<IO::Writer> errWriter, Net::EthernetAnalyzer::AnalyzeType aType);
		~RAWAnalyzer();

		Bool IsError();

		NN<Net::EthernetAnalyzer> GetAnalyzer();
	};
}
#endif
