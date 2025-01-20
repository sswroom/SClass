#ifndef _SM_NET_LOGCLIENT
#define _SM_NET_LOGCLIENT
#include "AnyType.h"
#include "Data/ArrayListInt64.h"
#include "Data/ArrayListStringNN.h"
#include "IO/LogTool.h"
#include "IO/ProtoHdlr/ProtoLogCliHandler.h"
#include "Net/SocketFactory.h"
#include "Net/TCPClient.h"
#include "Sync/Mutex.h"
#include "Sync/Event.h"
#include "Text/UTF8Writer.h"

namespace Net
{
	class LogClient : public IO::LogHandler, public IO::ProtocolHandler::DataListener
	{
	private:
		IO::ProtoHdlr::ProtoLogCliHandler protoHdlr;
		NN<Net::SocketFactory> sockf;
		Net::SocketUtil::AddressInfo addr;
		UInt16 port;

		Sync::Mutex mut;
		Data::ArrayList<Data::Timestamp> dateList;
		Data::ArrayListStringNN msgList;

		Int64 lastSendTime;
		Sync::Mutex cliMut;
		Net::TCPClient *cli;
		Data::Duration timeout;

		Bool sendRunning;
		Bool sendToStop;
		Sync::Event sendEvt;
		Bool recvRunning;
		Bool recvToStop;
		Sync::Event recvEvt;

		static UInt32 __stdcall RecvThread(AnyType userObj);
		static UInt32 __stdcall SendThread(AnyType userObj);
	public:
		LogClient(NN<Net::SocketFactory> sockf, const Net::SocketUtil::AddressInfo *addr, UInt16 port, Data::Duration timeout);
		virtual ~LogClient();
		virtual void LogClosed();
		virtual void LogAdded(const Data::Timestamp &logTime, Text::CStringNN logMsg, LogLevel logLev);

		virtual void DataParsed(NN<IO::Stream> stm, AnyType stmObj, Int32 cmdType, Int32 seqId, UnsafeArray<const UInt8> cmd, UOSInt cmdSize);
		virtual void DataSkipped(NN<IO::Stream> stm, AnyType stmObj, UnsafeArray<const UInt8> buff, UOSInt buffSize);
	};
}
#endif
