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

Net::HKOWeather::WeatherSignal Net::HKOWeather::GetSignalSummary(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, NN<Text::EncodingFactory> encFact)
{
	UInt8 buff[1024];
	UnsafeArray<UInt8> mbuff;
	NN<Net::HTTPClient> cli;
	Net::HKOWeather::WeatherSignal signal;
	UOSInt i;

	cli = Net::HTTPClient::CreateConnect(sockf, ssl, CSTR("http://rss.weather.gov.hk/rss/WeatherWarningSummary.xml"), Net::WebUtil::RequestMethod::HTTP_GET, false);
	if (cli->IsError())
	{
		cli.Delete();
		return Net::HKOWeather::WS_UNKNOWN;
	}
	IO::MemoryStream mstm;
	while ((i = cli->Read(BYTEARR(buff))) > 0)
	{
		mstm.Write(Data::ByteArrayR(buff, i));
	}
	cli.Delete();

	mbuff = mstm.GetBuff(i);
	Text::XMLDocument doc;
	doc.ParseBuff(encFact, mbuff, i);
	UnsafeArray<NN<Text::XMLNode>> nodes = doc.SearchNode(CSTR("/rss/channel/item/description"), i);
	NN<Text::XMLNode> n;
	NN<Text::XMLNode> n2;

	signal = Net::HKOWeather::WS_UNKNOWN;
	if (i > 0)
	{
		n = nodes[0];
		i = n->GetChildCnt();
		while (i-- > 0)
		{
			n2 = n->GetChildNoCheck(i);
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

Bool Net::HKOWeather::GetCurrentTempRH(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, OutParam<Int32> temperature, OutParam<Int32> rh, NN<IO::LogTool> log)
{
	Bool succ = false;
	Net::RSS *rss;
	Text::CStringNN userAgent = Net::UserAgentDB::FindUserAgent(Manage::OSInfo::OT_WINDOWS_NT64, Net::BrowserInfo::BT_FIREFOX);
	NN<Text::String> ua = Text::String::New(userAgent);
	NEW_CLASS(rss, Net::RSS(CSTR("https://rss.weather.gov.hk/rss/CurrentWeather.xml"), ua.Ptr(), sockf, ssl, 30000, log));
	ua->Release();
	if (!rss->IsError())
	{
		if (rss->GetCount() > 0)
		{
			temperature.Set(INVALID_READING);
			rh.Set(INVALID_READING);
			UOSInt i;
			NN<Net::RSSItem> item = rss->GetItemNoCheck(0);
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

Bool Net::HKOWeather::GetWeatherForecast(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Language lang, NN<WeatherForecast> weatherForecast)
{
	Text::CStringNN url;
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
	NN<Text::JSONBase> json;
	if (Net::HTTPJSONReader::Read(sockf, ssl, url).SetTo(json))
	{
		weatherForecast->seaTemp = json->GetValueAsInt32(CSTR("seaTemp.value"));
		NN<Text::String> sUpdateTime;
		NN<Text::String> sSeaTempTime;
		weatherForecast->generalSituation = json->GetValueNewString(CSTR("generalSituation"));
		weatherForecast->seaTempPlace = json->GetValueNewString(CSTR("seaTemp.place"));
		NN<Text::JSONBase> weatherForecastBase;
		if (!json->GetValueString(CSTR("updateTime")).SetTo(sUpdateTime) || !json->GetValueString(CSTR("seaTemp.recordTime")).SetTo(sSeaTempTime) || weatherForecast->generalSituation.IsNull() || weatherForecast->seaTempPlace.IsNull() || !json->GetValue(CSTR("weatherForecast")).SetTo(weatherForecastBase))
		{
			OPTSTR_DEL(weatherForecast->generalSituation);
			OPTSTR_DEL(weatherForecast->seaTempPlace);
			json->EndUse();
#if defined(VERBOSE)
			printf("Missing data from JSON\r\n");
#endif
			return false;
		}
		weatherForecast->updateTime = Data::Timestamp::FromStr(sUpdateTime->ToCString(), Data::DateTimeUtil::GetLocalTzQhr());
		weatherForecast->seaTempTime = Data::Timestamp::FromStr(sSeaTempTime->ToCString(), Data::DateTimeUtil::GetLocalTzQhr());
		NN<Text::JSONArray> weatherForecastArr = NN<Text::JSONArray>::ConvertFrom(weatherForecastBase);
		UOSInt i = 0;
		UOSInt j = weatherForecastArr->GetArrayLength();
		while (i < j)
		{
			NN<Text::JSONBase> weatherForecastItem;
			NN<Text::String> sDate;
			NN<Text::String> sWeekday;
			NN<Text::String> sWind;
			NN<Text::String> sWeather;
			NN<Text::String> sPSR;
			if (weatherForecastArr->GetArrayValue(i).SetTo(weatherForecastItem) &&
				weatherForecastItem->GetValueString(CSTR("forecastDate")).SetTo(sDate) &&
				weatherForecastItem->GetValueString(CSTR("week")).SetTo(sWeekday) &&
				weatherForecastItem->GetValueString(CSTR("forecastWind")).SetTo(sWind) &&
				weatherForecastItem->GetValueString(CSTR("forecastWeather")).SetTo(sWeather) &&
				weatherForecastItem->GetValueString(CSTR("PSR")).SetTo(sPSR) && sDate->leng == 8)
			{
				NN<DayForecast> forecast = MemAllocNN(DayForecast);
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

void Net::HKOWeather::FreeWeatherForecast(NN<WeatherForecast> weatherForecast)
{
	OPTSTR_DEL(weatherForecast->generalSituation);
	OPTSTR_DEL(weatherForecast->seaTempPlace);
	NN<DayForecast> forecast;
	UOSInt i = weatherForecast->forecast.GetCount();
	while (i-- > 0)
	{
		forecast = weatherForecast->forecast.GetItemNoCheck(i);
		forecast->wind->Release();
		forecast->weather->Release();
		MemFreeNN(forecast);
	}
}

Bool Net::HKOWeather::GetLocalForecast(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Language lang, NN<LocalForecast> localForecast)
{
	Text::CStringNN url;
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
	NN<Text::JSONBase> json;
	if (Net::HTTPJSONReader::Read(sockf, ssl, url).SetTo(json))
	{
		localForecast->generalSituation = json->GetValueString(CSTR("generalSituation"));
		localForecast->tcInfo = json->GetValueString(CSTR("tcInfo"));
		localForecast->fireDangerWarning = json->GetValueString(CSTR("fireDangerWarning"));
		localForecast->forecastPeriod = json->GetValueString(CSTR("forecastPeriod"));
		localForecast->forecastDesc = json->GetValueString(CSTR("forecastDesc"));
		localForecast->outlook = json->GetValueString(CSTR("outlook"));
		NN<Text::String> sUpdateTime;
		if (localForecast->generalSituation.IsNull() ||
			localForecast->tcInfo.IsNull() ||
			localForecast->fireDangerWarning.IsNull() ||
			localForecast->forecastPeriod.IsNull() ||
			localForecast->forecastDesc.IsNull() ||
			localForecast->outlook.IsNull() ||
			!json->GetValueString(CSTR("updateTime")).SetTo(sUpdateTime))
		{
			json->EndUse();
			return false;
		}
		localForecast->generalSituation = Text::String::CopyOrNull(localForecast->generalSituation);
		localForecast->tcInfo = Text::String::CopyOrNull(localForecast->tcInfo);
		localForecast->fireDangerWarning = Text::String::CopyOrNull(localForecast->fireDangerWarning);
		localForecast->forecastPeriod = Text::String::CopyOrNull(localForecast->forecastPeriod);
		localForecast->forecastDesc = Text::String::CopyOrNull(localForecast->forecastDesc);
		localForecast->outlook = Text::String::CopyOrNull(localForecast->outlook);
		localForecast->updateTime = Data::Timestamp::FromStr(sUpdateTime->ToCString(), Data::DateTimeUtil::GetLocalTzQhr());
		json->EndUse();
		return true;
	}
	return false;
}

void Net::HKOWeather::FreeLocalForecast(NN<LocalForecast> localForecast)
{
	OPTSTR_DEL(localForecast->generalSituation);
	OPTSTR_DEL(localForecast->tcInfo);
	OPTSTR_DEL(localForecast->fireDangerWarning);
	OPTSTR_DEL(localForecast->forecastPeriod);
	OPTSTR_DEL(localForecast->forecastDesc);
	OPTSTR_DEL(localForecast->outlook);
}

Bool Net::HKOWeather::GetWarningSummary(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, NN<Data::ArrayListNN<WarningSummary>> warnings)
{
	NN<Text::JSONBase> json;
	if (Net::HTTPJSONReader::Read(sockf, ssl, CSTR("https://data.weather.gov.hk/weatherAPI/opendata/weather.php?dataType=warnsum&lang=en")).SetTo(json))
	{
		if (json->GetType() == Text::JSONType::Object)
		{
			NN<Text::JSONObject> obj = NN<Text::JSONObject>::ConvertFrom(json);
			Data::ArrayListNN<Text::String> objNames;
			obj->GetObjectNames(objNames);
			NN<Text::JSONObject> warnObj;
			Data::ArrayIterator<NN<Text::String>> it = objNames.Iterator();
			while (it.HasNext())
			{
				if (obj->GetValueObject(it.Next()->ToCString()).SetTo(warnObj))
				{
					NN<Text::String> sCode;
					NN<Text::String> sActionCode;
					NN<Text::String> sIssueTime;
					NN<Text::String> sUpdateTime;
					Optional<Text::String> sExpireTime = warnObj->GetValueString(CSTR("expireTime"));
					if (warnObj->GetValueString(CSTR("code")).SetTo(sCode) &&
						warnObj->GetValueString(CSTR("actionCode")).SetTo(sActionCode) &&
						warnObj->GetValueString(CSTR("issueTime")).SetTo(sIssueTime) &&
						warnObj->GetValueString(CSTR("updateTime")).SetTo(sUpdateTime))
					{
						NN<WarningSummary> summary = MemAllocNN(WarningSummary);
						summary->code = WeatherWarningParse(sCode->ToCString());
						summary->actionCode = SignalActionParse(sActionCode->ToCString());
						summary->issueTime = Data::Timestamp::FromStr(sIssueTime->ToCString(), Data::DateTimeUtil::GetLocalTzQhr());
						summary->updateTime = Data::Timestamp::FromStr(sUpdateTime->ToCString(), Data::DateTimeUtil::GetLocalTzQhr());
						NN<Text::String> s;
						if (sExpireTime.SetTo(s))
						{
							summary->expireTime = Data::Timestamp::FromStr(s->ToCString(), Data::DateTimeUtil::GetLocalTzQhr());
						}
						else
						{
							summary->expireTime = 0;
						}
						warnings->Add(summary);
					}
				}
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

void Net::HKOWeather::FreeWarningSummary(NN<Data::ArrayListNN<WarningSummary>> warnings)
{
	warnings->MemFreeAll();
}

Net::HKOWeather::HKOWeather(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, NN<Text::EncodingFactory> encFact, UpdateHandler hdlr, NN<IO::LogTool> log)
{
	this->sockf = sockf;
	this->ssl = ssl;
	this->encFact = encFact;
	this->hdlr = hdlr;
	NEW_CLASS(this->rss, Net::RSSReader(CSTR("http://rss.weather.gov.hk/rss/WeatherWarningSummary.xml"), this->sockf, this->ssl, 10, this, 30000, log));
}

Net::HKOWeather::~HKOWeather()
{
	DEL_CLASS(this->rss);
}

void Net::HKOWeather::ItemAdded(NN<Net::RSSItem> item)
{
	WeatherSignal signal = String2Signal(item->description);
	if (signal != this->currSignal)
	{
		this->currSignal = signal;
		this->hdlr(this->currSignal);
	}
}

void Net::HKOWeather::ItemRemoved(NN<Net::RSSItem> item)
{
}

Net::HKOWeather::WeatherSignal Net::HKOWeather::GetCurrentSignal()
{
	return this->currSignal;
}

Net::HKOWeather::PSR Net::HKOWeather::PSRParse(Text::CStringNN psr)
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

Text::CStringNN Net::HKOWeather::ForecastIconGetName(ForecastIcon icon)
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

Text::CStringNN Net::HKOWeather::PSRGetName(PSR psr)
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

Net::HKOWeather::WeatherWarning Net::HKOWeather::WeatherWarningParse(Text::CStringNN warning)
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

Text::CStringNN Net::HKOWeather::WeatherWarningGetCode(WeatherWarning warning)
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

Text::CStringNN Net::HKOWeather::WeatherWarningGetName(WeatherWarning warning)
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

Net::HKOWeather::SignalAction Net::HKOWeather::SignalActionParse(Text::CStringNN action)
{
	if (action.Equals(UTF8STRC("ISSUE"))) return SignalAction::ISSUE;
	if (action.Equals(UTF8STRC("REISSUE"))) return SignalAction::REISSUE;
	if (action.Equals(UTF8STRC("CANCEL"))) return SignalAction::CANCEL;
	if (action.Equals(UTF8STRC("EXTEND"))) return SignalAction::EXTEND;
	if (action.Equals(UTF8STRC("UPDATE"))) return SignalAction::UPDATE;
	return SignalAction::ISSUE;
}

Text::CStringNN Net::HKOWeather::SignalActionGetName(SignalAction action)
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
