#ifndef _SM_NET_PUSHSERVER
#define _SM_NET_PUSHSERVER
#include "Net/GoogleFCM.h"
#include "Net/SocketFactory.h"
#include "Net/WebServer/WebListener.h"
#include "Sync/MutexUsage.h"

namespace Net
{
	class PushServerHandler;

	class PushServer
	{
	public:
		enum class DeviceType
		{
			Android,
			IOS
		};
	private:

		struct DeviceInfo
		{
			Text::String *token;
			Text::String *userName;
			DeviceType devType;
		};

		struct UserInfo
		{
			Text::String *userName;
			Data::FastStringMap<DeviceInfo*> devMap;
		};

		Net::SocketFactory *sockf;
		Net::SSLEngine *ssl;
		Net::WebServer::WebListener *listener;
		PushServerHandler *webHdlr;
		Text::String *fcmKey;
		IO::LogTool *log;

		Sync::Mutex dataMut;
		Data::FastStringMap<UserInfo*> userMap;
		Data::FastStringMap<DeviceInfo*> devMap;

		UserInfo *GetUser(Text::CString userName);
	public:
		PushServer(Net::SocketFactory *sockf, Net::SSLEngine *ssl, UInt16 port, Text::CString fcmKey, IO::LogTool *log);
		~PushServer();

		Bool IsError();

		Bool Subscribe(Text::CString token, Text::CString userName, DeviceType devType);
		Bool Unsubscribe(Text::CString token);
		Bool Send(Data::ArrayList<Text::String*> *userNames, Text::String *message);
		UOSInt GetUsers(Data::ArrayList<Text::String*> *users, Sync::MutexUsage *mutUsage);
	};
}
#endif
