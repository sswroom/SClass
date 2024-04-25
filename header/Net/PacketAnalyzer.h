#ifndef _SM_NET_PACKETANALYZER
#define _SM_NET_PACKETANALYZER
#include "IO/FileAnalyse/FrameDetailHandler.h"
#include "Text/StringBuilderUTF8.h"

namespace Net
{
	class PacketAnalyzer
	{
	public:
		static Bool PacketDataGetName(UInt32 linkType, const UInt8 *packet, UOSInt packetSize, NN<Text::StringBuilderUTF8> sb);
		static void PacketDataGetDetail(UInt32 linkType, const UInt8 *packet, UOSInt packetSize, NN<Text::StringBuilderUTF8> sb);
		static void PacketDataGetDetail(UInt32 linkType, const UInt8 *packet, UOSInt packetSize, UInt32 frameOfst, NN<IO::FileAnalyse::FrameDetailHandler> frame);
	};
}
#endif
