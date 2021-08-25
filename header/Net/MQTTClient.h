#ifndef _SM_NET_MQTTCLIENT
#define _SM_NET_MQTTCLIENT

namespace Net
{
	class MQTTClient
	{
	public:
		virtual ~MQTTClient() {};

		virtual Bool Subscribe(const UTF8Char *topic) = 0;
		virtual Bool Publish(const UTF8Char *topic, const UTF8Char *message) = 0;
		
	};
}
#endif
