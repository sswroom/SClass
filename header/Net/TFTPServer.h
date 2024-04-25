#ifndef _SM_NET_TFTPSERVER
#define _SM_NET_TFTPSERVER
#include "AnyType.h"
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
			NN<Text::String> fileName;
		} SessionInfo;
		
	private:
		NN<IO::LogTool> log;
		Net::UDPServer *svr;
		Net::UDPServer *dataSvr;
		NN<Text::String> path;
		Sync::Mutex mut;
		Data::FastMap<UInt64, SessionInfo*> sessMap;
		Bool threadToStop;
		Bool threadRunning;
		Sync::Event chkEvt;

		static void __stdcall OnCommandPacket(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, AnyType userData);
		static void __stdcall OnDataPacket(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, AnyType userData);
		static UInt32 __stdcall CheckThread(AnyType userObj);
		void ReleaseSess(SessionInfo *sess);
	public:
		TFTPServer(NN<Net::SocketFactory> sockf, UInt16 port, NN<IO::LogTool> log, Text::CString path);
		~TFTPServer();

		Bool IsError();
	};
}
#endif
