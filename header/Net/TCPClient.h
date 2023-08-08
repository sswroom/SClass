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
		NotNullPtr<Net::SocketFactory> sockf;
		Socket *s;
		UInt64 currCnt;
		UInt64 cliId;
		Int32 flags; //1 = shutdown send, 2 = shutdown recv, 4 = closed, 8 = connect error
		Data::Duration timeout;

		Sync::Event *readEvent;
		Sync::Event *writeEvent;

	public:
		TCPClient(NotNullPtr<Net::SocketFactory> sockf, Text::CStringNN name, UInt16 port, Data::Duration timeout);
		TCPClient(NotNullPtr<Net::SocketFactory> sockf, UInt32 ip, UInt16 port, Data::Duration timeout);
		TCPClient(NotNullPtr<Net::SocketFactory> sockf, const Net::SocketUtil::AddressInfo *addr, UInt16 port, Data::Duration timeout);
		TCPClient(NotNullPtr<Net::SocketFactory> sockf, Socket *s);
		virtual ~TCPClient();

		virtual Bool IsDown() const;
		virtual UOSInt Read(const Data::ByteArray &buff);
		virtual UOSInt Write(const UInt8 *buff, UOSInt size);

		virtual void *BeginRead(const Data::ByteArray &buff, Sync::Event *evt);
		virtual UOSInt EndRead(void *reqData, Bool toWait, Bool *incomplete);
		virtual void CancelRead(void *reqData);
		virtual void *BeginWrite(const UInt8 *buff, UOSInt size, Sync::Event *evt);
		virtual UOSInt EndWrite(void *reqData, Bool toWait);
		virtual void CancelWrite(void *reqData);

		virtual Int32 Flush();
		virtual void Close();
		virtual Bool Recover();
		virtual Bool IsSSL();
		virtual IO::StreamType GetStreamType() const;
		Bool IsClosed();
		Bool IsSendDown();
		Bool IsRecvDown();
		Bool IsConnectError();

		UOSInt GetRecvBuffSize();

		UInt64 GetCliId();
		virtual UTF8Char *GetRemoteName(UTF8Char *buff) const;
		virtual UTF8Char *GetLocalName(UTF8Char *buff) const;
		virtual Bool GetRemoteAddr(Net::SocketUtil::AddressInfo *addr) const;
		virtual UInt16 GetRemotePort() const;
		virtual UInt16 GetLocalPort() const;

		void SetNoDelay(Bool val);
		virtual void ShutdownSend();
		void SetTimeout(Data::Duration timeout);
		Data::Duration GetTimeout();

		Socket *GetSocket();
		Socket *RemoveSocket();
	};
}
#endif
