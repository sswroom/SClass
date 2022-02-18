#ifndef _SM_NET_MQTTCLIENT
#define _SM_NET_MQTTCLIENT
#include "Text/CString.h"

namespace Net
{
	class MQTTClient
	{
	public:
		virtual ~MQTTClient() {};

		virtual Bool Subscribe(Text::CString topic) = 0;
		virtual Bool Publish(Text::CString topic, Text::CString message) = 0;
	};
}
#endif
