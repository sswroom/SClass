#ifndef _SM_NET_UDPSIMULATOR
#define _SM_NET_UDPSIMULATOR
#include "AnyType.h"
#include "Data/DateTime.h"
#include "Text/String.h"

namespace Net
{
	class UDPSimulator
	{
	public:
		typedef Bool (CALLBACKFUNC UDPPacketHdlr)(UInt32 ip, UInt16 port, UnsafeArray<UInt8> buff, IntOS dataSize, AnyType userData, NN<Data::DateTime> recvTime); //return true to continue

	private:
		NN<Text::String> logFileName;
		NN<Text::String> rawFileName;
		UDPPacketHdlr hdlr;
		AnyType userObj;

	public:
		UDPSimulator(Text::CStringNN logFileName, Text::CStringNN rawFileName, UDPPacketHdlr hdlr, AnyType userObj);
		~UDPSimulator();

		void Run();
	};
}
#endif
