#ifndef _SM_NET_UDPSERVER
#define _SM_NET_UDPSERVER
#include "IO/LogTool.h"
#include "IO/FileStream.h"
#include "Data/DateTime.h"
#include "Sync/Event.h"
#include "Net/SocketFactory.h"

namespace Net
{
	class UDPServer
	{
	public:
		typedef void (__stdcall *UDPPacketHdlr)(const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, void *userData);

		typedef struct
		{
			UDPServer *me;
			Bool threadRunning;
			Sync::Event *evt;
			Bool toStop;
		} ThreadStat;

	private:
		Net::SocketFactory *sockf;
		Net::AddrType addrType;
		Socket *socV4;
		Socket *socV6;
		UDPPacketHdlr hdlr;
		const UTF8Char *logPrefix;
		void *userData;
		Int32 recvCnt;

		ThreadStat *v4threadStats;
		ThreadStat *v6threadStats;
		UOSInt threadCnt;
		UInt16 port;
		Sync::Event *ctrlEvt;

		IO::LogTool *msgLog;
		const UTF8Char *msgPrefix;

		Sync::Mutex *logFileMut;
		IO::FileStream *logFileR;
		IO::FileStream *logFileS;
		Data::DateTime *logDateR;
		Data::DateTime *logDateS;

	private:
		static UInt32 __stdcall DataV4Thread(void *obj);
		static UInt32 __stdcall DataV6Thread(void *obj);

	public:
		UDPServer(Net::SocketFactory *sockf, Net::SocketUtil::AddressInfo *bindAddr, UInt16 port, const UTF8Char *rawLogPrefix, UDPPacketHdlr hdlr, void *userData, IO::LogTool *msgLog, const UTF8Char *msgPrefix, UOSInt workerCnt, Bool reuseAddr);
		virtual ~UDPServer();

		UInt16 GetPort();
		Bool IsError();
		Bool SupportV6();
		virtual Bool SendTo(const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UInt8 *buff, UOSInt dataSize);
		Int32 GetRecvCnt();
		void AddMulticastIP(UInt32 ip);
		void SetBuffSize(Int32 buffSize);
		void SetBroadcast(Bool val);
	};
}
#endif
