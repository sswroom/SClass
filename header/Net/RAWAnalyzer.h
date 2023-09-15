#ifndef _SM_NET_RAWANALYZER
#define _SM_NET_RAWANALYZER
#include "Net/EthernetAnalyzer.h"
#include "Net/SocketFactory.h"
#include "Net/WebServer/WebListener.h"
#include "Net/WebServer/WebStandardHandler.h"

namespace Net
{
	class RAWAnalyzer
	{
	private:
		NotNullPtr<Net::SocketFactory> sockf;
		Net::WebServer::WebListener *listener;
		NotNullPtr<Net::WebServer::WebStandardHandler> webHdlr;
		Socket *rawSock;
		Net::EthernetAnalyzer *analyzer;
	
		Int32 threadCnt;
		Bool threadToStop;

		static UInt32 __stdcall RecvThread(void *obj);

	public:
		RAWAnalyzer(NotNullPtr<Net::SocketFactory> sockf, UInt16 infoPort, IO::Writer *errWriter, Net::EthernetAnalyzer::AnalyzeType aType);
		~RAWAnalyzer();

		Bool IsError();

		Net::EthernetAnalyzer *GetAnalyzer();
	};
}
#endif
