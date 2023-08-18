#ifndef _SM_IO_RAWMONITOR
#define _SM_IO_RAWMONITOR
#include "Text/CString.h"

namespace IO
{
	class RAWMonitor
	{
	public:
		enum class LinkType
		{
			Null = 0,
			Ethernet = 1,
			AX25 = 3,
			IEEE802_5 = 6,
			ARCNET = 7,
			SLIP = 8,
			PPP = 9,
			IPv4 = 101,
			Linux = 113,
			Bluetooth = 201
		};
	public:
		virtual ~RAWMonitor() {};

		virtual LinkType GetLinkType() const = 0;
		virtual Bool IsError() = 0;
		virtual void Close() = 0;
		virtual UOSInt GetMTU() = 0;
		virtual UOSInt NextPacket(UInt8 *buff, Int64 *timeTicks) = 0;

		static Text::CString LinkTypeGetName(UInt32 linkType);
	};
}
#endif
