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

			typedef void (__stdcall *TimeoutHandler)(AnyType userObj, Text::String *url);
		private:
			NN<IWebHandler> hdlr;
			NN<Net::TCPServer> svr;
			Data::ArrayListNN<Net::TCPClientMgr> cliMgrs;
			UOSInt nextCli;
			Net::TCPClientMgr *proxyCliMgr;
			Optional<Net::SSLEngine> ssl;
			NN<Net::SocketFactory> sockf;
			IO::LogTool log;
			NN<Text::String> svrName;
			Bool allowProxy;
			KeepAlive keepAlive;
			SERVER_STATUS status;

			Sync::Mutex accLogMut;
			IO::LogTool *accLog;
			IO::LogHandler::LogLevel accLogLev;
			IReqLogger *reqLog;

			TimeoutHandler timeoutHdlr;
			AnyType timeoutObj;
			

		private:
			static void __stdcall ClientReady(NN<Net::TCPClient> cli, AnyType userObj);
			static void __stdcall ConnHdlr(Socket *s, AnyType userObj);
			static void __stdcall ClientEvent(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData, Net::TCPClientMgr::TCPEventType evtType);
			static void __stdcall ClientData(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData, const Data::ByteArrayR &buff);
			static void __stdcall ClientTimeout(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData);

			static void __stdcall ProxyClientEvent(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData, Net::TCPClientMgr::TCPEventType evtType);
			static void __stdcall ProxyClientData(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData, const Data::ByteArrayR &buff);
			static void __stdcall ProxyTimeout(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData);

			static void __stdcall OnDataSent(AnyType userObj, UOSInt buffSize);
		public:
			WebListener(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, NN<IWebHandler> hdlr, UInt16 port, Int32 timeoutSeconds, UOSInt mgrCnt, UOSInt workerCnt, Text::CString svrName, Bool allowProxy, KeepAlive keepAlive, Bool autoStart);
			~WebListener();

			Bool Start();
			Bool IsError();
			NN<Text::String> GetServerName() const;
			UInt16 GetListenPort();
			void SetClientLog(Text::CStringNN logFile);
			void SetAccessLog(IO::LogTool *accLog, IO::LogHandler::LogLevel accLogLev);
			void SetRequestLog(IReqLogger *reqLog);
			void LogAccess(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Double time);
			void LogMessageC(Net::WebServer::IWebRequest *req, const UTF8Char *msg, UOSInt msgLen);
			void AddProxyConn(Net::WebServer::WebConnection *conn, NN<Net::TCPClient> proxyCli);
			void HandleTimeout(TimeoutHandler hdlr, AnyType userObj);

			void ExtendTimeout(NN<Net::TCPClient> cli);
			void GetStatus(SERVER_STATUS *status);
			UOSInt GetClientCount() const;
		};
	}
}
#endif
