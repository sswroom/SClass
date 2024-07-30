#include "Stdafx.h"
#include "Net/HTTPClient.h"
#include "Net/SolarEdgeAPI.h"
#include "Text/StringBuilderUTF8.h"

//#define VERBOSE
#if defined(VERBOSE)
#include <stdio.h>
#endif

#define API_LINK CSTR("https://monitoringapi.solaredge.com")

void Net::SolarEdgeAPI::BuildURL(NN<Text::StringBuilderUTF8> sb, Text::CStringNN path)
{
	sb->Append(API_LINK);
	sb->Append(path);
	sb->AppendC(UTF8STRC("?api_key="));
	sb->Append(this->apikey);
}

Optional<Text::JSONBase> Net::SolarEdgeAPI::GetJSON(Text::CStringNN url)
{
#if defined(VERBOSE)
	printf("SolarEdgeAPI requesting: %s\r\n", url.v);
#endif
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->clif, this->ssl, url, Net::WebUtil::RequestMethod::HTTP_GET, true);
	Text::StringBuilderUTF8 sb;
	if (!cli->ReadAllContent(sb, 8192, 1048576))
	{
		cli.Delete();
		return 0;
	}
	cli.Delete();
#if defined(VERBOSE)
	printf("SolarEdgeAPI received: %s\r\n", sb.ToString());
#endif
	return Text::JSONBase::ParseJSONStr(sb.ToCString());
}

Net::SolarEdgeAPI::SolarEdgeAPI(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Text::CStringNN apikey)
{
	this->clif = clif;
	this->ssl = ssl;
	this->apikey = Text::String::New(apikey);
}

Net::SolarEdgeAPI::~SolarEdgeAPI()
{
	this->apikey->Release();
}

Optional<Text::String> Net::SolarEdgeAPI::GetCurrentVersion()
{
	Text::StringBuilderUTF8 sbURL;
	this->BuildURL(sbURL, CSTR("/version/current"));
	NN<Text::JSONBase> json;
	if (!this->GetJSON(sbURL.ToCString()).SetTo(json))
		return 0;
	Optional<Text::String> s = json->GetValueString(CSTR("version.release"));
	if (s.IsNull())
	{
		s = json->GetValueString(CSTR("version"));
	}
	s = Text::String::CopyOrNull(s);
	json->EndUse();
	return s;
}

Bool Net::SolarEdgeAPI::GetSupportedVersions(NN<Data::ArrayListStringNN> versions)
{
	Text::StringBuilderUTF8 sbURL;
	this->BuildURL(sbURL, CSTR("/version/supported"));
	NN<Text::JSONBase> json;
	if (!this->GetJSON(sbURL.ToCString()).SetTo(json))
		return false;
	NN<Text::JSONBase> supported;
	if (json->GetValue(CSTR("supported")).SetTo(supported) && supported->GetType() == Text::JSONType::Array)
	{
		NN<Text::JSONArray> arr = NN<Text::JSONArray>::ConvertFrom(supported);
		UOSInt i = 0;
		UOSInt j = arr->GetArrayLength();
		while (i < j)
		{
			NN<Text::String> s;
			NN<Text::JSONBase> obj;
			if (arr->GetArrayValue(i).SetTo(obj))
			{
				if (obj->GetType() == Text::JSONType::String)
				{
					versions->Add(NN<Text::JSONString>::ConvertFrom(obj)->GetValue()->Clone());
				}
				else if (obj->GetValueString(CSTR("release")).SetTo(s))
				{
					versions->Add(s->Clone());
				}
			}
			i++;
		}
		json->EndUse();
		return true;
	}
	json->EndUse();
	return false;
}

Bool Net::SolarEdgeAPI::GetSiteList(NN<Data::ArrayListNN<Site>> siteList, UOSInt maxCount, UOSInt startOfst, OptOut<UOSInt> totalCount)
{
	Text::StringBuilderUTF8 sbURL;
	this->BuildURL(sbURL, CSTR("/sites/list"));
	if (maxCount != 0)
	{
		sbURL.AppendC(UTF8STRC("&size="));
		sbURL.AppendUOSInt(maxCount);
		sbURL.AppendC(UTF8STRC("&startIndex="));
		sbURL.AppendUOSInt(startOfst);
	}
	NN<Text::JSONBase> json;
	if (!this->GetJSON(sbURL.ToCString()).SetTo(json))
		return false;
	
	NN<Text::JSONBase> count;
	NN<Text::JSONBase> sites;
	if (json->GetValue(CSTR("sites.count")).SetTo(count) &&
		json->GetValue(CSTR("sites.site")).SetTo(sites) &&
		(count->GetType() == Text::JSONType::Number || count->GetType() == Text::JSONType::INT32) && sites->GetType() == Text::JSONType::Array)
	{
		if (totalCount.IsNotNull())
		{
			totalCount.SetNoCheck((UOSInt)count->GetAsInt32());
		}
		NN<Text::JSONArray> siteArr = NN<Text::JSONArray>::ConvertFrom(sites);
		UOSInt i = 0;
		UOSInt j = siteArr->GetArrayLength();
		while (i < j)
		{
			NN<Text::JSONObject> siteObj;
			NN<Site> site;
			NN<Text::String> s;
			if (Optional<Text::JSONObject>::ConvertFrom(siteArr->GetArrayValue(i)).SetTo(siteObj) && siteObj->GetType() == Text::JSONType::Object)
			{
				site = MemAllocNN(Site);
				site->id = siteObj->GetObjectInt32(CSTR("id"));
				site->name = siteObj->GetObjectNewString(CSTR("name"));
				site->accountId = siteObj->GetObjectInt32(CSTR("accountId"));
				site->status = siteObj->GetObjectNewString(CSTR("status"));
				site->peakPower_kWp = siteObj->GetObjectDouble(CSTR("peakPower"));
				if (!siteObj->GetObjectString(CSTR("lastUpdateTime")).SetTo(s))
					site->lastUpdateTime = 0;
				else
					site->lastUpdateTime = Data::Timestamp::FromStr(s->ToCString(), Data::DateTimeUtil::GetLocalTzQhr());
				site->currency = siteObj->GetObjectNewString(CSTR("currency"));
				if (!siteObj->GetObjectString(CSTR("installationDate")).SetTo(s))
					site->installationDate = 0;
				else
					site->installationDate = Data::Timestamp::FromStr(s->ToCString(), Data::DateTimeUtil::GetLocalTzQhr());
				if (!siteObj->GetObjectString(CSTR("ptoDate")).SetTo(s))
					site->ptoDate = 0;
				else
					site->ptoDate = Data::Timestamp::FromStr(s->ToCString(), Data::DateTimeUtil::GetLocalTzQhr());
				site->notes = siteObj->GetObjectNewString(CSTR("notes"));
				site->type = siteObj->GetObjectNewString(CSTR("type"));
				site->country = siteObj->GetValueNewString(CSTR("location.country"));
				site->city = siteObj->GetValueNewString(CSTR("location.city"));
				site->address = siteObj->GetValueNewString(CSTR("location.address"));
				site->address2 = siteObj->GetValueNewString(CSTR("location.address2"));
				site->zip = siteObj->GetValueNewString(CSTR("location.zip"));
				site->timeZone = siteObj->GetValueNewString(CSTR("location.timeZone"));
				site->countryCode = siteObj->GetValueNewString(CSTR("location.countryCode"));
				site->isPublic = siteObj->GetValueAsBool(CSTR("publicSettings.isPublic"));
				site->publicName = siteObj->GetValueNewString(CSTR("publicSettings.name"));
				siteList->Add(site);
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

void Net::SolarEdgeAPI::FreeSiteList(NN<Data::ArrayListNN<Site>> siteList)
{
	NN<Site> site;
	UOSInt i = siteList->GetCount();
	while (i-- > 0)
	{
		site = siteList->GetItemNoCheck(i);
		OPTSTR_DEL(site->name);
		OPTSTR_DEL(site->status);
		OPTSTR_DEL(site->currency);
		OPTSTR_DEL(site->notes);
		OPTSTR_DEL(site->type);
		OPTSTR_DEL(site->country);
		OPTSTR_DEL(site->city);
		OPTSTR_DEL(site->address);
		OPTSTR_DEL(site->address2);
		OPTSTR_DEL(site->zip);
		OPTSTR_DEL(site->timeZone);
		OPTSTR_DEL(site->countryCode);
		OPTSTR_DEL(site->publicName);
		MemFreeNN(site);
	}
	siteList->Clear();
}

Bool Net::SolarEdgeAPI::GetSiteOverview(Int32 siteId, NN<SiteOverview> overview)
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrConcatC(sbuff, UTF8STRC("/site/"));
	sptr = Text::StrInt32(sptr, siteId);
	sptr = Text::StrConcatC(sptr, UTF8STRC("/overview"));
	Text::StringBuilderUTF8 sbURL;
	this->BuildURL(sbURL, CSTRP(sbuff, sptr));
	NN<Text::JSONBase> json;
	if (!this->GetJSON(sbURL.ToCString()).SetTo(json))
		return false;
	NN<Text::String> s;
	if (json->GetValueString(CSTR("overview.lastUpdateTime")).SetTo(s))
		overview->lastUpdateTime = Data::Timestamp::FromStr(s->ToCString(), Data::DateTimeUtil::GetLocalTzQhr());
	else
		overview->lastUpdateTime = 0;
	overview->lifeTimeEnergy_Wh = json->GetValueAsDouble(CSTR("overview.lifeTimeData.energy"));
	overview->lifeTimeRevenue = json->GetValueAsDouble(CSTR("overview.lifeTimeData.revenue"));
	overview->yearlyEnergy_Wh = json->GetValueAsDouble(CSTR("overview.lastYearData.energy"));
	overview->monthlyEnergy_Wh = json->GetValueAsDouble(CSTR("overview.lastMonthData.energy"));
	overview->dailyEnergy_Wh = json->GetValueAsDouble(CSTR("overview.lastDayData.energy"));
	overview->currentPower_W = json->GetValueAsDouble(CSTR("overview.currentPower.power"));
	json->EndUse();
	return true;
}

Bool Net::SolarEdgeAPI::GetSiteEnergy(Int32 siteId, Data::Timestamp startTime, Data::Timestamp endTime, TimeUnit timeUnit, NN<Data::ArrayList<TimedValue>> values)
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrConcatC(sbuff, UTF8STRC("/site/"));
	sptr = Text::StrInt32(sptr, siteId);
	sptr = Text::StrConcatC(sptr, UTF8STRC("/energy"));
	Text::StringBuilderUTF8 sbURL;
	this->BuildURL(sbURL, CSTRP(sbuff, sptr));
	sbURL.AppendC(UTF8STRC("&timeUnit="));
	sbURL.Append(TimeUnitGetName(timeUnit));
	sbURL.AppendC(UTF8STRC("&startDate="));
	AppendFormDate(sbURL, startTime, false);
	sbURL.AppendC(UTF8STRC("&endDate="));
	AppendFormDate(sbURL, endTime, false);
	NN<Text::JSONBase> json;
	if (!this->GetJSON(sbURL.ToCString()).SetTo(json))
		return false;
	Bool succ = false;
	NN<Text::JSONBase> valueObj;
	NN<Text::JSONArray> valueArr;
	if (json->GetValue(CSTR("energy.values")).SetTo(valueObj) && valueObj->GetType() == Text::JSONType::Array)
	{
		valueArr = NN<Text::JSONArray>::ConvertFrom(valueObj);
		UOSInt i = 0;
		UOSInt j = valueArr->GetArrayLength();
		while (i < j)
		{
			if (valueArr->GetArrayValue(i).SetTo(valueObj))
			{
				NN<Text::JSONBase> dateObj;
				NN<Text::JSONBase> valObj;
				if (valueObj->GetValue(CSTR("date")).SetTo(dateObj) &&
					valueObj->GetValue(CSTR("value")).SetTo(valObj) &&
					valObj->GetType() != Text::JSONType::Null && dateObj->GetType() == Text::JSONType::String)
				{
					values->Add(TimedValue(Data::Timestamp::FromStr(NN<Text::JSONString>::ConvertFrom(dateObj)->GetValue()->ToCString(), Data::DateTimeUtil::GetLocalTzQhr()), valObj->GetAsDouble()));
				}
			}
			i++;
		}
		succ = true;	
	}
	json->EndUse();
	return succ;
}

Bool Net::SolarEdgeAPI::GetSitePower(Int32 siteId, Data::Timestamp startTime, Data::Timestamp endTime, NN<Data::ArrayList<TimedValue>> values)
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrConcatC(sbuff, UTF8STRC("/site/"));
	sptr = Text::StrInt32(sptr, siteId);
	sptr = Text::StrConcatC(sptr, UTF8STRC("/power"));
	Text::StringBuilderUTF8 sbURL;
	this->BuildURL(sbURL, CSTRP(sbuff, sptr));
	sbURL.AppendC(UTF8STRC("&startTime="));
	AppendFormDate(sbURL, startTime, true);
	sbURL.AppendC(UTF8STRC("&endTime="));
	AppendFormDate(sbURL, endTime, true);
	NN<Text::JSONBase> json;
	if (!this->GetJSON(sbURL.ToCString()).SetTo(json))
		return false;
	Bool succ = false;
	NN<Text::JSONBase> valueObj;
	NN<Text::JSONArray> valueArr;
	if (json->GetValue(CSTR("power.values")).SetTo(valueObj) && valueObj->GetType() == Text::JSONType::Array)
	{
		valueArr = NN<Text::JSONArray>::ConvertFrom(valueObj);
		UOSInt i = 0;
		UOSInt j = valueArr->GetArrayLength();
		while (i < j)
		{
			if (valueArr->GetArrayValue(i).SetTo(valueObj))
			{
				NN<Text::JSONBase> dateObj;
				NN<Text::JSONBase> valObj;
				if (valueObj->GetValue(CSTR("date")).SetTo(dateObj) &&
					valueObj->GetValue(CSTR("value")).SetTo(valObj) &&
					valObj->GetType() != Text::JSONType::Null && dateObj->GetType() == Text::JSONType::String)
				{
					values->Add(TimedValue(Data::Timestamp::FromStr(NN<Text::JSONString>::ConvertFrom(dateObj)->GetValue()->ToCString(), Data::DateTimeUtil::GetLocalTzQhr()), valObj->GetAsDouble()));
				}
			}
			i++;
		}
		succ = true;	
	}
	json->EndUse();
	return succ;
}

void Net::SolarEdgeAPI::AppendFormDate(NN<Text::StringBuilderUTF8> sb, Data::Timestamp ts, Bool hasTime)
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	sptr = ts.ToString(sbuff, "yyyy-MM-dd");
	if (hasTime)
	{
		sptr = Text::StrConcatC(sptr, UTF8STRC("%20"));
		sptr = ts.ToString(sptr, "HH:mm:ss");
	}
	sb->AppendP(sbuff, sptr);
}

Text::CStringNN Net::SolarEdgeAPI::TimeUnitGetName(TimeUnit timeUnit)
{
	switch (timeUnit)
	{
	case TimeUnit::DAY:
		return CSTR("DAY");
	case TimeUnit::QUARTER_OF_AN_HOUR:
		return CSTR("QUARTER_OF_AN_HOUR");
	case TimeUnit::HOUR:
		return CSTR("HOUR");
	case TimeUnit::WEEK:
		return CSTR("WEEK");
	case TimeUnit::MONTH:
		return CSTR("MONTH");
	case TimeUnit::YEAR:
		return CSTR("YEAR");
	default:
		return CSTR("Unknown");
	}
}
