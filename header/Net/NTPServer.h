#ifndef _SM_NET_NTPSERVER
#define _SM_NET_NTPSERVER
#include "Data/Timestamp.h"
#include "Net/UDPServer.h"
#include "Sync/Thread.h"
#include "Text/String.h"

namespace Net
{
	class NTPClient;
	class NTPServer
	{
	private:
		NN<Net::SocketFactory> sockf;
		Net::UDPServer *svr;
		NN<IO::LogTool> log;
		NN<Text::String> timeServerHost;
		Int64 refTime;
		Int64 timeDiff;
		Sync::Thread thread;
		Net::NTPClient *cli;

		static void __stdcall PacketHdlr(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::ByteArrayR data, AnyType userData);
		static void __stdcall CheckThread(NN<Sync::Thread> thread);
		void InitServer(NN<Net::SocketFactory> sockf, UInt16 port);
	public:
		NTPServer(NN<Net::SocketFactory> sockf, UInt16 port, NN<IO::LogTool> log, Text::CStringNN timeServerHost);
		~NTPServer();

		Bool IsError();

		static void ReadTime(const UInt8 *buff, Data::DateTime *time);
		static Data::Timestamp ReadTime(const UInt8 *buff);
		static void WriteTime(UInt8 *buff, Data::DateTime *time);
		static void WriteTime(UInt8 *buff, const Data::Timestamp &time);
		static void WriteTime(UInt8 *buff, Int64 timeTicks);
	};
}
#endif
