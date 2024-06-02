#ifndef _SM_NET_TCPSERVERSTREAM
#define _SM_NET_TCPSERVERSTREAM
#include "IO/LogTool.h"
#include "IO/Stream.h"
#include "Net/TCPClient.h"
#include "Net/TCPServer.h"

namespace Net
{
	class TCPServerStream : public IO::Stream
	{
	public:
		NN<Net::SocketFactory> sockf;
		Net::TCPServer *svr;
		NN<IO::LogTool> log;
		Sync::Mutex readMut;
		Sync::Mutex connMut;
		Sync::Event readEvt;
		Net::TCPClient *currCli;

		static void __stdcall ConnHandler(NN<Socket> s, AnyType userObj);
	public:
		TCPServerStream(NN<Net::SocketFactory> sockf, UInt16 port, NN<IO::LogTool> log);
		~TCPServerStream();

		virtual Bool IsDown() const;
		virtual UOSInt Read(const Data::ByteArray &buff);
		virtual UOSInt Write(UnsafeArray<const UInt8> buff, UOSInt size);

		virtual Int32 Flush();
		virtual void Close();
		virtual Bool Recover();
		virtual IO::StreamType GetStreamType() const;

		Bool IsError() const;
	};
}
#endif
