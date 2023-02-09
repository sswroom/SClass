#ifndef _SM_NET_PUSHMANAGER
#define _SM_NET_PUSHMANAGER
#include "Data/FastStringMap.h"
#include "IO/LogFile.h"
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"
#include "Sync/MutexUsage.h"

namespace Net
{
	class PushManager
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
		Text::String *fcmKey;
		IO::LogTool *log;

		Sync::Mutex dataMut;
		Data::FastStringMap<UserInfo*> userMap;
		Data::FastStringMap<DeviceInfo*> devMap;

		UserInfo *GetUser(Text::CString userName);
		void LoadData();
		void SaveData();
	public:
		PushManager(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::CString fcmKey, IO::LogTool *log);
		~PushManager();

		Bool Subscribe(Text::CString token, Text::CString userName, DeviceType devType);
		Bool Unsubscribe(Text::CString token);
		Bool Send(Data::ArrayList<Text::String*> *userNames, Text::String *message);
		UOSInt GetUsers(Data::ArrayList<Text::String*> *users, Sync::MutexUsage *mutUsage);
	};
}
#endif
