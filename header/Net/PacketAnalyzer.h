#ifndef _SM_NET_PACKETANALYZER
#define _SM_NET_PACKETANALYZER
#include "Text/StringBuilderUTF.h"

namespace Net
{
	class PacketAnalyzer
	{
	public:
		static Bool PacketDataGetName(UInt32 linkType, const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb);
		static void PacketDataGetDetail(UInt32 linkType, const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb);
	};
}
#endif
