#ifndef _SM_NET_LORAGATEWAY
#define _SM_NET_LORAGATEWAY
#include "AnyType.h"
#include "Net/UDPServer.h"
#include "Sync/Event.h"

namespace Net
{
	class LoRaGateway
	{
	private:
		NN<Net::SocketFactory> sockf;
		Net::UDPServer udp;
		Net::SocketUtil::AddressInfo svrAddr;
		UInt16 svrPort;
		UInt8 gatewayEUI[8];
		UInt16 tokenNext;
		Sync::Mutex tokenMut;
		UInt32 pullInterval;
		UInt32 statInterval;
		Data::Timestamp lastPullTime;
		Data::Timestamp lastStatTime;
		Bool threadToStop;
		Bool threadRunning;
		Sync::Event *threadEvt;
		Sync::Event mainEvt;

		Bool hasPos;
		Double lat;
		Double lon;
		Int32 altitude;

		static void __stdcall OnUDPPacket(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, AnyType userData);
		static UInt32 __stdcall PullThread(AnyType userObj);

		Bool SendPullData();
		Bool SendStatData();
	public:
		LoRaGateway(NN<Net::SocketFactory> sockf, NN<const Net::SocketUtil::AddressInfo> svrAddr, UInt16 svrPort, const UInt8 *gatewayEUI, NN<IO::LogTool> log);
		~LoRaGateway();

		void UpdatePos(Double lat, Double lon, Int32 altitude);
		Bool IsError();
		UInt16 NextToken();
		Bool SendPushData(UnsafeArray<const UInt8> data, UOSInt dataLeng);
	};
}
#endif
