#ifndef _SM_NET_TCPSERVER
#define _SM_NET_TCPSERVER
#include "Data/SyncLinkedList.h"
#include "IO/LogTool.h"
#include "Net/SocketFactory.h"
#include "Sync/Event.h"

namespace Net
{
	class TCPServer
	{
	public:
		typedef void (__stdcall *TCPServerConn)(Socket *s, void *userObj);
		typedef struct
		{
			TCPServer *me;
			Bool toStop;
			Bool threadRunning;
			Sync::Event *threadEvt;
		} SubthreadStatus;

	private:
		NotNullPtr<SocketFactory> socf;

	public:
		UInt16 port;
		NotNullPtr<IO::LogTool> log;
		TCPServerConn hdlr;
		Bool toStop;
		Bool errorv4;
		Bool errorv6;
		Optional<Text::String> logPrefix;
		Socket *svrSocv4;
		Socket *svrSocv6;
		void *userObj;
		Int32 threadRunning;
		Data::SyncLinkedList socs;
		Sync::Event socsEvt;

	private:
		void AddLogMsgC(const UTF8Char *msg, UOSInt msgLen, IO::LogHandler::LogLevel logLev);
		static UInt32 __stdcall Svrv4Subthread(void *o);
		static UInt32 __stdcall Svrv4Thread(void *o);
		static UInt32 __stdcall Svrv6Subthread(void *o);
		static UInt32 __stdcall Svrv6Thread(void *o);
		static UInt32 __stdcall WorkerThread(void *o);
		void AcceptSocket(Socket *svrSoc);
	public:
		TCPServer(NotNullPtr<SocketFactory> socf, Optional<Net::SocketUtil::AddressInfo> bindAddr, UInt16 port, NotNullPtr<IO::LogTool> log, TCPServerConn hdlr, void *userObj, Text::CString logPrefix, Bool autoStart);
		~TCPServer();

		Bool Start();
		void Close();
		Bool IsV4Error();
		Bool IsV6Error();
		UInt16 GetListenPort() const;
	};
}
#endif
