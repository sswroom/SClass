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
		NN<Net::SocketFactory> sockf;
		Net::UDPServer *svr;
		NN<IO::LogTool> log;
		Sync::Event readEvt;

		Sync::Mutex dataMut;
		Net::SocketUtil::AddressInfo lastAddr;
		UInt16 lastPort;
		UInt8 *buff;
		UOSInt buffSize;
		Bool isClient;

		static void __stdcall OnUDPPacket(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::ByteArrayR data, AnyType userData);
	public:
		UDPServerStream(NN<Net::SocketFactory> sockf, UInt16 port, NN<IO::LogTool> log);
		~UDPServerStream();

		virtual Bool IsDown() const;
		virtual UOSInt Read(const Data::ByteArray &buff);
		virtual UOSInt Write(Data::ByteArrayR buff);

		virtual Int32 Flush();
		virtual void Close();
		virtual Bool Recover();
		virtual IO::StreamType GetStreamType() const;

		Bool IsError() const;
		void SetClientAddr(const Net::SocketUtil::AddressInfo *addr, UInt16 port);
	};
}
#endif
