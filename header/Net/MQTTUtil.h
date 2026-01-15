#ifndef _SM_NET_MQTTUTIL
#define _SM_NET_MQTTUTIL
#include "Text/CString.h"
namespace Net
{
	class MQTTUtil
	{
	public:
		static Bool TopicValid(UnsafeArray<const UTF8Char> topic);
		static Bool TopicMatch(UnsafeArray<const UTF8Char> topic, UIntOS topicLen, UnsafeArray<const UTF8Char> subscribeTopic, UIntOS suibscribeTopicLen);
		static Text::CString PacketTypeGetName(UInt8 packetType);
	};
}
#endif
