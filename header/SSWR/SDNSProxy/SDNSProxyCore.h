#ifndef _SM_SSWR_SDNSPROXY_SDNSPROXYCORE
#define _SM_SSWR_SDNSPROXY_SDNSPROXYCORE
#include "Core/Core.h"
#include "Data/FastMapNN.h"
#include "IO/ConfigFile.h"
#include "IO/LogTool.h"
#include "IO/Writer.h"
#include "Net/DNSProxy.h"
#include "Net/SocketFactory.h"
#include "Net/TCPClientFactory.h"
#include "Net/WebServer/WebListener.h"
#include "SSWR/SDNSProxy/SDNSProxyWebHandler.h"
#include "Sync/Mutex.h"

namespace SSWR
{
	namespace SDNSProxy
	{
		class SDNSProxyCore
		{
		public:
			typedef struct
			{
				Int32 year;
				Int32 month;
				Int32 day;
				Int32 hour;
				Int64 reqCount;
			} HourInfo;

			struct ClientInfo
			{
				UInt32 cliId;
				Net::SocketUtil::AddressInfo addr;
				Sync::Mutex mut;
				Data::ArrayListNN<HourInfo> hourInfos;
			};

		private:
			IO::Writer *console;
			NN<Net::DNSProxy> proxy;
			NN<Net::SocketFactory> sockf;
			NN<Net::TCPClientFactory> clif;

			Sync::Mutex cliInfoMut;
			Data::FastMapNN<UInt32, ClientInfo> cliInfos;
			IO::LogTool log;
			Net::WebServer::WebListener *listener;
			SSWR::SDNSProxy::SDNSProxyWebHandler *hdlr;
			
			UInt8 lastHour;
			UInt8 lastMinute;
			UOSInt lastCnt;
			UOSInt currCnt;

			static void __stdcall OnDNSRequest(AnyType userObj, Text::CStringNN reqName, Int32 reqType, Int32 reqClass, NN<const Net::SocketUtil::AddressInfo> reqAddr, UInt16 reqPort, UInt32 reqId, Double timeUsed);

		public:
			SDNSProxyCore(NN<IO::ConfigFile> cfg, IO::Writer *console);
			~SDNSProxyCore();

			Bool IsError();
			void Run(NN<Core::ProgControl> progCtrl);

			UOSInt GetClientList(NN<Data::ArrayListNN<SSWR::SDNSProxy::SDNSProxyCore::ClientInfo>> cliList);
			UOSInt GetRequestPerMin();
		};
	}
}

#endif
