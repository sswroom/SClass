#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/MemoryReadingStream.h"
#include "Net/HKOWeather.h"
#include "Net/HTTPClient.h"
#include "Net/HTTPJSONReader.h"
#include "Net/RSS.h"
#include "Net/UserAgentDB.h"
#include "IO/MemoryStream.h"
#include "Text/MyString.h"
#include "Text/UTF8Reader.h"
#include "Text/XMLDOM.h"

//#define VERBOSE
#if defined(VERBOSE)
#include <stdio.h>
#endif

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
	NotNullPtr<Net::HTTPClient> cli;
	Net::HKOWeather::WeatherSignal signal;
	UOSInt i;

	cli = Net::HTTPClient::CreateConnect(sockf, ssl, CSTR("http://rss.weather.gov.hk/rss/WeatherWarningSummary.xml"), Net::WebUtil::RequestMethod::HTTP_GET, false);
	if (cli->IsError())
	{
		cli.Delete();
		return Net::HKOWeather::WS_UNKNOWN;
	}
	IO::MemoryStream mstm;
	while ((i = cli->Read(buff, 1024)) > 0)
	{
		mstm.Write(buff, i);
	}
	cli.Delete();

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
	NotNullPtr<Text::String> ua = Text::String::New(userAgent);
	NEW_CLASS(rss, Net::RSS(CSTR("https://rss.weather.gov.hk/rss/CurrentWeather.xml"), ua.Ptr(), sockf, ssl, 30000));
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
			Text::UTF8Reader reader(mstm);
			Text::StringBuilderUTF8 sb;
			while (reader.ReadLine(sb, 512))
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

Bool Net::HKOWeather::GetWeatherForecast(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Language lang, WeatherForecast *weatherForecast)
{
	Text::CString url;
	switch (lang)
	{
	case Language::TC:
		url = CSTR("https://data.weather.gov.hk/weatherAPI/opendata/weather.php?dataType=fnd&lang=tc");
		break;
	case Language::SC:
		url = CSTR("https://data.weather.gov.hk/weatherAPI/opendata/weather.php?dataType=fnd&lang=sc");
		break;
	case Language::En:
	default:
		url = CSTR("https://data.weather.gov.hk/weatherAPI/opendata/weather.php?dataType=fnd&lang=en");
		break;
	}
#if defined(VERBOSE)
	printf("Getting Weather forecast from: %s\r\n", url.v);
#endif
	Text::JSONBase *json = Net::HTTPJSONReader::Read(sockf, ssl, url);
	if (json)
	{
		weatherForecast->seaTemp = json->GetValueAsInt32(CSTR("seaTemp.value"));
		Text::String *sUpdateTime = json->GetValueString(CSTR("updateTime"));
		Text::String *sSeaTempTime = json->GetValueString(CSTR("seaTemp.recordTime"));
		weatherForecast->generalSituation = json->GetValueNewString(CSTR("generalSituation"));
		weatherForecast->seaTempPlace = json->GetValueNewString(CSTR("seaTemp.place"));
		Text::JSONBase *weatherForecastBase = json->GetValue(CSTR("weatherForecast"));
		if (sUpdateTime == 0 || sSeaTempTime == 0 || weatherForecast->generalSituation == 0 || weatherForecast->seaTempPlace == 0 || weatherForecastBase == 0)
		{
			SDEL_STRING(weatherForecast->generalSituation);
			SDEL_STRING(weatherForecast->seaTempPlace);
			json->EndUse();
#if defined(VERBOSE)
			printf("Missing data from JSON\r\n");
#endif
			return false;
		}
		weatherForecast->updateTime = Data::Timestamp::FromStr(sUpdateTime->ToCString(), Data::DateTimeUtil::GetLocalTzQhr());
		weatherForecast->seaTempTime = Data::Timestamp::FromStr(sSeaTempTime->ToCString(), Data::DateTimeUtil::GetLocalTzQhr());
		Text::JSONArray *weatherForecastArr = (Text::JSONArray*)weatherForecastBase;
		UOSInt i = 0;
		UOSInt j = weatherForecastArr->GetArrayLength();
		while (i < j)
		{
			Text::JSONBase *weatherForecastItem = weatherForecastArr->GetArrayValue(i);
			Text::String *sDate = weatherForecastItem->GetValueString(CSTR("forecastDate"));
			Text::String *sWeekday = weatherForecastItem->GetValueString(CSTR("week"));
			Text::String *sWind = weatherForecastItem->GetValueString(CSTR("forecastWind"));
			Text::String *sWeather = weatherForecastItem->GetValueString(CSTR("forecastWeather"));
			Text::String *sPSR = weatherForecastItem->GetValueString(CSTR("PSR"));
			if (sDate != 0 && sWeekday != 0 && sWind != 0 && sWeather != 0 && sPSR != 0 && sDate->leng == 8)
			{
				DayForecast *forecast = MemAlloc(DayForecast, 1);
				UInt32 dateVal = sDate->ToUInt32();
				forecast->date.year = (UInt16)(dateVal / 10000);
				forecast->date.month = (UInt8)((dateVal / 100) % 100);
				forecast->date.day = (UInt8)(dateVal % 100);
				forecast->weekday = Data::DateTimeUtil::WeekdayParse(sWeekday->ToCString());
				forecast->wind = sWind->Clone();
				forecast->weather = sWeather->Clone();
				forecast->maxTemp = weatherForecastItem->GetValueAsInt32(CSTR("forecastMaxtemp.value"));
				forecast->minTemp = weatherForecastItem->GetValueAsInt32(CSTR("forecastMintemp.value"));
				forecast->maxRH = weatherForecastItem->GetValueAsInt32(CSTR("forecastMaxrh.value"));
				forecast->minRH = weatherForecastItem->GetValueAsInt32(CSTR("forecastMinrh.value"));
				forecast->weatherIcon = (ForecastIcon)weatherForecastItem->GetValueAsInt32(CSTR("ForecastIcon"));
				forecast->psr = PSRParse(sPSR->ToCString());
				weatherForecast->forecast.Add(forecast);
			}
			i++;
		}
		json->EndUse();
		return true;
	}
	else
	{
#if defined(VERBOSE)
		printf("Error in getting json data\r\n");
#endif		
	}
	return false;
}

void Net::HKOWeather::FreeWeatherForecast(WeatherForecast *weatherForecast)
{
	SDEL_STRING(weatherForecast->generalSituation);
	SDEL_STRING(weatherForecast->seaTempPlace);
	DayForecast *forecast;
	UOSInt i = weatherForecast->forecast.GetCount();
	while (i-- > 0)
	{
		forecast = weatherForecast->forecast.GetItem(i);
		forecast->wind->Release();
		forecast->weather->Release();
		MemFree(forecast);
	}
}

Bool Net::HKOWeather::GetLocalForecast(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Language lang, LocalForecast *localForecast)
{
	Text::CString url;
	switch (lang)
	{
	case Language::TC:
		url = CSTR("https://data.weather.gov.hk/weatherAPI/opendata/weather.php?dataType=flw&lang=tc");
		break;
	case Language::SC:
		url = CSTR("https://data.weather.gov.hk/weatherAPI/opendata/weather.php?dataType=flw&lang=sc");
		break;
	case Language::En:
	default:
		url = CSTR("https://data.weather.gov.hk/weatherAPI/opendata/weather.php?dataType=flw&lang=en");
		break;
	}
	Text::JSONBase *json = Net::HTTPJSONReader::Read(sockf, ssl, url);
	if (json)
	{
		localForecast->generalSituation = json->GetValueString(CSTR("generalSituation"));
		localForecast->tcInfo = json->GetValueString(CSTR("tcInfo"));
		localForecast->fireDangerWarning = json->GetValueString(CSTR("fireDangerWarning"));
		localForecast->forecastPeriod = json->GetValueString(CSTR("forecastPeriod"));
		localForecast->forecastDesc = json->GetValueString(CSTR("forecastDesc"));
		localForecast->outlook = json->GetValueString(CSTR("outlook"));
		Text::String *sUpdateTime = json->GetValueString(CSTR("updateTime"));
		if (localForecast->generalSituation == 0 ||
			localForecast->tcInfo == 0 ||
			localForecast->fireDangerWarning == 0 ||
			localForecast->forecastPeriod == 0 ||
			localForecast->forecastDesc == 0 ||
			localForecast->outlook == 0 ||
			sUpdateTime == 0)
		{
			json->EndUse();
			return false;
		}
		localForecast->generalSituation = localForecast->generalSituation->Clone().Ptr();
		localForecast->tcInfo = localForecast->tcInfo->Clone().Ptr();
		localForecast->fireDangerWarning = localForecast->fireDangerWarning->Clone().Ptr();
		localForecast->forecastPeriod = localForecast->forecastPeriod->Clone().Ptr();
		localForecast->forecastDesc = localForecast->forecastDesc->Clone().Ptr();
		localForecast->outlook = localForecast->outlook->Clone().Ptr();
		localForecast->updateTime = Data::Timestamp::FromStr(sUpdateTime->ToCString(), Data::DateTimeUtil::GetLocalTzQhr());
		json->EndUse();
		return true;
	}
	return false;
}

void Net::HKOWeather::FreeLocalForecast(LocalForecast *localForecast)
{
	SDEL_STRING(localForecast->generalSituation);
	SDEL_STRING(localForecast->tcInfo);
	SDEL_STRING(localForecast->fireDangerWarning);
	SDEL_STRING(localForecast->forecastPeriod);
	SDEL_STRING(localForecast->forecastDesc);
	SDEL_STRING(localForecast->outlook);
}

Bool Net::HKOWeather::GetWarningSummary(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Data::ArrayList<WarningSummary*> *warnings)
{
	Text::JSONBase *json = Net::HTTPJSONReader::Read(sockf, ssl, CSTR("https://data.weather.gov.hk/weatherAPI/opendata/weather.php?dataType=warnsum&lang=en"));
	if (json)
	{
		if (json->GetType() == Text::JSONType::Object)
		{
			Text::JSONObject *obj = (Text::JSONObject*)json;
			Data::ArrayList<Text::String*> objNames;
			obj->GetObjectNames(&objNames);
			Text::JSONObject *warnObj;
			UOSInt i = 0;
			UOSInt j = objNames.GetCount();
			while (i < j)
			{
				warnObj = obj->GetValueObject(objNames.GetItem(i)->ToCString());
				if (warnObj)
				{
					Text::String *sCode = warnObj->GetValueString(CSTR("code"));
					Text::String *sActionCode = warnObj->GetValueString(CSTR("actionCode"));
					Text::String *sIssueTime = warnObj->GetValueString(CSTR("issueTime"));
					Text::String *sUpdateTime = warnObj->GetValueString(CSTR("updateTime"));
					Text::String *sExpireTime = warnObj->GetValueString(CSTR("expireTime"));
					if (sCode != 0 && sActionCode != 0 && sIssueTime != 0 && sUpdateTime != 0)
					{
						WarningSummary *summary = MemAlloc(WarningSummary, 1);
						summary->code = WeatherWarningParse(sCode->ToCString());
						summary->actionCode = SignalActionParse(sActionCode->ToCString());
						summary->issueTime = Data::Timestamp::FromStr(sIssueTime->ToCString(), Data::DateTimeUtil::GetLocalTzQhr());
						summary->updateTime = Data::Timestamp::FromStr(sUpdateTime->ToCString(), Data::DateTimeUtil::GetLocalTzQhr());
						if (sExpireTime)
						{
							summary->expireTime = Data::Timestamp::FromStr(sExpireTime->ToCString(), Data::DateTimeUtil::GetLocalTzQhr());
						}
						else
						{
							summary->expireTime = 0;
						}
						warnings->Add(summary);
					}
				}
				i++;
			}
			json->EndUse();
			return true;
		}
		else
		{
			json->EndUse();
			return false;
		}
	}
	return false;

}

void Net::HKOWeather::FreeWarningSummary(Data::ArrayList<WarningSummary*> *warnings)
{
	LIST_FREE_FUNC(warnings, MemFree);
}

Net::HKOWeather::HKOWeather(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::EncodingFactory *encFact, UpdateHandler hdlr)
{
	this->sockf = sockf;
	this->ssl = ssl;
	this->encFact = encFact;
	this->hdlr = hdlr;
	NEW_CLASS(this->rss, Net::RSSReader(CSTR("http://rss.weather.gov.hk/rss/WeatherWarningSummary.xml"), this->sockf, this->ssl, 10, this, 30000));
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

Net::HKOWeather::PSR Net::HKOWeather::PSRParse(Text::CString psr)
{
	if (psr.Equals(UTF8STRC("Low")))
	{
		return PSR::Low;
	}
	if (psr.Equals(UTF8STRC("Medium Low")))
	{
		return PSR::MediumLow;
	}
	if (psr.Equals(UTF8STRC("Medium")))
	{
		return PSR::Medium;
	}
	if (psr.Equals(UTF8STRC("Medium High")))
	{
		return PSR::MediumHigh;
	}
	if (psr.Equals(UTF8STRC("High")))
	{
		return PSR::High;
	}
	static UInt8 chiLow[] = {0xE4, 0xBD, 0x8E};
	if (psr.Equals(chiLow, 3))
	{
		return PSR::Low;
	}
	static UInt8 chiMidLow[] = {0xE4, 0xB8, 0xAD, 0xE4, 0xBD, 0x8E};
	if (psr.Equals(chiMidLow, 6))
	{
		return PSR::MediumLow;
	}
	static UInt8 chiMid[] = {0xE4, 0xB8, 0xAD};
	if (psr.Equals(chiMid, 3))
	{
		return PSR::Medium;
	}
	static UInt8 chiMidHigh[] = {0xE4, 0xB8, 0xAD, 0xE9, 0xAB, 0x98};
	if (psr.Equals(chiMidHigh, 6))
	{
		return PSR::MediumHigh;
	}
	static UInt8 chiHigh[] = {0xE9, 0xAB, 0x98};
	if (psr.Equals(chiHigh, 3))
	{
		return PSR::Low;
	}
	return PSR::Low;
}

Text::CString Net::HKOWeather::ForecastIconGetName(ForecastIcon icon)
{
	switch (icon)
	{
	case ForecastIcon::Sunny:
		return CSTR("Sunny");
	case ForecastIcon::SunnyPeriods:
		return CSTR("Sunny Periods");
	case ForecastIcon::SunnyIntervals:
		return CSTR("Sunny Intervals");
	case ForecastIcon::SunnyPeriodsFewShowers:
		return CSTR("Sunny Periods with A Few Showers");
	case ForecastIcon::SunnyIntervalsShowers:
		return CSTR("Sunny Intervals with Showers");
	case ForecastIcon::Cloudy:
		return CSTR("Cloudy");
	case ForecastIcon::Overcast:
		return CSTR("Overcast");
	case ForecastIcon::LightRain:
		return CSTR("Light Rain");
	case ForecastIcon::Rain:
		return CSTR("Rain");
	case ForecastIcon::HeavyRain:
		return CSTR("Heavy Rain");
	case ForecastIcon::Thunderstorms:
		return CSTR("Thunderstorms");
	case ForecastIcon::Night_1:
		return CSTR("Fine ( use only in night-time on 1st of the Lunar Month )");
	case ForecastIcon::Night_2:
		return CSTR("Fine ( use only in night-time on 2nd to 6th of the Lunar Month )");
	case ForecastIcon::Night_3:
		return CSTR("Fine ( use only in night-time during 7th to 13th of Lunar Month )");
	case ForecastIcon::Night_4:
		return CSTR("Fine ( use only in night-time during 14th to 17th of Lunar Month )");
	case ForecastIcon::Night_5:
		return CSTR("Fine ( use only in night-time during 18th to 24th of Lunar Month )");
	case ForecastIcon::Night_6:
		return CSTR("Fine ( use only in night-time during 25th to 30th of Lunar Month )");
	case ForecastIcon::NightCloudy:
		return CSTR("Mainly Cloudy ( use only in night-time )");
	case ForecastIcon::NightMainlyFine:
		return CSTR("Mainly Fine ( use only in night-time )");
	case ForecastIcon::Windy:
		return CSTR("Windy");
	case ForecastIcon::Dry:
		return CSTR("Dry");
	case ForecastIcon::Humid:
		return CSTR("Humid");
	case ForecastIcon::Fog:
		return CSTR("Fog");
	case ForecastIcon::Mist:
		return CSTR("Mist");
	case ForecastIcon::Haze:
		return CSTR("Haze");
	case ForecastIcon::Hot:
		return CSTR("Hot");
	case ForecastIcon::Warm:
		return CSTR("Warm");
	case ForecastIcon::Cool:
		return CSTR("Cool");
	case ForecastIcon::Cold:
		return CSTR("Cold");
	default:
		return CSTR("Unknown");
	}
}

Text::CString Net::HKOWeather::PSRGetName(PSR psr)
{
	switch (psr)
	{
	case PSR::High:
		return CSTR("High");
	case PSR::MediumHigh:
		return CSTR("Medium High");
	case PSR::Medium:
		return CSTR("Medium");
	case PSR::MediumLow:
		return CSTR("Medium Low");
	case PSR::Low:
		return CSTR("Low");
	default:
		return CSTR("Unknown");
	}
}

Net::HKOWeather::WeatherWarning Net::HKOWeather::WeatherWarningParse(Text::CString warning)
{
	if (warning.Equals(UTF8STRC("WFIREY"))) return WeatherWarning::WFIREY;
	if (warning.Equals(UTF8STRC("WFIRER"))) return WeatherWarning::WFIRER;
	if (warning.Equals(UTF8STRC("WFROST"))) return WeatherWarning::WFROST;
	if (warning.Equals(UTF8STRC("WHOT"))) return WeatherWarning::WHOT;
	if (warning.Equals(UTF8STRC("WCOLD"))) return WeatherWarning::WCOLD;
	if (warning.Equals(UTF8STRC("WMSGNL"))) return WeatherWarning::WMSGNL;
	if (warning.Equals(UTF8STRC("WRAINA"))) return WeatherWarning::WRAINA;
	if (warning.Equals(UTF8STRC("WRAINR"))) return WeatherWarning::WRAINR;
	if (warning.Equals(UTF8STRC("WRAINB"))) return WeatherWarning::WRAINB;
	if (warning.Equals(UTF8STRC("WFNTSA"))) return WeatherWarning::WFNTSA;
	if (warning.Equals(UTF8STRC("WL"))) return WeatherWarning::WL;
	if (warning.Equals(UTF8STRC("TC1"))) return WeatherWarning::TC1;
	if (warning.Equals(UTF8STRC("TC3"))) return WeatherWarning::TC3;
	if (warning.Equals(UTF8STRC("TC8NE"))) return WeatherWarning::TC8NE;
	if (warning.Equals(UTF8STRC("TC8SE"))) return WeatherWarning::TC8SE;
	if (warning.Equals(UTF8STRC("TC8NW"))) return WeatherWarning::TC8NW;
	if (warning.Equals(UTF8STRC("TC8SW"))) return WeatherWarning::TC8SW;
	if (warning.Equals(UTF8STRC("TC9"))) return WeatherWarning::TC9;
	if (warning.Equals(UTF8STRC("TC10"))) return WeatherWarning::TC10;
	if (warning.Equals(UTF8STRC("WTMW"))) return WeatherWarning::WTMW;
	if (warning.Equals(UTF8STRC("WTS"))) return WeatherWarning::WTS;
	return WeatherWarning::None;
}

Text::CString Net::HKOWeather::WeatherWarningGetCode(WeatherWarning warning)
{
	switch (warning)
	{
	case WeatherWarning::None:
		return CSTR("None");
	case WeatherWarning::WFIREY:
		return CSTR("WFIREY");
	case WeatherWarning::WFIRER:
		return CSTR("WFIRER");
	case WeatherWarning::WFROST:
		return CSTR("WFROST");
	case WeatherWarning::WHOT:
		return CSTR("WHOT");
	case WeatherWarning::WCOLD:
		return CSTR("WCOLD");
	case WeatherWarning::WMSGNL:
		return CSTR("WMSGNL");
	case WeatherWarning::WRAINA:
		return CSTR("WRAINA");
	case WeatherWarning::WRAINR:
		return CSTR("WRAINR");
	case WeatherWarning::WRAINB:
		return CSTR("WRAINB");
	case WeatherWarning::WFNTSA:
		return CSTR("WFNTSA");
	case WeatherWarning::WL:
		return CSTR("WL");
	case WeatherWarning::TC1:
		return CSTR("TC1");
	case WeatherWarning::TC3:
		return CSTR("TC3");
	case WeatherWarning::TC8NE:
		return CSTR("TC8NE");
	case WeatherWarning::TC8SE:
		return CSTR("TC8SE");
	case WeatherWarning::TC8NW:
		return CSTR("TC8NW");
	case WeatherWarning::TC8SW:
		return CSTR("TC8SW");
	case WeatherWarning::TC9:
		return CSTR("TC9");
	case WeatherWarning::TC10:
		return CSTR("TC10");
	case WeatherWarning::WTMW:
		return CSTR("WTMW");
	case WeatherWarning::WTS:
		return CSTR("WTS");
	default:
		return CSTR("Unknown");
	}
}

Text::CString Net::HKOWeather::WeatherWarningGetName(WeatherWarning warning)
{
	switch (warning)
	{
	case WeatherWarning::None:
		return CSTR("None");
	case WeatherWarning::WFIREY:
		return CSTR("Yellow Fire Danger Warning");
	case WeatherWarning::WFIRER:
		return CSTR("Red Fire Danger Warning");
	case WeatherWarning::WFROST:
		return CSTR("Frost Warning");
	case WeatherWarning::WHOT:
		return CSTR("Very Hot Weather Warning");
	case WeatherWarning::WCOLD:
		return CSTR("Cold Weather Warning");
	case WeatherWarning::WMSGNL:
		return CSTR("Strong Monsoon Signal");
	case WeatherWarning::WRAINA:
		return CSTR("Amber Rainstorm Warning Signal");
	case WeatherWarning::WRAINR:
		return CSTR("Red Rainstorm Warning Signal");
	case WeatherWarning::WRAINB:
		return CSTR("Black Rainstorm Warning Signal");
	case WeatherWarning::WFNTSA:
		return CSTR("Special Announcement on Flooding in the Northern New Territories");
	case WeatherWarning::WL:
		return CSTR("Landslip Warning");
	case WeatherWarning::TC1:
		return CSTR("No. 1 Tropical Cyclone Warning Signal");
	case WeatherWarning::TC3:
		return CSTR("No. 3 Tropical Cyclone Warning Signal");
	case WeatherWarning::TC8NE:
		return CSTR("No. 8 Northeast Gale or Storm Signal Tropical Cyclone Warning Signal");
	case WeatherWarning::TC8SE:
		return CSTR("No. 8 Southeast Gale or Storm Signal Tropical Cyclone Warning Signal");
	case WeatherWarning::TC8NW:
		return CSTR("No. 8 Northwest Gale or Storm Signal Tropical Cyclone Warning Signal");
	case WeatherWarning::TC8SW:
		return CSTR("No. 8 Southwest Gale or Storm Signal Tropical Cyclone Warning Signal");
	case WeatherWarning::TC9:
		return CSTR("No. 9 Tropical Cyclone Warning Signal");
	case WeatherWarning::TC10:
		return CSTR("No. 10 Tropical Cyclone Warning Signal");
	case WeatherWarning::WTMW:
		return CSTR("Tsunami Warning");
	case WeatherWarning::WTS:
		return CSTR("Thunderstorm Warning");
	default:
		return CSTR("Unknown");
	}
}

Net::HKOWeather::SignalAction Net::HKOWeather::SignalActionParse(Text::CString action)
{
	if (action.Equals(UTF8STRC("ISSUE"))) return SignalAction::ISSUE;
	if (action.Equals(UTF8STRC("REISSUE"))) return SignalAction::REISSUE;
	if (action.Equals(UTF8STRC("CANCEL"))) return SignalAction::CANCEL;
	if (action.Equals(UTF8STRC("EXTEND"))) return SignalAction::EXTEND;
	if (action.Equals(UTF8STRC("UPDATE"))) return SignalAction::UPDATE;
	return SignalAction::ISSUE;
}

Text::CString Net::HKOWeather::SignalActionGetName(SignalAction action)
{
	switch (action)
	{
	case SignalAction::ISSUE:
		return CSTR("Issue");
	case SignalAction::REISSUE:
		return CSTR("Reissue");
	case SignalAction::CANCEL:
		return CSTR("Cancel");
	case SignalAction::EXTEND:
		return CSTR("Extend");
	case SignalAction::UPDATE:
		return CSTR("Update");
	default:
		return CSTR("Unknown");
	}
}
