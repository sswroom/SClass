#ifndef _SM_NET_MQTTCLIENT
#define _SM_NET_MQTTCLIENT
#include "Text/CString.h"

namespace Net
{
	class MQTTClient
	{
	public:
		virtual ~MQTTClient() {};

		virtual Bool Subscribe(Text::CStringNN topic) = 0;
		virtual Bool Publish(Text::CStringNN topic, Text::CStringNN message, Bool dup, UInt8 qos, Bool retain) = 0;
	};
}
#endif
