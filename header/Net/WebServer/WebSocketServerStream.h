#ifndef _SM_NET_WEBSERVER_WEBSOCKETSERVERSTREAM
#define _SM_NET_WEBSERVER_WEBSOCKETSERVERSTREAM
#include "IO/StreamHandler.h"
#include "Net/WebServer/WebResponse.h"
#include "Sync/Mutex.h"

namespace Net
{
	namespace WebServer
	{
		class WebSocketServerStream : public Net::WebServer::ProtocolHandler, public IO::Stream
		{
		private:
			Sync::Mutex sendMut;
			NN<Net::WebServer::WebResponse> resp;
			NN<IO::StreamHandler> stmHdlr;
			AnyType stmData;

			UnsafeArray<UInt8> recvBuff;
			UIntOS recvCapacity;
			UIntOS recvSize;

			Bool SendPacket(UInt8 opcode, Data::ByteArrayR buff);
			void NextPacket(UInt8 opcode, UnsafeArray<const UInt8> buff, UIntOS buffSize);
		public:
			WebSocketServerStream(NN<IO::StreamHandler> stmHdlr, NN<Net::WebServer::WebResponse> resp);
			virtual ~WebSocketServerStream();

			virtual Bool IsDown() const;
			virtual UIntOS Read(const Data::ByteArray &buff);
			virtual UIntOS Write(Data::ByteArrayR buff);

			virtual Int32 Flush();
			virtual void Close();
			virtual Bool Recover();
			virtual IO::StreamType GetStreamType() const;

			virtual void ProtocolData(UnsafeArray<const UInt8> data, UIntOS dataSize);
			virtual void ConnectionClosed();
		};
	}
}
#endif
