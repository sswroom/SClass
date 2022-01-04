#ifndef _SM_NET_WEBSERVER_WEBLISTENER
#define _SM_NET_WEBSERVER_WEBLISTENER
#include "IO/FileStream.h"
#include "IO/LogTool.h"
#include "IO/StreamWriter.h"
#include "Net/SSLEngine.h"
#include "Net/TCPServer.h"
#include "Net/TCPClientMgr.h"
#include "Net/WebServer/IReqLogger.h"
#include "Net/WebServer/IWebHandler.h"

namespace Net
{
	namespace WebServer
	{
		class WebConnection;

		class WebListener
		{
		public:
			typedef struct
			{
				UInt32 currConn;
				UInt64 totalRead;
				UInt64 totalWrite;
				UInt32 connCnt;
				UInt32 reqCnt;
			} SERVER_STATUS;

			typedef void (__stdcall *TimeoutHandler)(void *userObj, Text::String *url);
		private:
			IWebHandler *hdlr;
			Net::TCPServer *svr;
			Net::TCPClientMgr *cliMgr;
			Net::TCPClientMgr *proxyCliMgr;
			Net::SSLEngine *ssl;
			Net::SocketFactory *sockf;
			IO::LogTool *log;
			const UTF8Char *svrName;
			Bool allowProxy;
			Bool allowKA;
			SERVER_STATUS status;
			Sync::Mutex *statusMut;

			Sync::Mutex *accLogMut;
			IO::LogTool *accLog;
			IO::ILogHandler::LogLevel accLogLev;
			IReqLogger *reqLog;

			TimeoutHandler timeoutHdlr;
			void *timeoutObj;
			

		private:
			static void __stdcall ClientReady(Net::TCPClient *cli, void *userObj);
			static void __stdcall ConnHdlr(Socket *s, void *userObj);
			static void __stdcall ClientEvent(Net::TCPClient *cli, void *userObj, void *cliData, Net::TCPClientMgr::TCPEventType evtType);
			static void __stdcall ClientData(Net::TCPClient *cli, void *userObj, void *cliData, const UInt8 *buff, UOSInt size);
			static void __stdcall ClientTimeout(Net::TCPClient *cli, void *userObj, void *cliData);

			static void __stdcall ProxyClientEvent(Net::TCPClient *cli, void *userObj, void *cliData, Net::TCPClientMgr::TCPEventType evtType);
			static void __stdcall ProxyClientData(Net::TCPClient *cli, void *userObj, void *cliData, const UInt8 *buff, UOSInt size);
			static void __stdcall ProxyTimeout(Net::TCPClient *cli, void *userObj, void *cliData);

			static void __stdcall OnDataSent(void *userObj, UOSInt buffSize);
		public:
			WebListener(Net::SocketFactory *sockf, Net::SSLEngine *ssl, IWebHandler *hdlr, UInt16 port, Int32 timeoutSeconds, UOSInt workerCnt, const UTF8Char *svrName, Bool allowProxy, Bool allowKA);
			~WebListener();

			Bool IsError();
			const UTF8Char *GetServerName();
			void SetAccessLog(IO::LogTool *accLog, IO::ILogHandler::LogLevel accLogLev);
			void SetRequestLog(IReqLogger *reqLog);
			void LogAccess(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Double time);
			void LogMessageC(Net::WebServer::IWebRequest *req, const UTF8Char *msg, UOSInt msgLen);
			void AddProxyConn(Net::WebServer::WebConnection *conn, Net::TCPClient *proxyCli);
			void HandleTimeout(TimeoutHandler hdlr, void *userObj);

			void ExtendTimeout(Net::TCPClient *cli);
			void GetStatus(SERVER_STATUS *status);
		};
	}
}
#endif
