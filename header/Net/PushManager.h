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
			NN<Text::String> token;
			Text::String *userName;
			DeviceType devType;
			Net::SocketUtil::AddressInfo subscribeAddr;
			Text::String *devModel;
			Data::Timestamp lastSubscribeTime;
		};

	private:

		struct UserInfo
		{
			NN<Text::String> userName;
			Data::FastStringMapNN<DeviceInfo2> devMap;
		};

		NN<Net::SocketFactory> sockf;
		Optional<Net::SSLEngine> ssl;
		NN<Text::String> fcmKey;
		NN<IO::LogTool> log;

		Sync::Mutex dataMut;
		Bool loading;
		Data::FastStringMapNN<UserInfo> userMap;
		Data::FastStringMapNN<DeviceInfo2> devMap;

		NN<UserInfo> GetUser(Text::CStringNN userName);
		void LoadData();
		void SaveData();
	public:
		PushManager(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Text::CString fcmKey, NN<IO::LogTool> log);
		~PushManager();

		Bool Subscribe(Text::CStringNN token, Text::CStringNN userName, DeviceType devType, NN<const Net::SocketUtil::AddressInfo> remoteAddr, Text::CString devModel);
		Bool Unsubscribe(Text::CStringNN token);
		Bool Send(NN<Data::ArrayListStringNN> userNames, NN<Text::String> message);
		UOSInt GetUsers(NN<Data::ArrayListStringNN> users, NN<Sync::MutexUsage> mutUsage);
		NN<const Data::ReadingListNN<DeviceInfo2>> GetDevices(NN<Sync::MutexUsage> mutUsage);
		void LogMessage(Text::CStringNN msg, IO::LogHandler::LogLevel logLev);

		static Text::CStringNN DeviceTypeGetName(DeviceType devType);
	};
}
#endif
