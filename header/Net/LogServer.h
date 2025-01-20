#ifndef _SM_NET_LOGSERVER
#define _SM_NET_LOGSERVER
#include "AnyType.h"
#include "Data/FastMapNN.h"
#include "IO/FileStream.h"
#include "IO/ProtoHdlr/ProtoLogCliHandler.h"
#include "Net/SocketFactory.h"
#include "Net/TCPServer.h"
#include "Net/TCPClientMgr.h"
#include "Sync/Mutex.h"
#include "Text/MyString.h"

namespace Net
{
	class LogServer : public IO::ProtocolHandler::DataListener
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
			Optional<IPStatus> status;
		} ClientStatus;

		typedef void (CALLBACKFUNC ClientLogHandler)(AnyType userObj, UInt32 ip, Text::CStringNN logMessage);
	private:
		NN<Net::SocketFactory> sockf;
		Net::TCPServer *svr;
		Net::TCPClientMgr *cliMgr;
		NN<Text::String> logPath;
		IO::ProtoHdlr::ProtoLogCliHandler protoHdlr;
		NN<IO::LogTool> log;
		Bool redirLog;
		Sync::Mutex ipMut;
		Data::FastMapNN<UInt32, IPStatus> ipMap;
		ClientLogHandler logHdlr;
		AnyType logHdlrObj;

		static void __stdcall ConnHdlr(NN<Socket> s, AnyType userObj);
		static void __stdcall ClientEvent(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData, Net::TCPClientMgr::TCPEventType evtType);
		static void __stdcall ClientData(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData, const Data::ByteArrayR &buff);
		static void __stdcall ClientTimeout(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData);

		Optional<IPStatus> GetIPStatus(NN<const Net::SocketUtil::AddressInfo> addr);
	public:
		LogServer(NN<Net::SocketFactory> sockf, UInt16 port, Text::CStringNN logPath, NN<IO::LogTool> svrLog, Bool redirLog, Bool autoStart);
		virtual ~LogServer();

		Bool Start();
		Bool IsError();
		void HandleClientLog(ClientLogHandler hdlr, AnyType userObj);

		virtual void DataParsed(NN<IO::Stream> stm, AnyType stmObj, Int32 cmdType, Int32 seqId, UnsafeArray<const UInt8> cmd, UOSInt cmdSize);
		virtual void DataSkipped(NN<IO::Stream> stm, AnyType stmObj, UnsafeArray<const UInt8> buff, UOSInt buffSize);
	};
}
#endif