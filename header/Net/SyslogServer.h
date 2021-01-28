#ifndef _SM_NET_SYSLOGSERVER
#define _SM_NET_SYSLOGSERVER
#include "Data/Integer32Map.h"
#include "IO/FileStream.h"
#include "IO/ProtoHdlr/ProtoLogCliHandler.h"
#include "Net/SocketFactory.h"
#include "Net/UDPServer.h"
#include "Sync/Mutex.h"
#include "Text/MyString.h"

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
		
		typedef void (__stdcall *ClientLogHandler)(void *userObj, UInt32 ip, const UTF8Char* logMessage);
	private:
		Net::SocketFactory *sockf;
		Net::UDPServer *svr;
		const UTF8Char *logPath;
		IO::LogTool *log;
		Bool redirLog;
		Sync::Mutex *ipMut;
		Data::Integer32Map<IPStatus*> *ipMap;
		ClientLogHandler logHdlr;
		void *logHdlrObj;

		static void __stdcall OnUDPPacket(const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, void *userData);

		IPStatus *GetIPStatus(const Net::SocketUtil::AddressInfo *addr);
	public:
		SyslogServer(Net::SocketFactory *sockf, UInt16 port, const UTF8Char *logPath, IO::LogTool *svrLog, Bool redirLog);
		~SyslogServer();

		Bool IsError();
		void HandleClientLog(ClientLogHandler hdlr, void *userObj);
	};
}
#endif