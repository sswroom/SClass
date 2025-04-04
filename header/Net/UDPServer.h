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
		typedef void (CALLBACKFUNC UDPPacketHdlr)(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::ByteArrayR data, AnyType userData);

		typedef struct
		{
			NN<UDPServer> me;
			Bool threadRunning;
			NN<Sync::Event> evt;
			Bool toStop;
		} ThreadStat;

	private:
		NN<Net::SocketFactory> sockf;
		Net::AddrType addrType;
		Optional<Socket> socV4;
		Optional<Socket> socV6;
		Data::CallbackStorage<UDPPacketHdlr> hdlr;
		Optional<Text::String> logPrefix;
		Int32 recvCnt;

		UnsafeArrayOpt<ThreadStat> v4threadStats;
		UnsafeArrayOpt<ThreadStat> v6threadStats;
		UOSInt threadCnt;
		UInt16 port;
		Sync::Event ctrlEvt;

		NN<IO::LogTool> msgLog;
		Optional<Text::String> msgPrefix;

		Sync::Mutex logFileMut;
		Optional<IO::FileStream> logFileR;
		Optional<IO::FileStream> logFileS;
		Data::Timestamp logDateR;
		Data::Timestamp logDateS;

	private:
		static UInt32 __stdcall DataV4Thread(AnyType obj);
		static UInt32 __stdcall DataV6Thread(AnyType obj);

	public:
		UDPServer(NN<Net::SocketFactory> sockf, Optional<Net::SocketUtil::AddressInfo> bindAddr, UInt16 port, Text::CString rawLogPrefix, UDPPacketHdlr hdlr, AnyType userData, NN<IO::LogTool> msgLog, Text::CString msgPrefix, UOSInt workerCnt, Bool reuseAddr);
		virtual ~UDPServer();

		UInt16 GetPort();
		Bool IsError();
		Bool SupportV6();
		virtual Bool SendTo(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, UnsafeArray<const UInt8> buff, UOSInt dataSize);
		Int32 GetRecvCnt();
		void AddMulticastIP(UInt32 ip);
		void SetBuffSize(Int32 buffSize);
		void SetBroadcast(Bool val);
	};
}
#endif
