#ifndef _SM_SSWR_SERVERMONITOR_SERVERMONITORCORE
#define _SM_SSWR_SERVERMONITOR_SERVERMONITORCORE
#include "Data/FastMapNN.hpp"
#include "Data/FastStringMapNN.hpp"
#include "DB/DBTool.h"
#include "IO/ConsoleWriter.h"
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"
#include "Net/TCPClientFactory.h"
#include "Net/Email/SMTPConn.h"
#include "Net/WebServer/WebListener.h"
#include "Net/WebServer/WebServiceHandler.h"
#include "SSWR/ServerMonitor/ServerMonitorClient.h"
#include "SSWR/ServerMonitor/ServerMonitorData.h"
#include "Sync/Thread.h"

namespace SSWR
{
	namespace ServerMonitor
	{
		class ServerMonitorCore
		{
		private:
			IO::ConsoleWriter console;
			IO::LogTool log;
			NN<Net::SocketFactory> sockf;
			NN<Net::TCPClientFactory> clif;
			Optional<Net::SSLEngine> ssl;
			Optional<DB::DBTool> db;
			Optional<Net::WebServer::WebListener> listener;
			Optional<Net::WebServer::WebServiceHandler> webHdlr;
			Sync::Mutex mut;
			Data::FastStringMapNN<UserInfo> userMap;
			Data::Int32FastMapNN<ServerInfo> serverMap;
			Data::Int32FastMapNN<AlertInfo> alertMap;
			Sync::Thread checkThread;

			static void __stdcall CheckThread(NN<Sync::Thread> thread);
			static void __stdcall FreeUserInfo(NN<UserInfo> userInfo);
			static void __stdcall FreeServerInfo(NN<ServerInfo> serverInfo);
			static void __stdcall FreeAlertInfo(NN<AlertInfo> alertInfo);
			void SendAlerts(Text::CStringNN serverName);
		public:
			ServerMonitorCore();
			~ServerMonitorCore();

			Bool IsError() const;
			Bool Run();

			UserRole Login(Text::CStringNN username, Text::CStringNN password);
			void GetServerList(NN<Data::ArrayListNN<ServerInfo>> serverList, NN<Sync::MutexUsage> mutUsage);
			void GetAlertList(NN<Data::ArrayListNN<AlertInfo>> alertList, NN<Sync::MutexUsage> mutUsage);
			Optional<ServerInfo> AddServerURL(Text::CStringNN name, Text::CStringNN url, Text::CString containsText, Int32 timeoutMS);
			Optional<AlertInfo> AddAlertSMTP(Text::CStringNN host, UInt16 port, Net::Email::SMTPConn::ConnType connType, Text::CStringNN smtpUser, Text::CStringNN smtpPassword, Text::CStringNN fromEmail, Text::CStringNN toEmails);
		};
	}
}
#endif