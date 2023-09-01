#ifndef _SM_NET_PUSHMANAGER
#define _SM_NET_PUSHMANAGER
#include "Data/ArrayListNN.h"
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
			NotNullPtr<Text::String> token;
			Text::String *userName;
			DeviceType devType;
			Net::SocketUtil::AddressInfo subscribeAddr;
			Text::String *devModel;
			Data::Timestamp lastSubscribeTime;
		};

	private:

		struct UserInfo
		{
			NotNullPtr<Text::String> userName;
			Data::FastStringMap<DeviceInfo2*> devMap;
		};

		NotNullPtr<Net::SocketFactory> sockf;
		Net::SSLEngine *ssl;
		NotNullPtr<Text::String> fcmKey;
		IO::LogTool *log;

		Sync::Mutex dataMut;
		Bool loading;
		Data::FastStringMap<UserInfo*> userMap;
		Data::FastStringMap<DeviceInfo2*> devMap;

		UserInfo *GetUser(Text::CStringNN userName);
		void LoadData();
		void SaveData();
	public:
		PushManager(NotNullPtr<Net::SocketFactory> sockf, Net::SSLEngine *ssl, Text::CString fcmKey, IO::LogTool *log);
		~PushManager();

		Bool Subscribe(Text::CStringNN token, Text::CStringNN userName, DeviceType devType, NotNullPtr<const Net::SocketUtil::AddressInfo> remoteAddr, Text::CString devModel);
		Bool Unsubscribe(Text::CStringNN token);
		Bool Send(Data::ArrayListNN<Text::String> *userNames, NotNullPtr<Text::String> message);
		UOSInt GetUsers(Data::ArrayListNN<Text::String> *users, NotNullPtr<Sync::MutexUsage> mutUsage);
		NotNullPtr<const Data::ReadingList<DeviceInfo2*>> GetDevices(NotNullPtr<Sync::MutexUsage> mutUsage);
		void LogMessage(Text::CString msg, IO::LogHandler::LogLevel logLev);

		static Text::CString DeviceTypeGetName(DeviceType devType);
	};
}
#endif
