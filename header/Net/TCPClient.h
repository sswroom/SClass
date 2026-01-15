#ifndef _SM_NET_TCPCLIENT
#define _SM_NET_TCPCLIENT
#include "IO/Stream.h"
#include "Net/NetConnection.h"
#include "Net/SocketFactory.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"

namespace Net
{
	class TCPClient : public IO::Stream, public Net::NetConnection
	{
	protected:
		NN<Net::SocketFactory> sockf;
		Optional<Socket> s;
		UInt64 currCnt;
		UInt64 cliId;
		Int32 flags; //1 = shutdown send, 2 = shutdown recv, 4 = closed, 8 = connect error
		Data::Duration timeout;

		Sync::Event *readEvent;
		Sync::Event *writeEvent;

	public:
		TCPClient(NN<Net::SocketFactory> sockf, Text::CStringNN name, UInt16 port, Data::Duration timeout);
		TCPClient(NN<Net::SocketFactory> sockf, UInt32 ip, UInt16 port, Data::Duration timeout);
		TCPClient(NN<Net::SocketFactory> sockf, NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::Duration timeout);
		TCPClient(NN<Net::SocketFactory> sockf, Optional<Socket> s);
		virtual ~TCPClient();

		virtual Bool IsDown() const;
		virtual UIntOS Read(const Data::ByteArray &buff);
		virtual UIntOS Write(Data::ByteArrayR buff);

		virtual Optional<IO::StreamReadReq> BeginRead(const Data::ByteArray &buff, NN<Sync::Event> evt);
		virtual UIntOS EndRead(NN<IO::StreamReadReq> reqData, Bool toWait, OutParam<Bool> incomplete);
		virtual void CancelRead(NN<IO::StreamReadReq> reqData);
		virtual Optional<IO::StreamWriteReq> BeginWrite(Data::ByteArrayR buff, NN<Sync::Event> evt);
		virtual UIntOS EndWrite(NN<IO::StreamWriteReq> reqData, Bool toWait);
		virtual void CancelWrite(NN<IO::StreamWriteReq> reqData);

		virtual Int32 Flush();
		virtual void Close();
		virtual Bool Recover();
		virtual Bool IsSSL() const;
		virtual IO::StreamType GetStreamType() const;
		Bool IsClosed();
		Bool IsSendDown();
		Bool IsRecvDown();
		Bool IsConnectError();

		UIntOS GetRecvBuffSize();
		Bool Wait(Data::Duration dur);

		UInt64 GetCliId();
		virtual UnsafeArrayOpt<UTF8Char> GetRemoteName(UnsafeArray<UTF8Char> buff) const;
		virtual UnsafeArrayOpt<UTF8Char> GetLocalName(UnsafeArray<UTF8Char> buff) const;
		virtual Bool GetRemoteAddr(NN<Net::SocketUtil::AddressInfo> addr) const;
		virtual UInt16 GetRemotePort() const;
		virtual UInt16 GetLocalPort() const;

		void SetNoDelay(Bool val);
		virtual void ShutdownSend();
		void SetTimeout(Data::Duration timeout);
		Data::Duration GetTimeout();

		Optional<Socket> GetSocket();
		Optional<Socket> RemoveSocket();
	};
}
#endif
