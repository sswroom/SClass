#ifndef _SM_NET_LOGSERVER
#define _SM_NET_LOGSERVER
#include "Data/FastMap.h"
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

		typedef void (__stdcall *ClientLogHandler)(void *userObj, UInt32 ip, Text::CString logMessage);
	private:
		Net::SocketFactory *sockf;
		Net::TCPServer *svr;
		Net::TCPClientMgr *cliMgr;
		NotNullPtr<Text::String> logPath;
		IO::ProtoHdlr::ProtoLogCliHandler protoHdlr;
		IO::LogTool *log;
		Bool redirLog;
		Sync::Mutex ipMut;
		Data::FastMap<UInt32, IPStatus*> ipMap;
		ClientLogHandler logHdlr;
		void *logHdlrObj;

		static void __stdcall ConnHdlr(Socket *s, void *userObj);
		static void __stdcall ClientEvent(NotNullPtr<Net::TCPClient> cli, void *userObj, void *cliData, Net::TCPClientMgr::TCPEventType evtType);
		static void __stdcall ClientData(NotNullPtr<Net::TCPClient> cli, void *userObj, void *cliData, const UInt8 *buff, UOSInt size);
		static void __stdcall ClientTimeout(NotNullPtr<Net::TCPClient> cli, void *userObj, void *cliData);

		IPStatus *GetIPStatus(const Net::SocketUtil::AddressInfo *addr);
	public:
		LogServer(Net::SocketFactory *sockf, UInt16 port, Text::CString logPath, IO::LogTool *svrLog, Bool redirLog, Bool autoStart);
		virtual ~LogServer();

		Bool Start();
		Bool IsError();
		void HandleClientLog(ClientLogHandler hdlr, void *userObj);

		virtual void DataParsed(NotNullPtr<IO::Stream> stm, void *stmObj, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize);
		virtual void DataSkipped(NotNullPtr<IO::Stream> stm, void *stmObj, const UInt8 *buff, UOSInt buffSize);
	};
}
#endif