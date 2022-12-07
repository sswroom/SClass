#include "Stdafx.h"
#include "Net/HTTPClient.h"
#include "Net/SolarEdgeAPI.h"
#include "Text/StringBuilderUTF8.h"

#define VERBOSE
#if defined(VERBOSE)
#include <stdio.h>
#endif

#define API_LINK CSTR("https://monitoringapi.solaredge.com")

void Net::SolarEdgeAPI::BuildURL(Text::StringBuilderUTF8 *sb, Text::CString path)
{
	sb->Append(API_LINK);
	sb->Append(path);
	sb->AppendC(UTF8STRC("?api_key="));
	sb->Append(this->apikey);
}

Text::JSONBase *Net::SolarEdgeAPI::GetJSON(Text::CString url)
{
	Net::HTTPClient *cli = Net::HTTPClient::CreateConnect(this->sockf, this->ssl, url, Net::WebUtil::RequestMethod::HTTP_GET, true);
	if (cli == 0)
	{
		return 0;
	}
	Text::StringBuilderUTF8 sb;
	if (!cli->ReadAllContent(&sb, 8192, 1048576))
	{
		DEL_CLASS(cli);
		return 0;
	}
	DEL_CLASS(cli);
#if defined(VERBOSE)
	printf("SolarEdgeAPI received: %s\r\n", sb.ToString());
#endif
	return Text::JSONBase::ParseJSONStr(sb.ToCString());
}

Net::SolarEdgeAPI::SolarEdgeAPI(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::CString apikey)
{
	this->sockf = sockf;
	this->ssl = ssl;
	this->apikey = Text::String::New(apikey);
}

Net::SolarEdgeAPI::~SolarEdgeAPI()
{
	this->apikey->Release();
}

Text::String *Net::SolarEdgeAPI::GetCurrentVersion()
{
	Text::StringBuilderUTF8 sbURL;
	this->BuildURL(&sbURL, CSTR("/version/current"));
	Text::JSONBase *json = this->GetJSON(sbURL.ToCString());
	if (json == 0)
		return 0;
	Text::String *s = json->GetValueString(CSTR("version.release"));
	if (s == 0)
	{
		s = json->GetValueString(CSTR("version"));
	}
	if (s)
	{
		s = s->Clone();
	}
	json->EndUse();
	return s;
}

Bool Net::SolarEdgeAPI::GetSupportedVersions(Data::ArrayList<Text::String*> *versions)
{
	Text::StringBuilderUTF8 sbURL;
	this->BuildURL(&sbURL, CSTR("/version/supported"));
	Text::JSONBase *json = this->GetJSON(sbURL.ToCString());
	if (json == 0)
		return false;
	Text::JSONBase *supported = json->GetValue(CSTR("supported"));
	if (supported && supported->GetType() == Text::JSONType::Array)
	{
		Text::JSONArray *arr = (Text::JSONArray*)supported;
		UOSInt i = 0;
		UOSInt j = arr->GetArrayLength();
		while (i < j)
		{
			Text::JSONBase *obj = arr->GetArrayValue(i);
			Text::String *s;
			if (obj->GetType() == Text::JSONType::String)
			{
				s = ((Text::JSONString*)obj)->GetValue();
			}
			else
			{
				s = obj->GetValueString(CSTR("release"));
			}
			if (s)
			{
				versions->Add(s->Clone());
			}
			i++;
		}
		json->EndUse();
		return true;
	}
	json->EndUse();
	return false;
}

Bool Net::SolarEdgeAPI::GetSiteList(Data::ArrayList<Site*> *siteList, UOSInt maxCount, UOSInt startOfst, UOSInt *totalCount)
{
	Text::StringBuilderUTF8 sbURL;
	this->BuildURL(&sbURL, CSTR("/sites/list"));
	if (maxCount != 0)
	{
		sbURL.AppendC(UTF8STRC("&size="));
		sbURL.AppendUOSInt(maxCount);
		sbURL.AppendC(UTF8STRC("&startIndex="));
		sbURL.AppendUOSInt(startOfst);
	}
	Text::JSONBase *json = this->GetJSON(sbURL.ToCString());
	if (json == 0)
		return false;
	
	Text::JSONBase *count = json->GetValue(CSTR("sites.count"));
	Text::JSONBase *sites = json->GetValue(CSTR("sites.site"));
	if (count && sites && (count->GetType() == Text::JSONType::Number || count->GetType() == Text::JSONType::INT32) && sites->GetType() == Text::JSONType::Array)
	{
		if (totalCount)
		{
			*totalCount = (UOSInt)count->GetAsInt32();
		}
		Text::JSONArray *siteArr = (Text::JSONArray*)sites;
		UOSInt i = 0;
		UOSInt j = siteArr->GetArrayLength();
		while (i < j)
		{
			Text::JSONObject *siteObj = (Text::JSONObject*)siteArr->GetArrayValue(i);
			Site *site;
			Text::String *s;
			if (siteObj->GetType() == Text::JSONType::Object)
			{
				site = MemAlloc(Site, 1);
				site->id = siteObj->GetObjectInt32(CSTR("id"));
				site->name = siteObj->GetObjectNewString(CSTR("name"));
				site->accountId = siteObj->GetObjectInt32(CSTR("accountId"));
				site->status = siteObj->GetObjectNewString(CSTR("status"));
				site->peakPower_kWp = siteObj->GetObjectDouble(CSTR("peakPower"));
				s = siteObj->GetObjectString(CSTR("lastUpdateTime"));
				if (s == 0)
					site->lastUpdateTime = 0;
				else
					site->lastUpdateTime = Data::Timestamp::FromStr(s->ToCString(), Data::DateTimeUtil::GetLocalTzQhr());
				site->currency = siteObj->GetObjectNewString(CSTR("currency"));
				s = siteObj->GetObjectString(CSTR("installationDate"));
				if (s == 0)
					site->installationDate = 0;
				else
					site->installationDate = Data::Timestamp::FromStr(s->ToCString(), Data::DateTimeUtil::GetLocalTzQhr());
				s = siteObj->GetObjectString(CSTR("ptoDate"));
				if (s == 0)
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

void Net::SolarEdgeAPI::FreeSiteList(Data::ArrayList<Site*> *siteList)
{
	Site *site;
	UOSInt i = siteList->GetCount();
	while (i-- > 0)
	{
		site = siteList->GetItem(i);
		SDEL_STRING(site->name);
		SDEL_STRING(site->status);
		SDEL_STRING(site->currency);
		SDEL_STRING(site->notes);
		SDEL_STRING(site->type);
		SDEL_STRING(site->country);
		SDEL_STRING(site->city);
		SDEL_STRING(site->address);
		SDEL_STRING(site->address2);
		SDEL_STRING(site->zip);
		SDEL_STRING(site->timeZone);
		SDEL_STRING(site->countryCode);
		SDEL_STRING(site->publicName);
		MemFree(site);
	}
	siteList->Clear();
}

Bool Net::SolarEdgeAPI::GetSiteOverview(Int32 siteId, SiteOverview *overview)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	sptr = Text::StrConcatC(sbuff, UTF8STRC("/site/"));
	sptr = Text::StrInt32(sptr, siteId);
	sptr = Text::StrConcatC(sptr, UTF8STRC("/overview"));
	Text::StringBuilderUTF8 sbURL;
	this->BuildURL(&sbURL, CSTRP(sbuff, sptr));
	Text::JSONBase *json = this->GetJSON(sbURL.ToCString());
	if (json == 0)
		return false;
	Text::String *s = json->GetValueString(CSTR("overview.lastUpdateTime"));
	if (s)
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
