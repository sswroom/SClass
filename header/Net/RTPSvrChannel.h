#ifndef _SM_NET_RTPSVRCHANNEL
#define _SM_NET_RTPSVRCHANNEL
#include "Net/RTPSessionController.h"
#include "Net/UDPServer.h"
#include "Sync/Mutex.h"

namespace Net
{
	class IRTPPLHandler;

	class RTPSvrChannel
	{
	private:
		Net::UDPServer *rtpUDP;
		Net::UDPServer *rtcpUDP;
		Net::SocketUtil::AddressInfo targetAddr;
		IO::LogTool log;
		UInt16 targetPort;
		Int32 ssrc;
		UOSInt threadCnt;
		Int32 lastSSRC;
		Int32 seqNum;
		Net::RTPSessionController *sessCtrl;

	private:
		static void __stdcall PacketHdlr(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, AnyType userData);
		static void __stdcall PacketCtrlHdlr(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, AnyType userData);

	public:
		RTPSvrChannel(NN<Net::SocketFactory> sockf, UInt16 port, Int32 ssrc, NN<const Net::SocketUtil::AddressInfo> targetAddr, UInt16 targetPort, Net::RTPSessionController *sessCtrl);
		~RTPSvrChannel();

		NN<const Net::SocketUtil::AddressInfo> GetTargetAddr();
		UInt16 GetPort();
		Int32 GetSeqNum();
		Bool SendPacket(Int32 payloadType, Int32 ts, UInt8 *buff, UOSInt dataSize, Bool marker);
		Bool SendControl(UInt8 *buff, UOSInt dataSize);
	};
}
#endif
