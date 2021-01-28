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
		Int32 targetIP;
		UInt16 targetPort;
		Int32 ssrc;
		OSInt threadCnt;
		Int32 lastSSRC;
		Int32 seqNum;
		Net::RTPSessionController *sessCtrl;

	private:
		static void __stdcall PacketHdlr(UInt32 ip, UInt16 port, UInt8 *buff, OSInt dataSize, void *userData);
		static void __stdcall PacketCtrlHdlr(UInt32 ip, UInt16 port, UInt8 *buff, OSInt dataSize, void *userData);

	public:
		RTPSvrChannel(Net::SocketFactory *sockf, UInt16 port, Int32 ssrc, Int32 targetIP, UInt16 targetPort, Net::RTPSessionController *sessCtrl);
		~RTPSvrChannel();

		Int32 GetTargetIP();
		UInt16 GetPort();
		Int32 GetSeqNum();
		Bool SendPacket(Int32 payloadType, Int32 ts, UInt8 *buff, UInt32 dataSize, Bool marker);
		Bool SendControl(UInt8 *buff, UInt32 dataSize);
	};
};
#endif
