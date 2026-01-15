#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/MemoryStream.h"
#include "Map/GoogleMap/GoogleWSSearcherXML.h"
#include "Net/HTTPClient.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/Locale.h"
#include "Text/MyStringFloat.h"
#include "Text/XMLDOM.h"

Map::GoogleMap::GoogleWSSearcherXML::GoogleWSSearcherXML(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, IO::Writer *errWriter, NN<Text::EncodingFactory> encFact)
{
	this->sockf = sockf;
	this->ssl = ssl;
	this->errWriter = errWriter;
	this->encFact = encFact;
	this->lastIsError = false;
	this->lastSrchDate.SetCurrTimeUTC();
}

Map::GoogleMap::GoogleWSSearcherXML::~GoogleWSSearcherXML()
{
}

UTF8Char *Map::GoogleMap::GoogleWSSearcherXML::SearchName(UTF8Char *buff, UIntOS buffSize, Double lat, Double lon, Text::CString lang)
{
	UTF8Char url[1024];
	UTF8Char *sptr;
	Data::DateTime currDt;
	UInt8 databuff[2048];
	Int32 i;

	Sync::MutexUsage mutUsage(this->mut);
	currDt.SetCurrTimeUTC();
	this->lastIsError = false;
	if ((i = (Int32)currDt.DiffMS(this->lastSrchDate)) < 200)
	{
		Sync::SimpleThread::Sleep(200 - i);
	}

	NN<Net::HTTPClient> cli;
	sptr = Text::StrConcatC(url, UTF8STRC("http://maps.googleapis.com/maps/api/geocode/xml?latlng="));
	sptr = Text::StrDouble(sptr, lat);
	sptr = Text::StrConcatC(sptr, UTF8STRC(","));
	sptr = Text::StrDouble(sptr, lon);
	sptr = Text::StrConcatC(sptr, UTF8STRC("&sensor=false"));

	cli = Net::HTTPClient::CreateConnect(this->sockf, this->ssl, CSTRP(url, sptr), Net::WebUtil::RequestMethod::HTTP_GET, true);
	if (!cli->IsError())
	{
		if (lang.leng > 0)
		{
			cli->AddHeaderC(CSTR("Accept-Language"), lang);
		}
		Int32 status = cli->GetRespStatus();
		UIntOS readSize;
		IO::MemoryStream mstm;
		while ((readSize = cli->Read(Data::ByteArray(databuff, 2048))) > 0)
		{
			mstm.Write(databuff, readSize);
		}
		if (status == 200)
		{
			Text::StringBuilderUTF8 sb;
			UInt8 *xmlBuff = mstm.GetBuff(readSize);
			Text::XMLDocument doc;
			if (doc.ParseBuff(this->encFact, xmlBuff, readSize))
			{
				Bool succ = false;
				Text::XMLNode **result;
				UIntOS resultCnt;
				result = doc.SearchNode(CSTR("/GeocodeResponse/status"), &resultCnt);
				if (resultCnt == 1)
				{
					result[0]->GetInnerXML(sb);
					if (Text::StrEqualsICaseC(sb.ToString(), sb.GetLength(), UTF8STRC("OK")))
					{
						succ = true;
					}
					else
					{
						this->lastIsError = true;
						errWriter->Write(CSTR("Google respose status invalid: "));
						errWriter->WriteLine(sb.ToCString());
					}
				}
				else
				{
					this->lastIsError = true;
					errWriter->WriteLine(CSTR("Invalid google response content"));
				}
				if (result)
				{
					doc.ReleaseSearch(result);
				}
				if (succ)
				{
					result = doc.SearchNode(CSTR("/GeocodeResponse/result[type='street_address']/formatted_address"), &resultCnt);
					if (resultCnt > 0)
					{
						sb.ClearStr();
						result[0]->GetInnerXML(sb);
						buff = Text::StrConcatS(buff, sb.ToString(), buffSize);
					}
					if (result)
					{
						doc.ReleaseSearch(result);
					}
				}
			}
			else
			{
				this->lastIsError = true;
				*buff = 0;
			}
		}
		else
		{
			sptr = Text::StrConcatC(url, UTF8STRC("Google "));
			sptr = Text::StrInt32(sptr, status);
			sptr = Text::StrConcatC(sptr, UTF8STRC(" Error"));
			errWriter->WriteLineC(url, (UIntOS)(sptr - url));
			*buff = 0;
		}
	}
	else
	{
		*buff = 0;
	}

	this->lastSrchDate.SetCurrTimeUTC();
	cli.Delete();
	return buff;
}

UTF8Char *Map::GoogleMap::GoogleWSSearcherXML::SearchName(UTF8Char *buff, UIntOS buffSize, Double lat, Double lon, Int32 lcid)
{
	Text::Locale::LocaleEntry *ent = Text::Locale::GetLocaleEntry(lcid);
	if (ent == 0)
		return 0;
	return SearchName(buff, buffSize, lat, lon, {ent->shortName, ent->shortNameLen});
}

UTF8Char *Map::GoogleMap::GoogleWSSearcherXML::CacheName(UTF8Char *buff, UIntOS buffSize, Double lat, Double lon, Int32 lcid)
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
	return SearchName(buff, buffSize, lat, lon, {ent->shortName, ent->shortNameLen});
}
