#ifndef _SM_NET_TFTPSERVER
#define _SM_NET_TFTPSERVER
#include "Data/FastMap.h"
#include "IO/LogTool.h"
#include "Net/UDPServer.h"
#include "Sync/Mutex.h"
#include "Text/String.h"

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
			NotNullPtr<Text::String> fileName;
		} SessionInfo;
		
	private:
		NotNullPtr<IO::LogTool> log;
		Net::UDPServer *svr;
		Net::UDPServer *dataSvr;
		NotNullPtr<Text::String> path;
		Sync::Mutex mut;
		Data::FastMap<UInt64, SessionInfo*> sessMap;
		Bool threadToStop;
		Bool threadRunning;
		Sync::Event chkEvt;

		static void __stdcall OnCommandPacket(NotNullPtr<const Net::SocketUtil::AddressInfo> addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, AnyType userData);
		static void __stdcall OnDataPacket(NotNullPtr<const Net::SocketUtil::AddressInfo> addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, AnyType userData);
		static UInt32 __stdcall CheckThread(void *userObj);
		void ReleaseSess(SessionInfo *sess);
	public:
		TFTPServer(NotNullPtr<Net::SocketFactory> sockf, UInt16 port, NotNullPtr<IO::LogTool> log, Text::CString path);
		~TFTPServer();

		Bool IsError();
	};
}
#endif
