#ifndef _SM_NET_MQTTUTIL
#define _SM_NET_MQTTUTIL
#include "Text/CString.h"
namespace Net
{
	class MQTTUtil
	{
	public:
		static Bool TopicValid(const UTF8Char *topic);
		static Bool TopicMatch(const UTF8Char *topic, UOSInt topicLen, const UTF8Char *subscribeTopic);
		static Text::CString PacketTypeGetName(UInt8 packetType);
	};
}
#endif
