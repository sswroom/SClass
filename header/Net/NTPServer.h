#ifndef _SM_NET_NTPSERVER
#define _SM_NET_NTPSERVER
#include "Data/Timestamp.h"
#include "Net/UDPServer.h"
#include "Sync/Event.h"
#include "Text/String.h"

namespace Net
{
	class NTPClient;
	class NTPServer
	{
	private:
		NotNullPtr<Net::SocketFactory> sockf;
		Net::UDPServer *svr;
		IO::LogTool *log;
		NotNullPtr<Text::String> timeServer;
		Int64 refTime;
		Int64 timeDiff;
		Bool threadRunning;
		Bool threadToStop;
		Sync::Event *evt;
		Net::NTPClient *cli;

		static void __stdcall PacketHdlr(const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, void *userData);
		static UInt32 __stdcall CheckThread(void *userObj);
		void InitServer(NotNullPtr<Net::SocketFactory> sockf, UInt16 port);
	public:
		NTPServer(NotNullPtr<Net::SocketFactory> sockf, UInt16 port, IO::LogTool *log, Text::CString timeServer);
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
