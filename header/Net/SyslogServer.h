#ifndef _SM_NET_SYSLOGSERVER
#define _SM_NET_SYSLOGSERVER
#include "Data/FastMap.h"
#include "IO/FileStream.h"
#include "IO/ProtoHdlr/ProtoLogCliHandler.h"
#include "Net/SocketFactory.h"
#include "Net/UDPServer.h"
#include "Sync/Mutex.h"
#include "Text/String.h"

namespace Net
{
	class SyslogServer
	{
	public:
		typedef struct
		{
			UInt32 ip;
			IO::LogTool *log;
		} IPStatus;
		
		typedef void (__stdcall *ClientLogHandler)(AnyType userObj, UInt32 ip, Text::CStringNN logMessage);
	private:
		NN<Net::SocketFactory> sockf;
		Net::UDPServer *svr;
		NN<Text::String> logPath;
		NN<IO::LogTool> log;
		Bool redirLog;
		Sync::Mutex ipMut;
		Data::FastMap<UInt32, IPStatus*> ipMap;
		ClientLogHandler logHdlr;
		AnyType logHdlrObj;

		static void __stdcall OnUDPPacket(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::ByteArrayR data, AnyType userData);

		IPStatus *GetIPStatus(NN<const Net::SocketUtil::AddressInfo> addr);
	public:
		SyslogServer(NN<Net::SocketFactory> sockf, UInt16 port, Text::CStringNN logPath, NN<IO::LogTool> svrLog, Bool redirLog);
		~SyslogServer();

		Bool IsError();
		void HandleClientLog(ClientLogHandler hdlr, AnyType userObj);
	};
}
#endif