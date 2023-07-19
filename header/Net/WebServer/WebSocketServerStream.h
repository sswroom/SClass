#ifndef _SM_NET_WEBSERVER_WEBSOCKETSERVERSTREAM
#define _SM_NET_WEBSERVER_WEBSOCKETSERVERSTREAM
#include "IO/StreamHandler.h"
#include "Net/WebServer/IWebResponse.h"
#include "Sync/Mutex.h"

namespace Net
{
	namespace WebServer
	{
		class WebSocketServerStream : public Net::WebServer::ProtocolHandler, public IO::Stream
		{
		private:
			Sync::Mutex sendMut;
			Net::WebServer::IWebResponse *resp;
			IO::StreamHandler *stmHdlr;
			void *stmData;

			UInt8 *recvBuff;
			UOSInt recvCapacity;
			UOSInt recvSize;

			Bool SendPacket(UInt8 opcode, const UInt8 *buff, UOSInt buffSize);
			void NextPacket(UInt8 opcode, const UInt8 *buff, UOSInt buffSize);
		public:
			WebSocketServerStream(IO::StreamHandler *stmHdlr, Net::WebServer::IWebResponse *resp);
			virtual ~WebSocketServerStream();

			virtual Bool IsDown() const;
			virtual UOSInt Read(const Data::ByteArray &buff);
			virtual UOSInt Write(const UInt8 *buff, UOSInt size);

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
