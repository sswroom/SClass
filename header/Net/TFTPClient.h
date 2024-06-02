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

		static void __stdcall OnDataPacket(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, AnyType userData);
	public:
		TFTPClient(NN<Net::SocketFactory> sockf, NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, NN<IO::LogTool> log);
		~TFTPClient();

		Bool IsError();
		Bool SendFile(UnsafeArray<const UTF8Char> fileName, IO::Stream *stm);
		Bool RecvFile(UnsafeArray<const UTF8Char> fileName, IO::Stream *stm);
	};
}
#endif
