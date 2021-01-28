#ifndef _SM_NET_TCPCLIENT
#define _SM_NET_TCPCLIENT
#include "Net/SocketFactory.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "IO/Stream.h"

namespace Net
{
	class TCPClient : public IO::Stream
	{
	protected:
		Net::SocketFactory *sockf;
		UInt32 *s;
		Int64 currCnt;
		Int64 cliId;
		Int32 flags; //1 = shutdown send, 2 = shutdown recv, 4 = closed, 8 = connect error

		Sync::Event *readEvent;
		Sync::Event *writeEvent;

	public:
		TCPClient(Net::SocketFactory *sockf, const UTF8Char *name, UInt16 port);
		TCPClient(Net::SocketFactory *sockf, Int32 ip, UInt16 port);
		TCPClient(Net::SocketFactory *sockf, const Net::SocketUtil::AddressInfo *addr, UInt16 port);
		TCPClient(Net::SocketFactory *sockf, UInt32 *s);
		virtual ~TCPClient();

		virtual UOSInt Read(UInt8 *buff, UOSInt size);
		virtual UOSInt Write(const UInt8 *buff, UOSInt size);

		virtual void *BeginRead(UInt8 *buff, UOSInt size, Sync::Event *evt);
		virtual UOSInt EndRead(void *reqData, Bool toWait);
		virtual void CancelRead(void *reqData);
		virtual void *BeginWrite(const UInt8 *buff, UOSInt size, Sync::Event *evt);
		virtual UOSInt EndWrite(void *reqData, Bool toWait);
		virtual void CancelWrite(void *reqData);

		virtual Int32 Flush();
		virtual void Close();
		virtual Bool Recover();
		Bool IsClosed();
		Bool IsSendDown();
		Bool IsRecvDown();
		Bool IsConnectError();

		OSInt GetRecvBuffSize();

		UTF8Char *GetRemoteName(UTF8Char *buff);
		UTF8Char *GetLocalName(UTF8Char *buff);
		Int64 GetCliId();
		Bool GetRemoteAddr(Net::SocketUtil::AddressInfo *addr);
		UInt16 GetRemotePort();

		void SetNoDelay(Bool val);
		void ShutdownSend();
		void SetTimeout(Int32 ms);

		UInt32 *GetSocket();
	};
}
#endif
