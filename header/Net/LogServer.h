#ifndef _SM_NET_LOGSERVER
#define _SM_NET_LOGSERVER
#include "AnyType.h"
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

		typedef void (__stdcall *ClientLogHandler)(AnyType userObj, UInt32 ip, Text::CString logMessage);
	private:
		NotNullPtr<Net::SocketFactory> sockf;
		Net::TCPServer *svr;
		Net::TCPClientMgr *cliMgr;
		NotNullPtr<Text::String> logPath;
		IO::ProtoHdlr::ProtoLogCliHandler protoHdlr;
		NotNullPtr<IO::LogTool> log;
		Bool redirLog;
		Sync::Mutex ipMut;
		Data::FastMap<UInt32, IPStatus*> ipMap;
		ClientLogHandler logHdlr;
		AnyType logHdlrObj;

		static void __stdcall ConnHdlr(Socket *s, AnyType userObj);
		static void __stdcall ClientEvent(NotNullPtr<Net::TCPClient> cli, AnyType userObj, AnyType cliData, Net::TCPClientMgr::TCPEventType evtType);
		static void __stdcall ClientData(NotNullPtr<Net::TCPClient> cli, AnyType userObj, AnyType cliData, const Data::ByteArrayR &buff);
		static void __stdcall ClientTimeout(NotNullPtr<Net::TCPClient> cli, AnyType userObj, AnyType cliData);

		IPStatus *GetIPStatus(NotNullPtr<const Net::SocketUtil::AddressInfo> addr);
	public:
		LogServer(NotNullPtr<Net::SocketFactory> sockf, UInt16 port, Text::CString logPath, NotNullPtr<IO::LogTool> svrLog, Bool redirLog, Bool autoStart);
		virtual ~LogServer();

		Bool Start();
		Bool IsError();
		void HandleClientLog(ClientLogHandler hdlr, AnyType userObj);

		virtual void DataParsed(NotNullPtr<IO::Stream> stm, AnyType stmObj, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize);
		virtual void DataSkipped(NotNullPtr<IO::Stream> stm, AnyType stmObj, const UInt8 *buff, UOSInt buffSize);
	};
}
#endif