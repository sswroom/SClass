#ifndef _SM_NET_TCPBOARDCASTSTREAM
#define _SM_NET_TCPBOARDCASTSTREAM
#include "AnyType.h"
#include "IO/LogTool.h"
#include "IO/Stream.h"
#include "Net/TCPClientMgr.h"
#include "Net/TCPServer.h"

namespace Net
{
	class TCPBoardcastStream : public IO::Stream
	{
	public:
		NN<Net::SocketFactory> sockf;
		Net::TCPServer *svr;
		Net::TCPClientMgr *cliMgr;
		NN<IO::LogTool> log;
		Sync::Mutex readMut;
		UInt32 readCnt;
		UInt8 *readBuff;
		UIntOS readBuffPtr1;
		UIntOS readBuffPtr2;
		UInt8 *writeBuff;
		UIntOS writeBuffSize;

		static void __stdcall ConnHandler(NN<Socket> s, AnyType userObj);
		static void __stdcall ClientEvent(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData, Net::TCPClientMgr::TCPEventType evtType);
		static void __stdcall ClientData(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData, const Data::ByteArrayR &buff);
		static void __stdcall ClientTimeout(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData);
	public:
		TCPBoardcastStream(NN<Net::SocketFactory> sockf, UInt16 port, NN<IO::LogTool> log);
		~TCPBoardcastStream();

		virtual Bool IsDown() const;
		virtual UIntOS Read(const Data::ByteArray &buff);
		virtual UIntOS Write(Data::ByteArrayR buff);

		virtual Int32 Flush();
		virtual void Close();
		virtual Bool Recover();
		virtual IO::StreamType GetStreamType() const;

		Bool IsError() const;
	};
}
#endif
