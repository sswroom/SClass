#ifndef _SM_NET_WEBSOCKETCLIENT
#define _SM_NET_WEBSOCKETCLIENT
#include "Data/ArrayList.h"
#include "Data/RandomBytesGenerator.h"
#include "IO/Stream.h"
#include "Net/NetConnection.h"
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"
#include "Net/TCPClient.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"

namespace Net
{
	class WebSocketClient : public IO::Stream, public Net::NetConnection
	{
	public:
		enum class Protocol
		{
			MQTT,
			Chat
		};
	private:
		Net::TCPClient *cli;
		UInt8 nonce[16];
		Data::RandomBytesGenerator rand;
		Sync::Mutex sendMut;

		Sync::Mutex recvMut;
		UInt8 *recvBuff;
		UOSInt recvCapacity;
		UOSInt recvSize;
		UOSInt recvParseOfst;

		UInt8 *recvData;
		UOSInt recvDataCap;
		UOSInt recvDataSize;
		UOSInt recvDataOfst;

		Bool SendPacket(UInt8 opcode, const UInt8 *buff, UOSInt buffSize);
		const UInt8 *NextPacket(UInt8 *opcode, UOSInt *packetSize);
		const UInt8 *NextPacket(Sync::MutexUsage *mutUsage, UOSInt *packetSize);
	public:
		WebSocketClient(NotNullPtr<Net::SocketFactory> sockf, Net::SSLEngine *ssl, Text::CString host, UInt16 port, Text::CString path, Text::CString origin, Protocol protocol, Data::Duration timeout);
		virtual ~WebSocketClient();

		virtual UTF8Char *GetRemoteName(UTF8Char *buff) const;
		virtual UTF8Char *GetLocalName(UTF8Char *buff) const;
		virtual Bool GetRemoteAddr(Net::SocketUtil::AddressInfo *addr) const;
		virtual UInt16 GetRemotePort() const;
		virtual UInt16 GetLocalPort() const;

		virtual Bool IsDown() const;
		virtual UOSInt Read(const Data::ByteArray &buff);
		virtual UOSInt Write(const UInt8 *buff, UOSInt size);
		virtual Int32 Flush();
		virtual void Close();
		virtual Bool Recover();
		virtual IO::StreamType GetStreamType() const;

		Bool Shutdown();

		Bool SendPing(const UInt8 *buff, UOSInt buffSize);
		Bool SendPong(const UInt8 *buff, UOSInt buffSize);
		Bool SendClose(const UInt8 *buff, UOSInt buffSize);
	};
}
#endif
