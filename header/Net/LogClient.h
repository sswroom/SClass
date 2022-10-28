#ifndef _SM_NET_LOGCLIENT
#define _SM_NET_LOGCLIENT
#include "Data/ArrayListInt64.h"
#include "Data/ArrayListString.h"
#include "IO/LogTool.h"
#include "IO/ProtoHdlr/ProtoLogCliHandler.h"
#include "Net/SocketFactory.h"
#include "Net/TCPClient.h"
#include "Sync/Mutex.h"
#include "Sync/Event.h"
#include "Text/UTF8Writer.h"

namespace Net
{
	class LogClient : public IO::ILogHandler, public IO::IProtocolHandler::DataListener
	{
	private:
		IO::ProtoHdlr::ProtoLogCliHandler protoHdlr;
		Net::SocketFactory *sockf;
		Net::SocketUtil::AddressInfo addr;
		UInt16 port;

		Sync::Mutex mut;
		Data::ArrayList<Data::Timestamp> dateList;
		Data::ArrayListString msgList;

		Int64 lastSendTime;
		Sync::Mutex cliMut;
		Net::TCPClient *cli;

		Bool sendRunning;
		Bool sendToStop;
		Sync::Event sendEvt;
		Bool recvRunning;
		Bool recvToStop;
		Sync::Event recvEvt;

		static UInt32 __stdcall RecvThread(void *userObj);
		static UInt32 __stdcall SendThread(void *userObj);
	public:
		LogClient(Net::SocketFactory *sockf, const Net::SocketUtil::AddressInfo *addr, UInt16 port);
		virtual ~LogClient();
		virtual void LogClosed();
		virtual void LogAdded(Data::Timestamp logTime, Text::CString logMsg, LogLevel logLev);

		virtual void DataParsed(IO::Stream *stm, void *stmObj, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize);
		virtual void DataSkipped(IO::Stream *stm, void *stmObj, const UInt8 *buff, UOSInt buffSize);
	};
}
#endif
