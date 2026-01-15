#include "Stdafx.h"
#include "Net/MQTTUtil.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

Bool Net::MQTTUtil::TopicValid(UnsafeArray<const UTF8Char> topic)
{
	UIntOS i;
	UIntOS j;
	i = Text::StrIndexOfChar(topic, '#');
	if (i != INVALID_INDEX)
	{
		if (topic[i + 1])
		{
			return false;
		}
		if (i > 0 && topic[i - 1] != '/')
		{
			return false;
		}
	}
	i = 0;
	while (true)
	{
		j = Text::StrIndexOfChar(&topic[i], '+');
		if (j == INVALID_INDEX)
			break;
		if (i + j > 0 && topic[i + j - 1] != '/')
		{
			return false;
		}
		if (topic[i + j + 1] != '/' && topic[i + j + 1] != 0)
		{
			return false;
		}
		i += j + 2;
	}
	return true;
}

Bool Net::MQTTUtil::TopicMatch(UnsafeArray<const UTF8Char> topic, UIntOS topicLen, UnsafeArray<const UTF8Char> subscribeTopic, UIntOS subscribeTopicLen)
{
	if (subscribeTopic[0] == '#' && subscribeTopic[1] == 0)
	{
		if (!Text::StrStartsWithC(topic, topicLen, UTF8STRC("$SYS/")))
		{
			return true;
		}
		return false;
	}
	UIntOS i;
	while (true)
	{
		i = Text::StrIndexOfCharC(subscribeTopic, subscribeTopicLen, '+');
		if (i == INVALID_INDEX)
			break;
		if (i > 0)
		{
			if (!Text::StrStartsWithC(topic, topicLen, subscribeTopic, (UIntOS)i))
			{
				return false;
			}
			topic += i;
			topicLen -= i;
			subscribeTopic += i;
			subscribeTopicLen -= i;
		}
		i = Text::StrIndexOfCharC(topic, topicLen, '/');
		if (subscribeTopic[1] == 0)
		{
			return (i == INVALID_INDEX);
		}
		else if (i == INVALID_INDEX)
		{
			return false;
		}
		subscribeTopic++;
		subscribeTopicLen--;
		topic += i;
		topicLen -= i;
	}
	i = Text::StrIndexOfCharC(subscribeTopic, subscribeTopicLen, '#');
	if (i == INVALID_INDEX)
	{
		return Text::StrEqualsC(topic, topicLen, subscribeTopic, subscribeTopicLen);
	}
	else if (i == 0)
	{
		return true;
	}

	if (!Text::StrStartsWithC(topic, topicLen, subscribeTopic, (UIntOS)i))
	{
		return false;
	}
	return true;
}

Text::CString Net::MQTTUtil::PacketTypeGetName(UInt8 packetType)
{
	switch (packetType)
	{
	case 1:
		return CSTR("CONNECT");
	case 2:
		return CSTR("CONNACK");
	case 3:
		return CSTR("PUBLISH");
	case 4:
		return CSTR("PUBACK");
	case 5:
		return CSTR("PUBREC");
	case 6:
		return CSTR("PUBREL");
	case 7:
		return CSTR("PUBCOMP");
	case 8:
		return CSTR("SUBSCRIBE");
	case 9:
		return CSTR("SUBACK");
	case 10:
		return CSTR("UNSUBSCRIBE");
	case 11:
		return CSTR("UNSUBACK");
	case 12:
		return CSTR("PINGREQ");
	case 13:
		return CSTR("PINGRESP");
	case 14:
		return CSTR("DISCONNECT");
	case 15:
		return CSTR("AUTH");
	default:
		return CSTR("Unknown");
	}
}
