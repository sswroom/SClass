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

NN<Net::SNS::SNSControl::SNSItem> Net::SNS::SNSControl::CreateItem(NN<Text::String> id, Int64 msgTime, Optional<Text::String> title, NN<Text::String> message, Optional<Text::String> msgLink, Optional<Text::String> imgURL, Optional<Text::String> videoURL)
{
	NN<Net::SNS::SNSControl::SNSItem> item = MemAllocNN(Net::SNS::SNSControl::SNSItem);
	item->id = id->Clone();
	item->msgTime = msgTime;
	item->title = Text::String::CopyOrNull(title);
	item->message = message->Clone();
	item->msgLink = Text::String::CopyOrNull(msgLink);
	item->imgURL = Text::String::CopyOrNull(imgURL);
	item->videoURL = Text::String::CopyOrNull(videoURL);
	return item;
}

void Net::SNS::SNSControl::FreeItem(NN<Net::SNS::SNSControl::SNSItem> item)
{
	item->id->Release();
	OPTSTR_DEL(item->title);
	item->message->Release();
	OPTSTR_DEL(item->msgLink);
	OPTSTR_DEL(item->imgURL);
	OPTSTR_DEL(item->videoURL);
	MemFreeNN(item);
}
