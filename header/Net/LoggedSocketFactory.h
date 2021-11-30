#ifndef _SM_NET_LOGGEDSOCKETFACTORY
#define _SM_NET_LOGGEDSOCKETFACTORY
#include "IO/LogTool.h"
#include "Net/SocketFactory.h"

namespace Net
{
	class LoggedSocketFactory : public Net::SocketFactory
	{
	private:
		Net::SocketFactory *sockf;
		IO::LogTool *log;
		const UTF8Char *logPrefix;
	public:
		LoggedSocketFactory(Net::SocketFactory *sockf, IO::LogTool *log, const UTF8Char *logPrefix);
		virtual ~LoggedSocketFactory();

		virtual Socket *CreateTCPSocketv4();
		virtual Socket *CreateTCPSocketv6();
		virtual Socket *CreateUDPSocketv4();
		virtual void DestroySocket(Socket *socket);
		virtual Bool SocketIsInvalid(Socket *socket);
		virtual Bool SocketBindv4(Socket *socket, UInt32 ip, UInt16 port);
		virtual Bool SocketListen(Socket *socket);
		virtual Socket *SocketAccept(Socket *socket);

		virtual UOSInt SendData(Socket *socket, const UInt8 *buff, UOSInt buffSize, ErrorType *et);
		virtual UOSInt ReceiveData(Socket *socket, UInt8 *buff, UOSInt buffSize, ErrorType *et);
		virtual void *BeginReceiveData(Socket *socket, UInt8 *buff, UOSInt buffSize, Sync::Event *evt, ErrorType *et);
		virtual UOSInt EndReceiveData(void *reqData, Bool toWait, Bool *incomplete);
		virtual void CancelReceiveData(void *reqData);
		virtual UOSInt UDPReceive(Socket *socket, UInt8 *buff, UOSInt buffSize, Net::SocketUtil::AddressInfo *addr, UInt16 *port, ErrorType *et);
		virtual UOSInt SendTo(Socket *socket, const UInt8 *buff, UOSInt buffSize, const Net::SocketUtil::AddressInfo *addr, UInt16 port);

		virtual Bool Connect(Socket *socket, UInt32 ip, UInt16 port);
		virtual Bool BindSocketIPv4(Socket *socket, const Char *ip, UInt16 port);
		virtual void ShutdownSend(Socket *socket);
	};
}
#endif
