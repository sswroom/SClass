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

		struct DeviceInfo2
		{
			Text::String *token;
			Text::String *userName;
			DeviceType devType;
			Net::SocketUtil::AddressInfo subscribeAddr;
			Text::String *devModel;
			Data::Timestamp lastSubscribeTime;
		};

	private:

		struct UserInfo
		{
			Text::String *userName;
			Data::FastStringMap<DeviceInfo2*> devMap;
		};

		Net::SocketFactory *sockf;
		Net::SSLEngine *ssl;
		Text::String *fcmKey;
		IO::LogTool *log;

		Sync::Mutex dataMut;
		Bool loading;
		Data::FastStringMap<UserInfo*> userMap;
		Data::FastStringMap<DeviceInfo2*> devMap;

		UserInfo *GetUser(Text::CString userName);
		void LoadData();
		void SaveData();
	public:
		PushManager(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::CString fcmKey, IO::LogTool *log);
		~PushManager();

		Bool Subscribe(Text::CString token, Text::CString userName, DeviceType devType, const Net::SocketUtil::AddressInfo *remoteAddr, Text::CString devModel);
		Bool Unsubscribe(Text::CString token);
		Bool Send(Data::ArrayList<Text::String*> *userNames, Text::String *message);
		UOSInt GetUsers(Data::ArrayList<Text::String*> *users, Sync::MutexUsage *mutUsage);
		const Data::ReadingList<DeviceInfo2*> *GetDevices(Sync::MutexUsage *mutUsage);
		void LogMessage(Text::CString msg, IO::ILogHandler::LogLevel logLev);

		static Text::CString DeviceTypeGetName(DeviceType devType);
	};
}
#endif
