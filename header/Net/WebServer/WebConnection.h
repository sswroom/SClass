#ifndef _SM_NET_WEBSERVER_WEBCONNECTION
#define _SM_NET_WEBSERVER_WEBCONNECTION
#include "IO/BufferedOutputStream.h"
#include "IO/FileStream.h"
#include "Net/SSLEngine.h"
#include "Net/WebServer/IWebHandler.h"
#include "Net/WebServer/WebListener.h"
#include "Net/WebServer/WebRequest.h"
#include "Net/WebServer/WebServerBase.h"
#include "Sync/Mutex.h"
#include "Text/StringBuilderUTF8.h"

namespace Net
{
	namespace WebServer
	{
		class WebConnection : public IWebResponse
		{
		public:
			typedef void (__stdcall *SendLogger)(void *userObj, UOSInt buffSize);
		private:
			NotNullPtr<Net::SocketFactory> sockf;
			Net::SSLEngine *ssl;
			NotNullPtr<Net::TCPClient> cli;
			IO::BufferedOutputStream *cstm;
			Net::WebServer::WebListener *svr;
			Net::WebServer::IWebHandler *hdlr;
			ProtocolHandler *protoHdlr;
			UInt8 *dataBuff;
			UOSInt dataBuffSize;
			UOSInt buffSize;
			Net::WebServer::WebRequest *currReq;
			KeepAlive keepAlive;
			Sync::Mutex procMut;

			Bool allowProxy;
			Bool proxyMode;
			Net::TCPClient *proxyCli;

			Bool respHeaderSent;
			Bool respDataEnd;
			Net::WebStatus::StatusCode respStatus;
			Text::StringBuilderUTF8 respHeaders;
			UInt64 respLeng;
			UInt32 respTranEnc;

			SendLogger logger;
			void *loggerObj;

			SSEDisconnectHandler sseHdlr;
			void *sseHdlrObj;
		public:
			WebConnection(NotNullPtr<Net::SocketFactory> sockf, Net::SSLEngine *ssl, NotNullPtr<Net::TCPClient> cli, WebListener *svr, IWebHandler *hdlr, Bool allowProxy, KeepAlive KeepAlive);
			virtual ~WebConnection();

			void ReceivedData(const UInt8 *buff, UOSInt size);
			void ProxyData(const UInt8 *buff, UOSInt size);
			void EndProxyConn();
			void ProxyShutdown();
			void ProcessTimeout();
			Text::String *GetRequestURL();

		private:
			void SendHeaders(IWebRequest::RequestProtocol protocol);
			void ProcessResponse();

		public:
			virtual void EnableWriteBuffer();
			virtual Bool SetStatusCode(Net::WebStatus::StatusCode code);
			virtual Int32 GetStatusCode();
			virtual Bool AddHeader(Text::CString name, Text::CString value);
			virtual Bool AddDefHeaders(Net::WebServer::IWebRequest *req);
			virtual UInt64 GetRespLength();
			virtual void ShutdownSend();
			virtual Bool ResponseSSE(Data::Duration timeout, SSEDisconnectHandler hdlr, void *userObj);
			virtual Bool SSESend(const UTF8Char *eventName, const UTF8Char *data);
			virtual Bool SwitchProtocol(ProtocolHandler *protoHdlr);
			virtual Text::CString GetRespHeaders();

			virtual Bool IsDown() const;
			virtual UOSInt Read(Data::ByteArray buff);
			virtual UOSInt Write(const UInt8 *buff, UOSInt size);
			virtual Int32 Flush();
			virtual void Close();
			virtual Bool Recover();
			virtual IO::StreamType GetStreamType() const;

			void SetSendLogger(SendLogger logger, void *userObj);
		};
	}
}
#endif
