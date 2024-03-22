#ifndef _SM_NET_UDPSERVER
#define _SM_NET_UDPSERVER
#include "AnyType.h"
#include "Data/CallbackStorage.h"
#include "IO/LogTool.h"
#include "IO/FileStream.h"
#include "Data/Timestamp.h"
#include "Sync/Event.h"
#include "Net/SocketFactory.h"
#include "Text/CString.h"

namespace Net
{
	class UDPServer
	{
	public:
		typedef void (__stdcall *UDPPacketHdlr)(NotNullPtr<const Net::SocketUtil::AddressInfo> addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, AnyType userData);

		typedef struct
		{
			UDPServer *me;
			Bool threadRunning;
			Sync::Event *evt;
			Bool toStop;
		} ThreadStat;

	private:
		NotNullPtr<Net::SocketFactory> sockf;
		Net::AddrType addrType;
		Socket *socV4;
		Socket *socV6;
		Data::CallbackStorage<UDPPacketHdlr> hdlr;
		Optional<Text::String> logPrefix;
		Int32 recvCnt;

		ThreadStat *v4threadStats;
		ThreadStat *v6threadStats;
		UOSInt threadCnt;
		UInt16 port;
		Sync::Event ctrlEvt;

		NotNullPtr<IO::LogTool> msgLog;
		Optional<Text::String> msgPrefix;

		Sync::Mutex logFileMut;
		IO::FileStream *logFileR;
		IO::FileStream *logFileS;
		Data::Timestamp logDateR;
		Data::Timestamp logDateS;

	private:
		static UInt32 __stdcall DataV4Thread(AnyType obj);
		static UInt32 __stdcall DataV6Thread(AnyType obj);

	public:
		UDPServer(NotNullPtr<Net::SocketFactory> sockf, Net::SocketUtil::AddressInfo *bindAddr, UInt16 port, Text::CString rawLogPrefix, UDPPacketHdlr hdlr, AnyType userData, NotNullPtr<IO::LogTool> msgLog, Text::CString msgPrefix, UOSInt workerCnt, Bool reuseAddr);
		virtual ~UDPServer();

		UInt16 GetPort();
		Bool IsError();
		Bool SupportV6();
		virtual Bool SendTo(NotNullPtr<const Net::SocketUtil::AddressInfo> addr, UInt16 port, const UInt8 *buff, UOSInt dataSize);
		Int32 GetRecvCnt();
		void AddMulticastIP(UInt32 ip);
		void SetBuffSize(Int32 buffSize);
		void SetBroadcast(Bool val);
	};
}
#endif
