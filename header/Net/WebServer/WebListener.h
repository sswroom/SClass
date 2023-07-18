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
#include "Net/WebServer/WebServerBase.h"

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
			Net::TCPClientMgr cliMgr;
			Net::TCPClientMgr *proxyCliMgr;
			Net::SSLEngine *ssl;
			NotNullPtr<Net::SocketFactory> sockf;
			IO::LogTool log;
			NotNullPtr<Text::String> svrName;
			Bool allowProxy;
			KeepAlive keepAlive;
			SERVER_STATUS status;

			Sync::Mutex accLogMut;
			IO::LogTool *accLog;
			IO::LogHandler::LogLevel accLogLev;
			IReqLogger *reqLog;

			TimeoutHandler timeoutHdlr;
			void *timeoutObj;
			

		private:
			static void __stdcall ClientReady(NotNullPtr<Net::TCPClient> cli, void *userObj);
			static void __stdcall ConnHdlr(Socket *s, void *userObj);
			static void __stdcall ClientEvent(NotNullPtr<Net::TCPClient> cli, void *userObj, void *cliData, Net::TCPClientMgr::TCPEventType evtType);
			static void __stdcall ClientData(NotNullPtr<Net::TCPClient> cli, void *userObj, void *cliData, const UInt8 *buff, UOSInt size);
			static void __stdcall ClientTimeout(NotNullPtr<Net::TCPClient> cli, void *userObj, void *cliData);

			static void __stdcall ProxyClientEvent(NotNullPtr<Net::TCPClient> cli, void *userObj, void *cliData, Net::TCPClientMgr::TCPEventType evtType);
			static void __stdcall ProxyClientData(NotNullPtr<Net::TCPClient> cli, void *userObj, void *cliData, const UInt8 *buff, UOSInt size);
			static void __stdcall ProxyTimeout(NotNullPtr<Net::TCPClient> cli, void *userObj, void *cliData);

			static void __stdcall OnDataSent(void *userObj, UOSInt buffSize);
		public:
			WebListener(NotNullPtr<Net::SocketFactory> sockf, Net::SSLEngine *ssl, IWebHandler *hdlr, UInt16 port, Int32 timeoutSeconds, UOSInt workerCnt, Text::CString svrName, Bool allowProxy, KeepAlive keepAlive, Bool autoStart);
			~WebListener();

			Bool Start();
			Bool IsError();
			NotNullPtr<Text::String> GetServerName() const;
			void SetAccessLog(IO::LogTool *accLog, IO::LogHandler::LogLevel accLogLev);
			void SetRequestLog(IReqLogger *reqLog);
			void LogAccess(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Double time);
			void LogMessageC(Net::WebServer::IWebRequest *req, const UTF8Char *msg, UOSInt msgLen);
			void AddProxyConn(Net::WebServer::WebConnection *conn, NotNullPtr<Net::TCPClient> proxyCli);
			void HandleTimeout(TimeoutHandler hdlr, void *userObj);

			void ExtendTimeout(NotNullPtr<Net::TCPClient> cli);
			void GetStatus(SERVER_STATUS *status);
			UOSInt GetClientCount() const;
		};
	}
}
#endif
