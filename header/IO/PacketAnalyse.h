#ifndef _SM_IO_PACKETANALYSE
#define _SM_IO_PACKETANALYSE

namespace IO
{
	class PacketAnalyse
	{
	public:
		enum class LinkType
		{
			Null = 0,
			Ethernet = 1,
			Linux = 113,
			Bluetooth = 201
		};
	};
}
#endif
