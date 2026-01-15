#ifndef _SM_SSWR_SMONITOR_SMONITORCORE
#define _SM_SSWR_SMONITOR_SMONITORCORE
#include "Data/ArrayListNative.hpp"
#include "Data/ArrayListNN.hpp"
#include "Data/FastMapNN.hpp"
#include "IO/MemoryStream.h"
#include "Media/DrawEngine.h"
#include "Sync/RWMutex.h"
#define SMONITORCORE_DEVREADINGCNT 48
#define SMONITORCORE_DIGITALCNT 32

namespace SSWR
{
	namespace SMonitor
	{
		class SMonitorCore
		{
		public:
			typedef struct
			{
				Int32 userId;
				Int32 userType;
				Int32 loginId;
			} LoginInfo;

			typedef struct
			{
				UInt8 status[8]; //SensorId, SensorType, ReadingId, ReadingType
				Double reading;
			} ReadingInfo;

/*
	Profile ID:
	0 = unknown
	1 = ?
	2 = NB-IoT RSSI + SDM120M
	3 = NB-IoT RSSI + AMGU4241
	4 = NB-IoT RSSI + TH10SB
*/
			typedef struct
			{
				Int64 recTime;
				Int64 recvTime;
				UIntOS ndigital;
				UIntOS nreading;
				UIntOS nOutput;
				Int32 profileId;
				UInt32 digitalVals;
				ReadingInfo readings[SMONITORCORE_DEVREADINGCNT];
			} DevRecord2;

			struct DeviceInfo
			{
				Int64 cliId;
				NN<Text::String> platformName;
				NN<Text::String> cpuName;
				Int32 flags; //0: allow anonymous
				Int64 lastKATime;
				Sync::RWMutex mut;
				Optional<Text::String> devName;
				IO::Stream *stm;
				Net::SocketUtil::AddressInfo udpAddr;
				UInt16 udpPort;
				Int32 cliFlags;
				Int64 version;

				Int64 photoTime;
				UIntOS photoSize;
				UInt8 *photoBuff;
				UInt8 *photoBuffRecv;
				UIntOS photoPacketSize;
				Int32 photoSeq;
				UIntOS photoOfst;
				Int32 photoFmt;

				Int64 readingTime;
				UIntOS ndigital;
				UInt32 digitalVals;
				UIntOS nReading;
				UIntOS nOutput;
				ReadingInfo readings[SMONITORCORE_DEVREADINGCNT];
				UnsafeArrayOpt<const UTF8Char> readingNames[SMONITORCORE_DEVREADINGCNT];
				UnsafeArrayOpt<const UTF8Char> digitalNames[SMONITORCORE_DIGITALCNT];

				Data::ArrayListNN<DevRecord2> recToStore;
				Data::FastMapNN<Int64, DevRecord2> todayRecs;
				Data::FastMapNN<Int64, DevRecord2> yesterdayRecs;
				Data::FastMapNN<Int32, IO::MemoryStream> imgCaches;
				Bool valUpdated;
			};

			struct WebUser
			{
				Int32 userId;
				Int32 userType; //0 = anonymous, 1 = admin, 2 = user
				Sync::RWMutex mut;
				const UTF8Char *userName;
				UInt8 md5Pwd[16];
				Data::FastMapNN<Int64, DeviceInfo> devMap;
			};

		public:
			virtual NN<Media::DrawEngine> GetDrawEngine() = 0;

			virtual Optional<DeviceInfo> DeviceGet(Int64 cliId) = 0;
			virtual Bool DeviceModify(Int64 cliId, Text::CString devName, Int32 flags) = 0;
			virtual Bool DeviceSetReadings(NN<DeviceInfo> dev, UnsafeArrayOpt<const UTF8Char> readings) = 0;
			virtual Bool DeviceSetDigitals(NN<DeviceInfo> dev, UnsafeArrayOpt<const UTF8Char> digitals) = 0;
			virtual UIntOS DeviceQueryRec(Int64 cliId, Int64 startTime, Int64 endTime, NN<Data::ArrayListNN<DevRecord2>> recList) = 0;
			virtual Bool DeviceSetOutput(Int64 cliId, UInt32 outputNum, Bool toHigh) = 0;

			virtual Bool UserExist() = 0;
			virtual Bool UserAdd(UnsafeArray<const UTF8Char> userName, UnsafeArray<const UTF8Char> password, Int32 userType) = 0;
			virtual Bool UserSetPassword(Int32 userId, UnsafeArray<const UTF8Char> password) = 0;
			virtual Optional<LoginInfo> UserLogin(UnsafeArray<const UTF8Char> userName, UnsafeArray<const UTF8Char> password) = 0;
			virtual void UserFreeLogin(NN<LoginInfo> login) = 0;
			virtual UIntOS UserGetDevices(Int32 userId, Int32 userType, NN<Data::ArrayListNN<DeviceInfo>> devList) = 0;
			virtual Bool UserHasDevice(Int32 userId, Int32 userType, Int64 cliId) = 0;
			virtual UIntOS UserGetList(NN<Data::ArrayListNN<WebUser>> userList) = 0;
			virtual Optional<WebUser> UserGet(Int32 userId) = 0;
			virtual Bool UserAssign(Int32 userId, NN<Data::ArrayListNative<Int64>> devIdList) = 0;

			virtual Bool SendCapturePhoto(Int64 cliId) = 0;
		};
	}
}

#endif
