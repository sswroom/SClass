#ifndef _SM_SSWR_SERVERMONITOR_SERVERMONITORDATA
#define _SM_SSWR_SERVERMONITOR_SERVERMONITORDATA
#include "Data/Timestamp.h"
#include "SSWR/ServerMonitor/ServerMonitorClient.h"
#include "Text/String.h"

namespace SSWR
{
	namespace ServerMonitor
	{
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
			Int32 intervalMS;
			Int32 timeoutMS;
			Data::Timestamp lastCheck;
			Bool lastSuccess;
			Optional<ServerMonitorClient> client;
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

		Text::CStringNN ServerTypeGetName(ServerType serverType);
	}
}
#endif
