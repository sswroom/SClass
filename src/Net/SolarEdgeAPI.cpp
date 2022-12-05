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
	Text::String *s = json->GetString(UTF8STRC("version.release"));
	if (s == 0)
	{
		s = json->GetString(UTF8STRC("version"));
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
	Text::JSONBase *supported = json->GetValue(UTF8STRC("supported"));
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
				s = obj->GetString(UTF8STRC("release"));
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
