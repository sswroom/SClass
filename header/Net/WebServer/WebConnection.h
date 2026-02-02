#ifndef _SM_NET_WEBSERVER_WEBCONNECTION
#define _SM_NET_WEBSERVER_WEBCONNECTION
#include "IO/BufferedOutputStream.h"
#include "IO/FileStream.h"
#include "Net/SSLEngine.h"
#include "Net/WebServer/WebHandler.h"
#include "Net/WebServer/WebListener.h"
#include "Net/WebServer/WebServerRequest.h"
#include "Net/WebServer/WebServerBase.h"
#include "Sync/Mutex.h"
#include "Text/StringBuilderUTF8.h"

namespace Net
{
	namespace WebServer
	{
		class WebConnection : public WebResponse
		{
		public:
			typedef void (CALLBACKFUNC SendLogger)(AnyType userObj, UIntOS buffSize);
		private:
			NN<Net::TCPClientFactory> clif;
			Optional<Net::SSLEngine> ssl;
			NN<Net::TCPClient> cli;
			Optional<IO::BufferedOutputStream> cstm;
			NN<Net::WebServer::WebListener> svr;
			NN<Net::WebServer::WebHandler> hdlr;
			Optional<ProtocolHandler> protoHdlr;
			UnsafeArray<UInt8> dataBuff;
			UIntOS dataBuffSize;
			UIntOS buffSize;
			Optional<Net::WebServer::WebServerRequest> currReq;
			KeepAlive keepAlive;
			Sync::Mutex procMut;

			Bool allowProxy;
			Bool proxyMode;
			Optional<Net::TCPClient> proxyCli;

			Bool respHeaderSent;
			Bool respDataEnd;
			Net::WebStatus::StatusCode respStatus;
			Text::StringBuilderUTF8 respHeaders;
			UInt64 respLeng;
			UInt32 respTranEnc;

			SendLogger logger;
			AnyType loggerObj;
			Optional<IO::SMTCWriter> logWriter;

			SSEDisconnectHandler sseHdlr;
			AnyType sseHdlrObj;

			UIntOS SendData(UnsafeArray<const UInt8> buff, UIntOS buffSize);
		public:
			WebConnection(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, NN<Net::TCPClient> cli, NN<WebListener> svr, NN<WebHandler> hdlr, Bool allowProxy, KeepAlive KeepAlive);
			virtual ~WebConnection();

			void ReceivedData(const Data::ByteArrayR &buff);
			void ProxyData(const Data::ByteArrayR &buff);
			void EndProxyConn();
			void ProxyShutdown();
			void ProcessTimeout();
			Optional<Text::String> GetRequestURL();

		private:
			void SendHeaders(WebRequest::RequestProtocol protocol);
			void ProcessResponse();

		public:
			virtual void EnableWriteBuffer();
			virtual Bool SetStatusCode(Net::WebStatus::StatusCode code);
			virtual Int32 GetStatusCode();
			virtual Bool AddHeader(Text::CStringNN name, Text::CStringNN value);
			virtual Bool AddDefHeaders(NN<Net::WebServer::WebRequest> req);
			virtual UInt64 GetRespLength();
			virtual void ShutdownSend();
			virtual Bool ResponseSSE(Data::Duration timeout, SSEDisconnectHandler hdlr, AnyType userObj);
			virtual Bool SSESend(const UTF8Char *eventName, const UTF8Char *data);
			virtual Bool SwitchProtocol(Optional<ProtocolHandler> protoHdlr);
			virtual Text::CStringNN GetRespHeaders();

			virtual Bool IsDown() const;
			virtual UIntOS Read(const Data::ByteArray &buff);
			virtual UIntOS Write(Data::ByteArrayR buff);
			virtual Int32 Flush();
			virtual void Close();
			virtual Bool Recover();
			virtual IO::StreamType GetStreamType() const;

			void SetSendLogger(SendLogger logger, AnyType userObj);
			void SetLogWriter(Optional<IO::SMTCWriter> logWriter);
		};
	}
}
#endif
