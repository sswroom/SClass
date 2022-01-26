#ifndef _SM_IO_RAWMONITOR
#define _SM_IO_RAWMONITOR
#include "Text/CString.h"

namespace IO
{
	class RAWMonitor
	{
	public:
		enum LinkType
		{
			LT_NULL = 0,
			LT_ETHERNET = 1,
			LT_AX25 = 3,
			LT_IEEE802_5 = 6,
			LT_ARCNET = 7,
			LT_SLIP = 8,
			LT_PPP = 9,
			LT_IPV4 = 101,
			LT_LINUX = 113,
			LT_BLUETOOTH = 201
		};
	public:
		virtual ~RAWMonitor() {};

		virtual LinkType GetLinkType() = 0;
		virtual Bool IsError() = 0;
		virtual void Close() = 0;
		virtual UOSInt GetMTU() = 0;
		virtual UOSInt NextPacket(UInt8 *buff, Int64 *timeTicks) = 0;

		static Text::CString LinkTypeGetName(UInt32 linkType);
	};
}
#endif
