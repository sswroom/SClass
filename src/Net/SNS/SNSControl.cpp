#include "Stdafx.h"
#include "IO/Path.h"
#include "Net/SNS/SNSControl.h"

const UTF8Char *Net::SNS::SNSControl::SNSTypeGetName(Net::SNS::SNSControl::SNSType snsType)
{
	switch (snsType)
	{
	case ST_RSS:
		return (const UTF8Char*)"RSS";
	case ST_TWITTER:
		return (const UTF8Char*)"Twitter";
	case ST_7GOGO:
		return (const UTF8Char*)"7gogo";
	case ST_INSTAGRAM:
		return (const UTF8Char*)"Instagram";
	case ST_UNKNOWN:
	default:
		return (const UTF8Char*)"Unknown";
	}
}

Net::SNS::SNSControl::SNSType Net::SNS::SNSControl::SNSTypeFromName(const UTF8Char *name)
{
	if (Text::StrEquals(name, (const UTF8Char*)"RSS"))
	{
		return ST_RSS;
	}
	else if (Text::StrEquals(name, (const UTF8Char*)"Twitter"))
	{
		return ST_TWITTER;
	}
	else if (Text::StrEquals(name, (const UTF8Char*)"7gogo"))
	{
		return ST_7GOGO;
	}
	else if (Text::StrEquals(name, (const UTF8Char*)"Instagram"))
	{
		return ST_INSTAGRAM;
	}
	return Net::SNS::SNSControl::ST_UNKNOWN;
}

Net::SNS::SNSControl::SNSItem *Net::SNS::SNSControl::CreateItem(const UTF8Char *id, Int64 msgTime, const UTF8Char *title, const UTF8Char *message, const UTF8Char *msgLink, const UTF8Char *imgURL, const UTF8Char *videoURL)
{
	Net::SNS::SNSControl::SNSItem *item = MemAlloc(Net::SNS::SNSControl::SNSItem, 1);
	item->id = Text::StrCopyNew(id);
	item->msgTime = msgTime;
	item->title = title?Text::StrCopyNew(title):0;
	item->message = Text::StrCopyNew(message); 
	item->msgLink = msgLink?Text::StrCopyNew(msgLink):0;
	item->imgURL = imgURL?Text::StrCopyNew(imgURL):0;
	item->videoURL = videoURL?Text::StrCopyNew(videoURL):0;
	return item;
}

void Net::SNS::SNSControl::FreeItem(Net::SNS::SNSControl::SNSItem *item)
{
	Text::StrDelNew(item->id);
	SDEL_TEXT(item->title);
	Text::StrDelNew(item->message);
	SDEL_TEXT(item->msgLink);
	SDEL_TEXT(item->imgURL);
	SDEL_TEXT(item->videoURL);
	MemFree(item);
}
