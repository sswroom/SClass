#ifndef _SM_NET_TCPSERVER
#define _SM_NET_TCPSERVER
#include "Net/SocketFactory.h"
#include "Data/LinkedList.h"
#include "IO/LogTool.h"
#include "Sync/Event.h"

namespace Net
{
	class TCPServer
	{
	public:
		typedef void (__stdcall *TCPServerConn)(UInt32 *s, void *userObj);
		typedef struct
		{
			TCPServer *me;
			Bool toStop;
			Bool threadRunning;
			Sync::Event *threadEvt;
		} SubthreadStatus;

	private:
		SocketFactory *socf;

	public:
		UInt16 port;
		IO::LogTool *log;
		TCPServerConn hdlr;
		Bool toStop;
		Bool errorv4;
		Bool errorv6;
		const UTF8Char *logPrefix;
		UInt32 *svrSocv4;
		UInt32 *svrSocv6;
		void *userObj;
		Int32 threadRunning;
		Data::LinkedList *socs;
		Sync::Event *socsEvt;

	private:
		void AddLogMsg(const UTF8Char *msg, IO::ILogHandler::LogLevel logLev);
		static UInt32 __stdcall Svrv4Subthread(void *o);
		static UInt32 __stdcall Svrv4Thread(void *o);
		static UInt32 __stdcall Svrv6Subthread(void *o);
		static UInt32 __stdcall Svrv6Thread(void *o);
		static UInt32 __stdcall SvrThread2(void *o);

	public:
		TCPServer(SocketFactory *socf, UInt16 port, IO::LogTool *log, TCPServerConn hdlr, void *userObj, const UTF8Char *logPrefix);
		~TCPServer();
		void Close();
		Bool IsV4Error();
		Bool IsV6Error();
	};
};
#endif
