#ifndef _SM_SSWR_SMONITOR_SMONITORREDIR
#define _SM_SSWR_SMONITOR_SMONITORREDIR
#include "Crypto/Hash/CRC16.h"
#include "Net/UDPServer.h"
#include "SSWR/SMonitor/ISMonitorCore.h"
#include "Sync/Mutex.h"

namespace SSWR
{
	namespace SMonitor
	{
		class SMonitorRedir
		{
		public:
			typedef void (__stdcall *RecordReplyHandler)(void *userObj, Int64 recTime, Int64 svrTime);
		private:
			NotNullPtr<Net::SocketFactory> sockf;
			Net::UDPServer *svr;
			NotNullPtr<Text::String> hostName;
			UInt16 port;
			Crypto::Hash::CRC16 dataCRC;
			Sync::Mutex dataCRCMut;
			RecordReplyHandler recReplyHdlr;
			void *recReplyObj;

			static void __stdcall OnDataUDPPacket(const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, void *userData);
			void CalcCRC(const UInt8 *buff, UOSInt size, UInt8 *crcVal);
		public:
			SMonitorRedir(NotNullPtr<Net::SocketFactory> sockf);
			SMonitorRedir(NotNullPtr<Net::SocketFactory> sockf, NotNullPtr<Text::String> hostName, UInt16 port);
			~SMonitorRedir();

			Bool IsError();

			Bool SendDevReading(Int64 cliId, const SSWR::SMonitor::ISMonitorCore::DevRecord2 *rec, Int32 reportInterval, Int32 kaInterval);
			Bool SendDevName(Int64 cliId, const UTF8Char *name, UOSInt nameLen);
			Bool SendDevPlatform(Int64 cliId, const UTF8Char *platform, UOSInt nameLen);
			Bool SendDevCPUName(Int64 cliId, const UTF8Char *cpuName, UOSInt nameLen);
			Bool SendDevReadingName(Int64 cliId, UOSInt index, UInt16 sensorId, UInt16 readingId, const UTF8Char *readingName, UOSInt nameLen);
			Bool SendDevVersion(Int64 cliId, Int64 progVersion);

			void HandleRecReply(RecordReplyHandler hdlr, void *userObj);
		};
	}
}
#endif
