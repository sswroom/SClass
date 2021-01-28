#ifndef _SM_SSWR_SDNSPROXY_SDNSPROXYCORE
#define _SM_SSWR_SDNSPROXY_SDNSPROXYCORE
#include "Core/Core.h"
#include "Data/Integer32Map.h"
#include "IO/ConfigFile.h"
#include "IO/IWriter.h"
#include "IO/LogTool.h"
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
				Int32 cliId;
				Net::SocketUtil::AddressInfo addr;
				Sync::Mutex *mut;
				Data::ArrayList<HourInfo*> *hourInfos;
			} ClientInfo;

		private:
			IO::IWriter *console;
			Net::DNSProxy *proxy;
			Net::SocketFactory *sockf;

			Sync::Mutex *cliInfoMut;
			Data::Integer32Map<ClientInfo*> *cliInfos;
			IO::LogTool *log;
			Net::WebServer::WebListener *listener;
			SSWR::SDNSProxy::SDNSProxyWebHandler *hdlr;
			
			UInt8 lastHour;
			UInt8 lastMinute;
			OSInt lastCnt;
			OSInt currCnt;

			static void __stdcall OnDNSRequest(void *userObj, const UTF8Char *reqName, Int32 reqType, Int32 reqClass, const Net::SocketUtil::AddressInfo *reqAddr, UInt16 reqPort, Int32 reqId, Double timeUsed);

		public:
			SDNSProxyCore(IO::ConfigFile *cfg, IO::IWriter *console);
			~SDNSProxyCore();

			Bool IsError();
			void Run(Core::IProgControl *progCtrl);

			OSInt GetClientList(Data::ArrayList<SSWR::SDNSProxy::SDNSProxyCore::ClientInfo *> *cliList);
			OSInt GetRequestPerMin();
		};
	}
}

#endif
