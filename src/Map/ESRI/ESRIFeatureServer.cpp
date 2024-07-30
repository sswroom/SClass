#include "Stdafx.h"
#include "Map/ESRI/ESRIFeatureServer.h"

Map::ESRI::ESRIFeatureServer::ESRIFeatureServer(Text::CStringNN url, NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i;
	this->url = Text::String::New(url);
	this->clif = clif;
	this->ssl = ssl;
//	this->csys = Math::CoordinateSystemManager::CreateDefaultCsys();

	sptr = url.ConcatTo(sbuff);
	if (Text::StrEndsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("/FeatureServer")))
	{
		sptr -= 14;
		*sptr = 0;
	}
	i = Text::StrIndexOfC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("/services/"));
	if (i != INVALID_INDEX)
	{
		this->name = Text::String::NewP(&sbuff[i + 10], sptr);
	}
	else
	{
		i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), '/');
		this->name = Text::String::NewP(&sbuff[i + 1], sptr);
	}
}

Map::ESRI::ESRIFeatureServer::~ESRIFeatureServer()
{
	this->url->Release();
	this->name->Release();
}

Optional<Map::ESRI::ESRIFeatureServer::LayerInfo> Map::ESRI::ESRIFeatureServer::GetLayerInfo()
{
	Text::StringBuilderUTF8 sb;
	sb.Append(this->url);
	sb.Append(CSTR("/0?f=json"));
	JSONREQ_RET(this->clif, this->ssl, sb.ToCString(), LayerInfo)
}

NN<Text::String> Map::ESRI::ESRIFeatureServer::GetURL() const
{
	return this->url;
}

NN<Text::String> Map::ESRI::ESRIFeatureServer::GetName() const
{
	return this->name;
}
