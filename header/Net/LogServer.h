#ifndef _SM_NET_LOGSERVER
#define _SM_NET_LOGSERVER
#include "Data/UInt32Map.h"
#include "IO/FileStream.h"
#include "IO/ProtoHdlr/ProtoLogCliHandler.h"
#include "Net/SocketFactory.h"
#include "Net/TCPServer.h"
#include "Net/TCPClientMgr.h"
#include "Sync/Mutex.h"
#include "Text/MyString.h"

namespace Net
{
	class LogServer : public IO::IProtocolHandler::DataListener
	{
	public:
		typedef struct
		{
			UInt32 ip;
			IO::LogTool *log;
		} IPStatus;
		
		typedef struct
		{
			UInt8 *buff;
			UOSInt buffSize;
			IPStatus *status;
		} ClientStatus;

		typedef void (__stdcall *ClientLogHandler)(void *userObj, UInt32 ip, const UTF8Char* logMessage);
	private:
		Net::SocketFactory *sockf;
		Net::TCPServer *svr;
		Net::TCPClientMgr *cliMgr;
		Text::String *logPath;
		IO::ProtoHdlr::ProtoLogCliHandler *protoHdlr;
		IO::LogTool *log;
		Bool redirLog;
		Sync::Mutex *ipMut;
		Data::UInt32Map<IPStatus*> *ipMap;
		ClientLogHandler logHdlr;
		void *logHdlrObj;

		static void __stdcall ConnHdlr(Socket *s, void *userObj);
		static void __stdcall ClientEvent(Net::TCPClient *cli, void *userObj, void *cliData, Net::TCPClientMgr::TCPEventType evtType);
		static void __stdcall ClientData(Net::TCPClient *cli, void *userObj, void *cliData, const UInt8 *buff, UOSInt size);
		static void __stdcall ClientTimeout(Net::TCPClient *cli, void *userObj, void *cliData);

		IPStatus *GetIPStatus(const Net::SocketUtil::AddressInfo *addr);
	public:
		LogServer(Net::SocketFactory *sockf, UInt16 port, Text::CString logPath, IO::LogTool *svrLog, Bool redirLog);
		virtual ~LogServer();

		Bool IsError();
		void HandleClientLog(ClientLogHandler hdlr, void *userObj);

		virtual void DataParsed(IO::Stream *stm, void *stmObj, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize);
		virtual void DataSkipped(IO::Stream *stm, void *stmObj, const UInt8 *buff, UOSInt buffSize);
	};
}
#endif