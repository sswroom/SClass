#ifndef _SM_NET_TFTPSERVER
#define _SM_NET_TFTPSERVER
#include "Data/UInt64Map.h"
#include "IO/LogTool.h"
#include "Net/UDPServer.h"
#include "Sync/Mutex.h"

namespace Net
{
	class TFTPServer
	{
	private:
		typedef struct
		{
			UInt64 sessId;
			Int64 lastSignalTime;
			IO::Stream *stm;
			UOSInt blockSize;
			Bool isWrite;
			Bool isLast;
			UInt16 currBlock;
			Text::String *fileName;
		} SessionInfo;
		
	private:
		IO::LogTool *log;
		Net::UDPServer *svr;
		Net::UDPServer *dataSvr;
		const UTF8Char *path;
		Sync::Mutex *mut;
		Data::UInt64Map<SessionInfo*> *sessMap;
		Bool threadToStop;
		Bool threadRunning;
		Sync::Event *chkEvt;

		static void __stdcall OnCommandPacket(const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, void *userData);
		static void __stdcall OnDataPacket(const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, void *userData);
		static UInt32 __stdcall CheckThread(void *userObj);
		void ReleaseSess(SessionInfo *sess);
	public:
		TFTPServer(Net::SocketFactory *sockf, UInt16 port, IO::LogTool *log, const UTF8Char *path);
		~TFTPServer();

		Bool IsError();
	};
}
#endif
