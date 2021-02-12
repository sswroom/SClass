#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/XMLDOM.h"
#include "Text/Locale.h"
#include "Sync/Thread.h"
#include "Net/HTTPClient.h"
#include "IO/MemoryStream.h"
#include "Map/GoogleMap/GoogleWSSearcherXML.h"

Map::GoogleMap::GoogleWSSearcherXML::GoogleWSSearcherXML(Net::SocketFactory *sockf, IO::Writer *errWriter, Text::EncodingFactory *encFact)
{
	this->sockf = sockf;
	this->errWriter = errWriter;
	this->encFact = encFact;
	this->lastIsError = false;
	NEW_CLASS(this->lastSrchDate, Data::DateTime());
	NEW_CLASS(this->mut, Sync::Mutex());
	this->lastSrchDate->SetCurrTimeUTC();
}

Map::GoogleMap::GoogleWSSearcherXML::~GoogleWSSearcherXML()
{
	DEL_CLASS(this->lastSrchDate);
	DEL_CLASS(this->mut);
}

WChar *Map::GoogleMap::GoogleWSSearcherXML::SearchName(WChar *buff, Double lat, Double lon, const WChar *lang)
{
	WChar url[1024];
	WChar *sptr;
	Data::DateTime currDt;
	UInt8 databuff[2048];
	IO::MemoryStream *mstm;
	Int32 i;

	this->mut->Lock();
	currDt.SetCurrTimeUTC();
	this->lastIsError = false;
	if ((i = (Int32)currDt.DiffMS(this->lastSrchDate)) < 200)
	{
		Sync::Thread::Sleep(200 - i);
	}

	Net::HTTPClient *cli;
	sptr = Text::StrConcat(url, L"http://maps.googleapis.com/maps/api/geocode/xml?latlng=");
	sptr = Text::StrDouble(sptr, lat);
	sptr = Text::StrConcat(sptr, L",");
	sptr = Text::StrDouble(sptr, lon);
	sptr = Text::StrConcat(sptr, L"&sensor=false");

	NEW_CLASS(cli, Net::HTTPClient(this->sockf, url, 0, true));
	if (!cli->IsError())
	{
		if (lang)
		{
			cli->AddHeader(L"Accept-Language", lang);
		}
		Int32 status = cli->GetRespStatus();
		OSInt readSize;
		NEW_CLASS(mstm, IO::MemoryStream());
		while ((readSize = cli->Read(databuff, 2048)) > 0)
		{
			mstm->Write(databuff, readSize);
		}
		if (status == 200)
		{
			Text::StringBuilder *sb;
			NEW_CLASS(sb, Text::StringBuilder());
			Text::Encoding enc(65001);
			UInt8 *xmlBuff = mstm->GetBuff(&readSize);
			Text::XMLDocument *doc;
			NEW_CLASS(doc, Text::XMLDocument());
			if (doc->ParseBuff(this->encFact, xmlBuff, readSize))
			{
				Bool succ = false;
				Text::XMLNode **result;
				OSInt resultCnt;
				result = doc->SearchNode(L"/GeocodeResponse/status", &resultCnt);
				if (resultCnt == 1)
				{
					result[0]->GetInnerXML(sb);
					if (Text::StrCompareICase(sb->ToString(), L"OK") == 0)
					{
						succ = true;
					}
					else
					{
						this->lastIsError = true;
						errWriter->Write(L"Google respose status invalid: ");
						errWriter->WriteLine(sb->ToString());
					}
				}
				else
				{
					this->lastIsError = true;
					errWriter->WriteLine(L"Invalid google response content");
				}
				if (result)
				{
					doc->ReleaseSearch(result);
				}
				if (succ)
				{
					result = doc->SearchNode(L"/GeocodeResponse/result[type='street_address']/formatted_address", &resultCnt);
					if (resultCnt > 0)
					{
						sb->ClearStr();
						result[0]->GetInnerXML(sb);
						buff = Text::StrConcat(buff, sb->ToString());
					}
					if (result)
					{
						doc->ReleaseSearch(result);
					}
				}
			}
			else
			{
				this->lastIsError = true;
				*buff = 0;
			}
			DEL_CLASS(doc);
			DEL_CLASS(sb);
		}
		else
		{
			sptr = Text::StrConcat(url, L"Google ");
			sptr = Text::StrInt32(sptr, status);
			sptr = Text::StrConcat(sptr, L" Error");
			errWriter->WriteLine(url);
			*buff = 0;
		}
		DEL_CLASS(mstm);
	}
	else
	{
		*buff = 0;
	}

	this->lastSrchDate->SetCurrTimeUTC();
	DEL_CLASS(cli);
	this->mut->Unlock();
	return buff;
}

WChar *Map::GoogleMap::GoogleWSSearcherXML::SearchName(WChar *buff, Double lat, Double lon, Int32 lcid)
{
	Text::Locale::LocaleEntry *ent = Text::Locale::GetLocaleEntry(lcid);
	if (ent == 0)
		return 0;
	return SearchName(buff, lat, lon, ent->shortName);
}

WChar *Map::GoogleMap::GoogleWSSearcherXML::CacheName(WChar *buff, Double lat, Double lon, Int32 lcid)
{
	if (this->lastIsError)
	{
		Data::DateTime dt;
		dt.SetCurrTimeUTC();
		if (dt.DiffMS(this->lastSrchDate) < 60000)
			return 0;
	}
	Text::Locale::LocaleEntry *ent = Text::Locale::GetLocaleEntry(lcid);
	if (ent == 0)
		return 0;
	return SearchName(buff, lat, lon, ent->shortName);
}
