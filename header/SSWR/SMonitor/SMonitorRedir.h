#ifndef _SM_SSWR_SMONITOR_SMONITORREDIR
#define _SM_SSWR_SMONITOR_SMONITORREDIR
#include "Crypto/Hash/CRC16.h"
#include "Net/UDPServer.h"
#include "SSWR/SMonitor/SMonitorCore.h"
#include "Sync/Mutex.h"

namespace SSWR
{
	namespace SMonitor
	{
		class SMonitorRedir
		{
		public:
			typedef void (CALLBACKFUNC RecordReplyHandler)(AnyType userObj, Int64 recTime, Int64 svrTime);
		private:
			NN<Net::SocketFactory> sockf;
			Net::UDPServer *svr;
			NN<Text::String> hostName;
			UInt16 port;
			Crypto::Hash::CRC16 dataCRC;
			Sync::Mutex dataCRCMut;
			RecordReplyHandler recReplyHdlr;
			AnyType recReplyObj;

			static void __stdcall OnDataUDPPacket(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::ByteArrayR data, AnyType userData);
			void CalcCRC(UnsafeArray<const UInt8> buff, UOSInt size, UnsafeArray<UInt8> crcVal);
		public:
			SMonitorRedir(NN<Net::SocketFactory> sockf, NN<IO::LogTool> log);
			SMonitorRedir(NN<Net::SocketFactory> sockf, NN<Text::String> hostName, UInt16 port, NN<IO::LogTool> log);
			~SMonitorRedir();

			Bool IsError();

			Bool SendDevReading(Int64 cliId, NN<const SSWR::SMonitor::SMonitorCore::DevRecord2> rec, Int32 reportInterval, Int32 kaInterval);
			Bool SendDevName(Int64 cliId, UnsafeArray<const UTF8Char> name, UOSInt nameLen);
			Bool SendDevPlatform(Int64 cliId, UnsafeArray<const UTF8Char> platform, UOSInt nameLen);
			Bool SendDevCPUName(Int64 cliId, UnsafeArray<const UTF8Char> cpuName, UOSInt nameLen);
			Bool SendDevReadingName(Int64 cliId, UOSInt index, UInt16 sensorId, UInt16 readingId, UnsafeArray<const UTF8Char> readingName, UOSInt nameLen);
			Bool SendDevVersion(Int64 cliId, Int64 progVersion);

			void HandleRecReply(RecordReplyHandler hdlr, AnyType userObj);
		};
	}
}
#endif
