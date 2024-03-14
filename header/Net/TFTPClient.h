#ifndef _SM_NET_TFTPCLIENT
#define _SM_NET_TFTPCLIENT
#include "IO/Stream.h"
#include "Net/UDPServer.h"
#include "Sync/Event.h"

namespace Net
{
	class TFTPClient
	{
	private:
		Net::SocketUtil::AddressInfo addr;
		UInt16 port;
		UInt16 recvPort;
		Net::UDPServer *svr;
		UInt16 nextId;
		Bool replyRecv;
		Bool replyError;
		UOSInt recvSize;
		IO::Stream *recvStm;
		Sync::Event *evt;

		static void __stdcall OnDataPacket(NotNullPtr<const Net::SocketUtil::AddressInfo> addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, AnyType userData);
	public:
		TFTPClient(NotNullPtr<Net::SocketFactory> sockf, NotNullPtr<const Net::SocketUtil::AddressInfo> addr, UInt16 port, NotNullPtr<IO::LogTool> log);
		~TFTPClient();

		Bool IsError();
		Bool SendFile(const UTF8Char *fileName, IO::Stream *stm);
		Bool RecvFile(const UTF8Char *fileName, IO::Stream *stm);
	};
}
#endif
