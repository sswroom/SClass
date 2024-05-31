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
			typedef void (__stdcall *SendLogger)(AnyType userObj, UOSInt buffSize);
		private:
			NN<Net::SocketFactory> sockf;
			Optional<Net::SSLEngine> ssl;
			NN<Net::TCPClient> cli;
			IO::BufferedOutputStream *cstm;
			NN<Net::WebServer::WebListener> svr;
			NN<Net::WebServer::IWebHandler> hdlr;
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
			AnyType loggerObj;
			IO::SMTCWriter *logWriter;

			SSEDisconnectHandler sseHdlr;
			AnyType sseHdlrObj;

			UOSInt SendData(UnsafeArray<const UInt8> buff, UOSInt buffSize);
		public:
			WebConnection(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, NN<Net::TCPClient> cli, NN<WebListener> svr, NN<IWebHandler> hdlr, Bool allowProxy, KeepAlive KeepAlive);
			virtual ~WebConnection();

			void ReceivedData(const Data::ByteArrayR &buff);
			void ProxyData(const Data::ByteArrayR &buff);
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
			virtual Bool AddHeader(Text::CStringNN name, Text::CStringNN value);
			virtual Bool AddDefHeaders(NN<Net::WebServer::IWebRequest> req);
			virtual UInt64 GetRespLength();
			virtual void ShutdownSend();
			virtual Bool ResponseSSE(Data::Duration timeout, SSEDisconnectHandler hdlr, AnyType userObj);
			virtual Bool SSESend(const UTF8Char *eventName, const UTF8Char *data);
			virtual Bool SwitchProtocol(ProtocolHandler *protoHdlr);
			virtual Text::CString GetRespHeaders();

			virtual Bool IsDown() const;
			virtual UOSInt Read(const Data::ByteArray &buff);
			virtual UOSInt Write(UnsafeArray<const UInt8> buff, UOSInt size);
			virtual Int32 Flush();
			virtual void Close();
			virtual Bool Recover();
			virtual IO::StreamType GetStreamType() const;

			void SetSendLogger(SendLogger logger, AnyType userObj);
			void SetLogWriter(IO::SMTCWriter *logWriter);
		};
	}
}
#endif
