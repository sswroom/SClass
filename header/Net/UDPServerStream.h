#ifndef _SM_NET_UDPSERVERSTREAM
#define _SM_NET_UDPSERVERSTREAM
#include "IO/LogTool.h"
#include "IO/Stream.h"
#include "Net/UDPServer.h"
#include "Sync/Mutex.h"

namespace Net
{
	class UDPServerStream : public IO::Stream
	{
	public:
		Net::SocketFactory *sockf;
		Net::UDPServer *svr;
		IO::LogTool *log;
		Sync::Event *readEvt;

		Sync::Mutex *dataMut;
		Net::SocketUtil::AddressInfo lastAddr;
		UInt16 lastPort;
		UInt8 *buff;
		UOSInt buffSize;			

		static void __stdcall OnUDPPacket(const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, void *userData);
	public:
		UDPServerStream(Net::SocketFactory *sockf, UInt16 port, IO::LogTool *log);
		~UDPServerStream();

		virtual Bool IsDown();
		virtual UOSInt Read(UInt8 *buff, UOSInt size);
		virtual UOSInt Write(const UInt8 *buff, UOSInt size);

		virtual Int32 Flush();
		virtual void Close();
		virtual Bool Recover();

		Bool IsError();
		void SetClientAddr(const Net::SocketUtil::AddressInfo *addr, UInt16 port);
	};
}
#endif
