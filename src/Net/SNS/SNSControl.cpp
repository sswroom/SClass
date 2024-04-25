#include "Stdafx.h"
#include "IO/Path.h"
#include "Net/SNS/SNSControl.h"

Text::CStringNN Net::SNS::SNSControl::SNSTypeGetName(Net::SNS::SNSControl::SNSType snsType)
{
	switch (snsType)
	{
	case ST_RSS:
		return CSTR("RSS");
	case ST_TWITTER:
		return CSTR("Twitter");
	case ST_7GOGO:
		return CSTR("7gogo");
	case ST_INSTAGRAM:
		return CSTR("Instagram");
	case ST_UNKNOWN:
	default:
		return CSTR("Unknown");
	}
}

Net::SNS::SNSControl::SNSType Net::SNS::SNSControl::SNSTypeFromName(Text::CStringNN name)
{
	if (name.Equals(UTF8STRC("RSS")))
	{
		return ST_RSS;
	}
	else if (name.Equals(UTF8STRC("Twitter")))
	{
		return ST_TWITTER;
	}
	else if (name.Equals(UTF8STRC("7gogo")))
	{
		return ST_7GOGO;
	}
	else if (name.Equals(UTF8STRC("Instagram")))
	{
		return ST_INSTAGRAM;
	}
	return Net::SNS::SNSControl::ST_UNKNOWN;
}

Net::SNS::SNSControl::SNSItem *Net::SNS::SNSControl::CreateItem(NN<Text::String> id, Int64 msgTime, Text::String *title, NN<Text::String> message, Text::String *msgLink, Text::String *imgURL, Text::String *videoURL)
{
	Net::SNS::SNSControl::SNSItem *item = MemAlloc(Net::SNS::SNSControl::SNSItem, 1);
	item->id = id->Clone();
	item->msgTime = msgTime;
	item->title = SCOPY_STRING(title);
	item->message = message->Clone();
	item->msgLink = SCOPY_STRING(msgLink);
	item->imgURL = SCOPY_STRING(imgURL);
	item->videoURL = SCOPY_STRING(videoURL);
	return item;
}

void Net::SNS::SNSControl::FreeItem(Net::SNS::SNSControl::SNSItem *item)
{
	item->id->Release();
	SDEL_STRING(item->title);
	item->message->Release();
	SDEL_STRING(item->msgLink);
	SDEL_STRING(item->imgURL);
	SDEL_STRING(item->videoURL);
	MemFree(item);
}
