#ifndef _SM_NET_TCPBOARDCASTSTREAM
#define _SM_NET_TCPBOARDCASTSTREAM
#include "IO/LogTool.h"
#include "IO/Stream.h"
#include "Net/TCPClientMgr.h"
#include "Net/TCPServer.h"

namespace Net
{
	class TCPBoardcastStream : public IO::Stream
	{
	public:
		Net::SocketFactory *sockf;
		Net::TCPServer *svr;
		Net::TCPClientMgr *cliMgr;
		IO::LogTool *log;
		Sync::Mutex *readMut;
		Sync::Mutex *writeMut;
		Int32 readCnt;
		UInt8 *readBuff;
		OSInt readBuffPtr1;
		OSInt readBuffPtr2;
		UInt8 *writeBuff;
		OSInt writeBuffSize;

		static void __stdcall ConnHandler(UInt32 *s, void *userObj);
		static void __stdcall ClientEvent(Net::TCPClient *cli, void *userObj, void *cliData, Net::TCPClientMgr::TCPEventType evtType);
		static void __stdcall ClientData(Net::TCPClient *cli, void *userObj, void *cliData, const UInt8 *buff, UOSInt size);
		static void __stdcall ClientTimeout(Net::TCPClient *cli, void *userObj, void *cliData);
	public:
		TCPBoardcastStream(Net::SocketFactory *sockf, Int32 port, IO::LogTool *log);
		~TCPBoardcastStream();

		virtual UOSInt Read(UInt8 *buff, UOSInt size);
		virtual UOSInt Write(const UInt8 *buff, UOSInt size);

		virtual Int32 Flush();
		virtual void Close();
		virtual Bool Recover();

		Bool IsError();
	};
}
#endif
