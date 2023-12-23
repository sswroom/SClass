#include "Stdafx.h"
#include "Map/ESRI/ESRIFeatureServer.h"

Map::ESRI::ESRIFeatureServer::ESRIFeatureServer(Text::CStringNN url, NotNullPtr<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UInt8 buff[2048];
	UOSInt readSize;
	UInt32 codePage;
	UOSInt i;
	UOSInt j;
	this->url = Text::String::New(url);
	this->sockf = sockf;
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

NotNullPtr<Text::String> Map::ESRI::ESRIFeatureServer::GetURL() const
{
	return this->url;
}

NotNullPtr<Text::String> Map::ESRI::ESRIFeatureServer::GetName() const
{
	return this->name;
}
