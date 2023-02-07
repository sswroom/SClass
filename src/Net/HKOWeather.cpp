#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/MemoryReadingStream.h"
#include "Net/HKOWeather.h"
#include "Net/HTTPClient.h"
#include "Net/RSS.h"
#include "Net/UserAgentDB.h"
#include "IO/MemoryStream.h"
#include "Text/MyString.h"
#include "Text/UTF8Reader.h"
#include "Text/XMLDOM.h"

Net::HKOWeather::WeatherSignal Net::HKOWeather::String2Signal(Text::String *textMessage)
{
	WeatherSignal signal;
	signal = Net::HKOWeather::WS_NONE;
	if (textMessage->IndexOf(UTF8STRC("Standby Signal, No. 1 Tropical Cyclone Warning Signal")) != INVALID_INDEX)
	{
		signal = (Net::HKOWeather::WeatherSignal)(signal | Net::HKOWeather::WS_TYPHOON_1);
	}
	else if (textMessage->IndexOf(UTF8STRC("Strong Wind Signal, No. 3 Tropical Cyclone Warning Signal")) != INVALID_INDEX)
	{
		signal = (Net::HKOWeather::WeatherSignal)(signal | Net::HKOWeather::WS_TYPHOON_3);
	}
	else if (textMessage->IndexOf(UTF8STRC("No. 8 Southeast Gale or Storm Signal Tropical Cyclone Warning Signal")) != INVALID_INDEX)
	{
		signal = (Net::HKOWeather::WeatherSignal)(signal | Net::HKOWeather::WS_TYPHOON_8SE);
	}
	else if (textMessage->IndexOf(UTF8STRC("No. 8 Northeast Gale or Storm Signal Tropical Cyclone Warning Signal")) != INVALID_INDEX)
	{
		signal = (Net::HKOWeather::WeatherSignal)(signal | Net::HKOWeather::WS_TYPHOON_8NE);
	}
	else if (textMessage->IndexOf(UTF8STRC("No. 8 Northwest Gale or Storm Signal Tropical Cyclone Warning Signal")) != INVALID_INDEX)
	{
		signal = (Net::HKOWeather::WeatherSignal)(signal | Net::HKOWeather::WS_TYPHOON_8NW);
	}
	else if (textMessage->IndexOf(UTF8STRC("No. 8 Southwest Gale or Storm Signal Tropical Cyclone Warning Signal")) != INVALID_INDEX)
	{
		signal = (Net::HKOWeather::WeatherSignal)(signal | Net::HKOWeather::WS_TYPHOON_8SW);
	}
	else if (textMessage->IndexOf(UTF8STRC("Hurricane Signal, No. 10 Tropical Cyclone Warning Signal")) != INVALID_INDEX)
	{
		signal = (Net::HKOWeather::WeatherSignal)(signal | Net::HKOWeather::WS_TYPHOON_10);
	}
	if (textMessage->IndexOf(UTF8STRC("Thunderstorm Warning")) != INVALID_INDEX)
	{
		signal = (Net::HKOWeather::WeatherSignal)(signal | Net::HKOWeather::WS_THUNDERSTORM);
	}
	if (textMessage->IndexOf(UTF8STRC("Very Hot Weather Warning")) != INVALID_INDEX)
	{
		signal = (Net::HKOWeather::WeatherSignal)(signal | Net::HKOWeather::WS_VERYHOT);
	}
	if (textMessage->IndexOf(UTF8STRC("Strong Monsoon Signal")) != INVALID_INDEX)
	{
		signal = (Net::HKOWeather::WeatherSignal)(signal | Net::HKOWeather::WS_STRONGMONSOON);
	}
	if (textMessage->IndexOf(UTF8STRC("Cold Weather Warning")) != INVALID_INDEX)
	{
		signal = (Net::HKOWeather::WeatherSignal)(signal | Net::HKOWeather::WS_COLD);
	}
	if (textMessage->IndexOf(UTF8STRC("Red Fire Danger Warning")) != INVALID_INDEX)
	{
		signal = (Net::HKOWeather::WeatherSignal)(signal | Net::HKOWeather::WS_FIRE_RED);
	}
	if (textMessage->IndexOf(UTF8STRC("Landslip Warning")) != INVALID_INDEX)
	{
		signal = (Net::HKOWeather::WeatherSignal)(signal | Net::HKOWeather::WS_LANDSLIP);
	}
	return signal;
}

Net::HKOWeather::WeatherSignal Net::HKOWeather::GetSignalSummary(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::EncodingFactory *encFact)
{
	UInt8 buff[1024];
	UInt8 *mbuff;
	Net::HTTPClient *cli;
	Net::HKOWeather::WeatherSignal signal;
	UOSInt i;

	cli = Net::HTTPClient::CreateConnect(sockf, ssl, CSTR("http://rss.weather.gov.hk/rss/WeatherWarningSummary.xml"), Net::WebUtil::RequestMethod::HTTP_GET, false);
	if (cli->IsError())
	{
		DEL_CLASS(cli);
		return Net::HKOWeather::WS_UNKNOWN;
	}
	IO::MemoryStream mstm;
	while ((i = cli->Read(buff, 1024)) > 0)
	{
		mstm.Write(buff, i);
	}
	DEL_CLASS(cli);

	mbuff = mstm.GetBuff(&i);
	Text::XMLDocument doc;
	doc.ParseBuff(encFact, mbuff, i);
	Text::XMLNode **nodes = doc.SearchNode(CSTR("/rss/channel/item/description"), &i);
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
			if (n2->GetNodeType() == Text::XMLNode::NodeType::CData)
			{
				signal = String2Signal(n2->value);

				break;
			}
		}
	}
	doc.ReleaseSearch(nodes);
	return signal;
}

Bool Net::HKOWeather::GetCurrentTempRH(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Int32 *temperature, Int32 *rh)
{
	Bool succ = false;
	Net::RSS *rss;
	Text::CString userAgent = Net::UserAgentDB::FindUserAgent(Manage::OSInfo::OT_WINDOWS_NT64, Net::BrowserInfo::BT_FIREFOX);
	Text::String *ua = Text::String::New(userAgent);
	NEW_CLASS(rss, Net::RSS(CSTR("https://rss.weather.gov.hk/rss/CurrentWeather.xml"), ua, sockf, ssl));
	ua->Release();
	if (!rss->IsError())
	{
		if (rss->GetCount() > 0)
		{
			*temperature = INVALID_READING;
			*rh = INVALID_READING;
			UOSInt i;
			Net::RSSItem *item = rss->GetItem(0);
			IO::MemoryReadingStream mstm(item->description->v, item->description->leng);
			Text::UTF8Reader reader(&mstm);
			Text::StringBuilderUTF8 sb;
			while (reader.ReadLine(&sb, 512))
			{
				sb.Trim();
				if (sb.StartsWith(UTF8STRC("Air temperature : ")))
				{
					i = sb.IndexOf(' ', 18);
					if (i != INVALID_INDEX)
					{
						sb.TrimToLength(i);
						sb.ToCString().Substring(18).ToInt32(temperature);
					}
				}
				else if (sb.StartsWith(UTF8STRC("Relative Humidity : ")))
				{
					i = sb.IndexOf(' ', 20);
					if (i != INVALID_INDEX)
					{
						sb.TrimToLength(i);
						sb.ToCString().Substring(20).ToInt32(rh);
					}
				}
				sb.ClearStr();
			}
			succ = true;
		}
	}
	DEL_CLASS(rss);
	return succ;
}

Net::HKOWeather::HKOWeather(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::EncodingFactory *encFact, UpdateHandler hdlr)
{
	this->sockf = sockf;
	this->ssl = ssl;
	this->encFact = encFact;
	this->hdlr = hdlr;
	NEW_CLASS(this->rss, Net::RSSReader(CSTR("http://rss.weather.gov.hk/rss/WeatherWarningSummary.xml"), this->sockf, this->ssl, 10, this));
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
