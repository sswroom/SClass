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

		virtual UInt32 *CreateTCPSocketv4();
		virtual UInt32 *CreateTCPSocketv6();
		virtual UInt32 *CreateUDPSocketv4();
		virtual void DestroySocket(UInt32 *socket);
		virtual Bool SocketIsInvalid(UInt32 *socket);
		virtual Bool SocketBindv4(UInt32 *socket, UInt32 ip, UInt16 port);
		virtual Bool SocketListen(UInt32 *socket);
		virtual UInt32 *SocketAccept(UInt32 *socket);

		virtual OSInt SendData(UInt32 *socket, const UInt8 *buff, OSInt buffSize, ErrorType *et);
		virtual OSInt ReceiveData(UInt32 *socket, UInt8 *buff, OSInt buffSize, ErrorType *et);
		virtual void *BeginReceiveData(UInt32 *socket, UInt8 *buff, OSInt buffSize, Sync::Event *evt, ErrorType *et);
		virtual OSInt EndReceiveData(void *reqData, Bool toWait);
		virtual void CancelReceiveData(void *reqData);
		virtual OSInt UDPReceive(UInt32 *socket, UInt8 *buff, OSInt buffSize, Net::SocketUtil::AddressInfo *addr, UInt16 *port, ErrorType *et);
		virtual OSInt SendTo(UInt32 *socket, const UInt8 *buff, OSInt buffSize, const Net::SocketUtil::AddressInfo *addr, UInt16 port);

		virtual Bool Connect(UInt32 *socket, UInt32 ip, UInt16 port);
		virtual Bool BindSocketIPv4(UInt32 *socket, const Char *ip, UInt16 port);
		virtual void ShutdownSend(UInt32 *socket);
	};
}
#endif
