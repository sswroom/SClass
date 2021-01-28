#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/HKOWeather.h"
#include "Net/HTTPClient.h"
#include "IO/MemoryStream.h"
#include "Text/MyString.h"
#include "Text/XMLDOM.h"

Net::HKOWeather::WeatherSignal Net::HKOWeather::String2Signal(const UTF8Char *textMessage)
{
	WeatherSignal signal;
	signal = Net::HKOWeather::WS_NONE;
	if (Text::StrIndexOf(textMessage, (const UTF8Char*)"Standby Signal, No. 1 Tropical Cyclone Warning Signal") >= 0)
	{
		signal = (Net::HKOWeather::WeatherSignal)(signal | Net::HKOWeather::WS_TYPHOON_1);
	}
	else if (Text::StrIndexOf(textMessage, (const UTF8Char*)"Strong Wind Signal, No. 3 Tropical Cyclone Warning Signal") >= 0)
	{
		signal = (Net::HKOWeather::WeatherSignal)(signal | Net::HKOWeather::WS_TYPHOON_3);
	}
	else if (Text::StrIndexOf(textMessage, (const UTF8Char*)"No. 8 Southeast Gale or Storm Signal Tropical Cyclone Warning Signal") >= 0)
	{
		signal = (Net::HKOWeather::WeatherSignal)(signal | Net::HKOWeather::WS_TYPHOON_8SE);
	}
	else if (Text::StrIndexOf(textMessage, (const UTF8Char*)"No. 8 Northeast Gale or Storm Signal Tropical Cyclone Warning Signal") >= 0)
	{
		signal = (Net::HKOWeather::WeatherSignal)(signal | Net::HKOWeather::WS_TYPHOON_8NE);
	}
	else if (Text::StrIndexOf(textMessage, (const UTF8Char*)"No. 8 Northwest Gale or Storm Signal Tropical Cyclone Warning Signal") >= 0)
	{
		signal = (Net::HKOWeather::WeatherSignal)(signal | Net::HKOWeather::WS_TYPHOON_8NW);
	}
	else if (Text::StrIndexOf(textMessage, (const UTF8Char*)"No. 8 Southwest Gale or Storm Signal Tropical Cyclone Warning Signal") >= 0)
	{
		signal = (Net::HKOWeather::WeatherSignal)(signal | Net::HKOWeather::WS_TYPHOON_8SW);
	}
	else if (Text::StrIndexOf(textMessage, (const UTF8Char*)"Hurricane Signal, No. 10 Tropical Cyclone Warning Signal") >= 0)
	{
		signal = (Net::HKOWeather::WeatherSignal)(signal | Net::HKOWeather::WS_TYPHOON_10);
	}
	if (Text::StrIndexOf(textMessage, (const UTF8Char*)"Thunderstorm Warning") >= 0)
	{
		signal = (Net::HKOWeather::WeatherSignal)(signal | Net::HKOWeather::WS_THUNDERSTORM);
	}
	if (Text::StrIndexOf(textMessage, (const UTF8Char*)"Very Hot Weather Warning") >= 0)
	{
		signal = (Net::HKOWeather::WeatherSignal)(signal | Net::HKOWeather::WS_VERYHOT);
	}
	if (Text::StrIndexOf(textMessage, (const UTF8Char*)"Strong Monsoon Signal") >= 0)
	{
		signal = (Net::HKOWeather::WeatherSignal)(signal | Net::HKOWeather::WS_STRONGMONSOON);
	}
	if (Text::StrIndexOf(textMessage, (const UTF8Char*)"Cold Weather Warning") >= 0)
	{
		signal = (Net::HKOWeather::WeatherSignal)(signal | Net::HKOWeather::WS_COLD);
	}
	if (Text::StrIndexOf(textMessage, (const UTF8Char*)"Red Fire Danger Warning") >= 0)
	{
		signal = (Net::HKOWeather::WeatherSignal)(signal | Net::HKOWeather::WS_FIRE_RED);
	}
	if (Text::StrIndexOf(textMessage, (const UTF8Char*)"Landslip Warning") >= 0)
	{
		signal = (Net::HKOWeather::WeatherSignal)(signal | Net::HKOWeather::WS_LANDSLIP);
	}
	return signal;
}

Net::HKOWeather::WeatherSignal Net::HKOWeather::GetSignalSummary(Net::SocketFactory *sockf, Text::EncodingFactory *encFact)
{
	UInt8 buff[1024];
	UInt8 *mbuff;
	IO::MemoryStream *mstm;
	Net::HTTPClient *cli;
	Net::HKOWeather::WeatherSignal signal;
	UOSInt i;

	cli = Net::HTTPClient::CreateClient(sockf, (const UTF8Char*)"http://rss.weather.gov.hk/rss/WeatherWarningSummary.xml", "GET", false);
	if (cli->IsError())
	{
		DEL_CLASS(cli);
		return Net::HKOWeather::WS_UNKNOWN;
	}
	NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"Net.HKOWeather.GetSignalSummary"));
	while ((i = cli->Read(buff, 1024)) > 0)
	{
		mstm->Write(buff, i);
	}
	DEL_CLASS(cli);

	Text::XMLDocument *doc;
	mbuff = mstm->GetBuff(&i);
	NEW_CLASS(doc, Text::XMLDocument());
	doc->ParseBuff(encFact, mbuff, i);
	Text::XMLNode **nodes = doc->SearchNode((const UTF8Char*)"/rss/channel/item/description", &i);
	Text::XMLNode *n;
	Text::XMLNode *n2;

	signal = Net::HKOWeather::WS_UNKNOWN;
	if (i > 0)
	{
		n = nodes[0];
		i = n->GetChildCnt();
		while (i-- > 0)
		{
			n2 = n->GetChild(i);
			if (n2->GetNodeType() == Text::XMLNode::NT_CDATA)
			{
				signal = String2Signal(n2->value);

				break;
			}
		}
	}
	doc->ReleaseSearch(nodes);
	DEL_CLASS(doc);

	DEL_CLASS(mstm);
	return signal;
}

Net::HKOWeather::HKOWeather(Net::SocketFactory *sockf, Text::EncodingFactory *encFact, UpdateHandler hdlr)
{
	this->sockf = sockf;
	this->encFact = encFact;
	this->hdlr = hdlr;
	NEW_CLASS(this->rss, Net::RSSReader((const UTF8Char*)"http://rss.weather.gov.hk/rss/WeatherWarningSummary.xml", this->sockf, 10, this));
}

Net::HKOWeather::~HKOWeather()
{
	DEL_CLASS(this->rss);
}

void Net::HKOWeather::ItemAdded(Net::RSSItem *item)
{
	WeatherSignal signal = String2Signal(item->description);
	if (signal != this->currSignal)
	{
		this->currSignal = signal;
		this->hdlr(this->currSignal);
	}
}

void Net::HKOWeather::ItemRemoved(Net::RSSItem *item)
{
}

Net::HKOWeather::WeatherSignal Net::HKOWeather::GetCurrentSignal()
{
	return this->currSignal;
}
