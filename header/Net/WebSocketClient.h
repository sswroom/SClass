#ifndef _SM_NET_WEBSOCKETCLIENT
#define _SM_NET_WEBSOCKETCLIENT
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
		Optional<Net::TCPClient> cli;
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

		Bool SendPacket(UInt8 opcode, UnsafeArray<const UInt8> buff, UOSInt buffSize);
		UnsafeArrayOpt<const UInt8> NextPacket(OutParam<UInt8> opcode, OutParam<UOSInt> packetSize);
		UnsafeArrayOpt<const UInt8> NextPacket(NN<Sync::MutexUsage> mutUsage, OutParam<UOSInt> packetSize);
	public:
		WebSocketClient(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Text::CStringNN host, UInt16 port, Text::CStringNN path, Text::CString origin, Protocol protocol, Data::Duration timeout);
		virtual ~WebSocketClient();

		virtual UnsafeArrayOpt<UTF8Char> GetRemoteName(UnsafeArray<UTF8Char> buff) const;
		virtual UnsafeArrayOpt<UTF8Char> GetLocalName(UnsafeArray<UTF8Char> buff) const;
		virtual Bool GetRemoteAddr(NN<Net::SocketUtil::AddressInfo> addr) const;
		virtual UInt16 GetRemotePort() const;
		virtual UInt16 GetLocalPort() const;

		virtual Bool IsDown() const;
		virtual UOSInt Read(const Data::ByteArray &buff);
		virtual UOSInt Write(Data::ByteArrayR buff);
		virtual Int32 Flush();
		virtual void Close();
		virtual Bool Recover();
		virtual IO::StreamType GetStreamType() const;

		Bool Shutdown();

		Bool SendPing(UnsafeArray<const UInt8> buff, UOSInt buffSize);
		Bool SendPong(UnsafeArray<const UInt8> buff, UOSInt buffSize);
		Bool SendClose(UnsafeArray<const UInt8> buff, UOSInt buffSize);
	};
}
#endif
