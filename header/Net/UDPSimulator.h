#ifndef _SM_NET_UDPSIMULATOR
#define _SM_NET_UDPSIMULATOR
#include "Data/DateTime.h"

namespace Net
{
	class UDPSimulator
	{
	public:
		typedef Bool (__stdcall *UDPPacketHdlr)(UInt32 ip, UInt16 port, UInt8 *buff, OSInt dataSize, void *userData, Data::DateTime *recvTime); //return true to continue

	private:
		const WChar *logFileName;
		const WChar *rawFileName;
		UDPPacketHdlr hdlr;
		void *userObj;

	public:
		UDPSimulator(const WChar *logFileName, const WChar *rawFileName, UDPPacketHdlr hdlr, void *userObj);
		~UDPSimulator();

		void Run();
	};
};
#endif
