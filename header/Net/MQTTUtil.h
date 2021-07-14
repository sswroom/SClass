#ifndef _SM_NET_MQTTUTIL
#define _SM_NET_MQTTUTIL
namespace Net
{
	class MQTTUtil
	{
	public:
		static Bool TopicValid(const UTF8Char *topic);
		static Bool TopicMatch(const UTF8Char *topic, const UTF8Char *subscribeTopic);
	};
}
#endif
