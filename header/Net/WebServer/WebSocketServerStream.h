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
			IO::StreamHandler *stmHdlr;
			AnyType stmData;

			UInt8 *recvBuff;
			UOSInt recvCapacity;
			UOSInt recvSize;

			Bool SendPacket(UInt8 opcode, Data::ByteArrayR buff);
			void NextPacket(UInt8 opcode, UnsafeArray<const UInt8> buff, UOSInt buffSize);
		public:
			WebSocketServerStream(IO::StreamHandler *stmHdlr, NN<Net::WebServer::WebResponse> resp);
			virtual ~WebSocketServerStream();

			virtual Bool IsDown() const;
			virtual UOSInt Read(const Data::ByteArray &buff);
			virtual UOSInt Write(Data::ByteArrayR buff);

			virtual Int32 Flush();
			virtual void Close();
			virtual Bool Recover();
			virtual IO::StreamType GetStreamType() const;

			virtual void ProtocolData(const UInt8 *data, UOSInt dataSize);
			virtual void ConnectionClosed();
		};
	}
}
#endif
