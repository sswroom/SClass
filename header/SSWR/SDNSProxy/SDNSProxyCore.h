#ifndef _SM_SSWR_SDNSPROXY_SDNSPROXYCORE
#define _SM_SSWR_SDNSPROXY_SDNSPROXYCORE
#include "Core/Core.h"
#include "Data/UInt32Map.h"
#include "IO/ConfigFile.h"
#include "IO/LogTool.h"
#include "IO/Writer.h"
#include "Net/DNSProxy.h"
#include "Net/SocketFactory.h"
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

			typedef struct
			{
				UInt32 cliId;
				Net::SocketUtil::AddressInfo addr;
				Sync::Mutex *mut;
				Data::ArrayList<HourInfo*> *hourInfos;
			} ClientInfo;

		private:
			IO::Writer *console;
			Net::DNSProxy *proxy;
			Net::SocketFactory *sockf;

			Sync::Mutex *cliInfoMut;
			Data::UInt32Map<ClientInfo*> *cliInfos;
			IO::LogTool *log;
			Net::WebServer::WebListener *listener;
			SSWR::SDNSProxy::SDNSProxyWebHandler *hdlr;
			
			UInt8 lastHour;
			UInt8 lastMinute;
			UOSInt lastCnt;
			UOSInt currCnt;

			static void __stdcall OnDNSRequest(void *userObj, Text::CString reqName, Int32 reqType, Int32 reqClass, const Net::SocketUtil::AddressInfo *reqAddr, UInt16 reqPort, UInt32 reqId, Double timeUsed);

		public:
			SDNSProxyCore(IO::ConfigFile *cfg, IO::Writer *console);
			~SDNSProxyCore();

			Bool IsError();
			void Run(Core::IProgControl *progCtrl);

			UOSInt GetClientList(Data::ArrayList<SSWR::SDNSProxy::SDNSProxyCore::ClientInfo *> *cliList);
			UOSInt GetRequestPerMin();
		};
	}
}

#endif
