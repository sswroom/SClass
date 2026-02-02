#ifndef _SM_SSWR_SERVERMONITOR_SERVERMONITORCORE
#define _SM_SSWR_SERVERMONITOR_SERVERMONITORCORE
#include "Data/FastMapNN.hpp"
#include "Data/FastStringMapNN.hpp"
#include "DB/DBConn.h"
#include "IO/ConsoleWriter.h"
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"
#include "Net/TCPClientFactory.h"
#include "Net/WebServer/WebListener.h"
#include "Net/WebServer/WebServiceHandler.h"

namespace SSWR
{
	namespace ServerMonitor
	{
		class ServerMonitorCore
		{
		public:
			enum class UserRole
			{
				Admin,
				User,
				NotLogged
			};

			struct UserInfo
			{
				NN<Text::String> username;
				NN<Text::String> passwordHash;
				UserRole role;
			};

			enum class ServerType
			{
				URL,
				TCP
			};

			struct ServerInfo
			{
				Int32 id;
				NN<Text::String> name;
				ServerType serverType;
				NN<Text::String> target;
				Int32 port;
				Int32 intervalMS;
				Int32 timeoutMS;
				Data::Timestamp lastCheck;
				Bool lastSuccess;
			};

			enum class AlertType
			{
				Email,
				WebPush
			};

			struct AlertInfo
			{
				Int32 id;
				AlertType type;
				NN<Text::String> settings;
				NN<Text::String> targets;
			};

		private:
			IO::ConsoleWriter console;
			NN<Net::SocketFactory> sockf;
			NN<Net::TCPClientFactory> clif;
			Optional<Net::SSLEngine> ssl;
			Optional<DB::DBConn> db;
			Optional<Net::WebServer::WebListener> listener;
			Optional<Net::WebServer::WebServiceHandler> webHdlr;
			Sync::Mutex mut;
			Data::FastStringMapNN<UserInfo> userMap;
			Data::Int32FastMapNN<ServerInfo> serverMap;
			Data::Int32FastMapNN<AlertInfo> alertMap;

			static void __stdcall FreeUserInfo(NN<UserInfo> userInfo);
			static void __stdcall FreeServerInfo(NN<ServerInfo> serverInfo);
			static void __stdcall FreeAlertInfo(NN<AlertInfo> alertInfo);
		public:
			ServerMonitorCore();
			~ServerMonitorCore();

			Bool IsError() const;
			Bool Run();

			UserRole Login(Text::CStringNN username, Text::CStringNN password);
			void GetServerList(NN<Data::ArrayListNN<ServerInfo>> serverList, NN<Sync::MutexUsage> mutUsage);

			static Text::CStringNN ServerTypeGetName(ServerType serverType);
		};
	}
}
#endif