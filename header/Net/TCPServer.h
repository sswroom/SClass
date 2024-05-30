#ifndef _SM_NET_TCPSERVER
#define _SM_NET_TCPSERVER
#include "AnyType.h"
#include "Data/CallbackStorage.h"
#include "Data/SyncLinkedList.h"
#include "IO/LogTool.h"
#include "Net/SocketFactory.h"
#include "Sync/Event.h"

namespace Net
{
	class TCPServer
	{
	public:
		typedef void (__stdcall *TCPServerConn)(NN<Socket> s, AnyType userObj);
		typedef struct
		{
			NN<TCPServer> me;
			Bool toStop;
			Bool threadRunning;
			Sync::Event *threadEvt;
		} SubthreadStatus;

	private:
		NN<SocketFactory> socf;

	public:
		UInt16 port;
		NN<IO::LogTool> log;
		Data::CallbackStorage<TCPServerConn> hdlr;
		Bool toStop;
		Bool errorv4;
		Bool errorv6;
		Optional<Text::String> logPrefix;
		Optional<Socket> svrSocv4;
		Optional<Socket> svrSocv6;
		Int32 threadRunning;
		Data::SyncLinkedList socs;
		Sync::Event socsEvt;

	private:
		void AddLogMsgC(Text::CStringNN msg, IO::LogHandler::LogLevel logLev);
		static UInt32 __stdcall Svrv4Subthread(AnyType o);
		static UInt32 __stdcall Svrv4Thread(AnyType o);
		static UInt32 __stdcall Svrv6Subthread(AnyType o);
		static UInt32 __stdcall Svrv6Thread(AnyType o);
		static UInt32 __stdcall WorkerThread(AnyType o);
		void AcceptSocket(NN<Socket> svrSoc);
	public:
		TCPServer(NN<SocketFactory> socf, Optional<Net::SocketUtil::AddressInfo> bindAddr, UInt16 port, NN<IO::LogTool> log, TCPServerConn hdlr, AnyType userObj, Text::CString logPrefix, Bool autoStart);
		~TCPServer();

		Bool Start();
		void Close();
		Bool IsV4Error();
		Bool IsV6Error();
		UInt16 GetListenPort() const;
	};
}
#endif
