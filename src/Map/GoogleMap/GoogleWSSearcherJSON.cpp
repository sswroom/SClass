#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Hash/HMAC.h"
#include "Crypto/Hash/SHA1.h"
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "Map/GoogleMap/GoogleWSSearcherJSON.h"
#include "Net/HTTPClient.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"
#include "Text/JSON.h"
#include "Text/MyStringFloat.h"
#include "Text/Locale.h"
#include "Text/TextBinEnc/Base64Enc.h"

Map::GoogleMap::GoogleWSSearcherJSON::GoogleWSSearcherJSON(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, NN<IO::Writer> errWriter, Optional<Text::EncodingFactory> encFact)
{
	this->sockf = sockf;
	this->ssl = ssl;
	this->errWriter = errWriter;
	this->encFact = encFact;
	this->lastIsError = 0;
	this->srchCnt = 0;
	this->lastSrchDate.SetCurrTimeUTC();
	this->gooCliId = 0;
	this->gooPrivKey = 0;
	this->gooPrivKeyLeng = 0;
	this->gooAPIKey = 0;
}

Map::GoogleMap::GoogleWSSearcherJSON::~GoogleWSSearcherJSON()
{
	SDEL_STRING(this->gooCliId);
	if (this->gooPrivKey)
	{
		MemFree(this->gooPrivKey);
		this->gooPrivKey = 0;
	}
	OPTSTR_DEL(this->gooAPIKey);
}

void Map::GoogleMap::GoogleWSSearcherJSON::SetGoogleClientId(Text::String *gooCliId, Text::String *gooPrivKey)
{
	SDEL_STRING(this->gooCliId);
	if (this->gooPrivKey)
	{
		MemFree(this->gooPrivKey);
		this->gooPrivKey = 0;
	}
	this->gooPrivKeyLeng = 0;
	if (gooCliId)
	{
		Text::TextBinEnc::Base64Enc b64(Text::TextBinEnc::Base64Enc::Charset::URL, false);
		this->gooCliId = gooCliId->Clone().Ptr();
		this->gooPrivKey = MemAlloc(UInt8, gooPrivKey->leng + 1);
		this->gooPrivKeyLeng = b64.DecodeBin(gooPrivKey->ToCString(), this->gooPrivKey);
	}
}

void Map::GoogleMap::GoogleWSSearcherJSON::SetGoogleClientId(Text::CString gooCliId, Text::CString gooPrivKey)
{
	SDEL_STRING(this->gooCliId);
	if (this->gooPrivKey)
	{
		MemFree(this->gooPrivKey);
		this->gooPrivKey = 0;
	}
	this->gooPrivKeyLeng = 0;
	if (gooCliId.leng > 0)
	{
		Text::TextBinEnc::Base64Enc b64(Text::TextBinEnc::Base64Enc::Charset::URL, false);
		this->gooCliId = Text::String::New(gooCliId).Ptr();
		this->gooPrivKey = MemAlloc(UInt8, gooPrivKey.leng + 1);
		this->gooPrivKeyLeng = b64.DecodeBin(gooPrivKey.OrEmpty(), this->gooPrivKey);
	}
}

void Map::GoogleMap::GoogleWSSearcherJSON::SetGoogleAPIKey(Optional<Text::String> gooAPIKey)
{
	OPTSTR_DEL(this->gooAPIKey);
	this->gooAPIKey = Text::String::CopyOrNull(gooAPIKey);
}

void Map::GoogleMap::GoogleWSSearcherJSON::SetGoogleAPIKey(Text::CString gooAPIKey)
{
	OPTSTR_DEL(this->gooAPIKey);
	this->gooAPIKey = Text::String::NewOrNull(gooAPIKey);
}

UnsafeArrayOpt<UTF8Char> Map::GoogleMap::GoogleWSSearcherJSON::SearchName(UnsafeArray<UTF8Char> buff, UOSInt buffSize, Math::Coord2DDbl pos, UnsafeArrayOpt<const UTF8Char> lang)
{
	UTF8Char url[1024];
	UnsafeArray<UTF8Char> urlStart;
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<const UTF8Char> nnlang;
	Data::DateTime currDt;
	Int32 i;
	UInt8 databuff[2048];
	UOSInt readSize;

	Sync::MutexUsage mutUsage(this->mut);
	this->srchCnt++;
	currDt.SetCurrTimeUTC();
	this->lastIsError = 0;
	if ((i = (Int32)currDt.DiffMS(this->lastSrchDate)) < 200)
	{
		if (i >= 0)
		{
			Sync::SimpleThread::Sleep((UOSInt)(200 - i));
		}
	}

	NN<Text::String> s;
	NN<Net::HTTPClient> cli;
	urlStart = sptr = Text::StrConcatC(url, UTF8STRC("https://maps.googleapis.com"));
	sptr = Text::StrConcatC(sptr, UTF8STRC("/maps/api/geocode/json?latlng="));
	sptr = Text::StrDouble(sptr, pos.GetLat());
	sptr = Text::StrConcatC(sptr, UTF8STRC(","));
	sptr = Text::StrDouble(sptr, pos.GetLon());
//	sptr = Text::StrConcatC(sptr, UTF8STRC("&sensor=false"));
	if (lang.SetTo(nnlang))
	{
		sptr = Text::StrConcatC(sptr, UTF8STRC("&language="));
		sptr = Text::StrConcat(sptr, nnlang);
	}
	if (this->gooCliId)
	{
		sptr = Text::StrConcatC(sptr, UTF8STRC("&client="));
		sptr = this->gooCliId->ConcatTo(sptr);

		UInt8 result[20];
		Crypto::Hash::SHA1 sha;
		Crypto::Hash::HMAC hmac(sha, this->gooPrivKey, this->gooPrivKeyLeng);
		hmac.Calc(urlStart, (UOSInt)(sptr - urlStart));
		hmac.GetValue(result);
		Text::TextBinEnc::Base64Enc b64(Text::TextBinEnc::Base64Enc::Charset::URL, false);
		sptr = Text::StrConcatC(sptr, UTF8STRC("&signature="));
		sptr = b64.EncodeBin(sptr, result, 20);
	}
	else if (this->gooAPIKey.SetTo(s))
	{
		sptr = Text::StrConcatC(sptr, UTF8STRC("&key="));
		sptr = s->ConcatTo(sptr);
	}

	cli = Net::HTTPClient::CreateConnect(this->sockf, this->ssl, CSTRP(url, sptr), Net::WebUtil::RequestMethod::HTTP_GET, true);
	if (!cli->IsError())
	{
		if (lang.SetTo(nnlang))
		{
			cli->AddHeaderC(CSTR("Accept-Language"), {nnlang, Text::StrCharCnt(nnlang)});
		}
		Int32 status = cli->GetRespStatus();

		Text::StringBuilderUTF8 sb;
		while ((readSize = cli->Read(BYTEARR(databuff))) > 0)
		{
			sb.AppendC(databuff, readSize);
		}

		if (status == 200)
		{
			Text::JSONBase *obj = Text::JSONBase::ParseJSONStr(sb.ToCString());
			if (obj)
			{
				if (obj->GetType() == Text::JSONType::Object)
				{
					Text::JSONObject *jobj = (Text::JSONObject*)obj;
					if (jobj->GetObjectValue(CSTR("status"))->Equals(CSTR("OK")))
					{
						UOSInt i;
						UOSInt j;
						UOSInt bestResult = 0;
						Text::JSONObject *result;
						Text::JSONArray *resultType;
						Text::JSONArray *arr = (Text::JSONArray*)jobj->GetObjectValue(CSTR("results"));
						if (arr)
						{
							j = arr->GetArrayLength();
							i = 0;
							while (i < j)
							{
								result = (Text::JSONObject*)arr->GetArrayValue(i);
								if (result)
								{
									resultType = (Text::JSONArray*)result->GetObjectValue(CSTR("types"));
									if (resultType && resultType->GetArrayLength() > 0)
									{
										if (resultType->GetArrayValue(0)->Equals(CSTR("street_address")))
										{
											bestResult = i;
											break;
										}
									}
								}
								i++;
							}
							if (j > 0)
							{
								result = (Text::JSONObject*)arr->GetArrayValue(bestResult);
								buff = Text::StrConcatS(buff, ((Text::JSONString*)result->GetObjectValue(CSTR("formatted_address")))->GetValue()->v, buffSize);
							}
							else
							{
								errWriter->WriteLine(CSTR("Google JSON empty results"));
							}
						}
						else
						{
							errWriter->WriteLine(CSTR("Google JSON results not found"));
						}
					}
					else
					{
						Text::JSONString *jstr = (Text::JSONString*)jobj->GetObjectValue(CSTR("status"));
						if (jstr->GetValue()->Equals(UTF8STRC("ZERO_RESULTS")))
						{
							buff = Text::StrConcatS(buff, (const UTF8Char*)"-", buffSize);
						}
						else
						{
							sptr = Text::StrConcatC(url, UTF8STRC("Google JSON Status "));
							sptr = jstr->GetValue()->ConcatTo(sptr);
							sptr = Text::StrConcatC(sptr, UTF8STRC(" Error"));
							errWriter->WriteLine(CSTRP(url, sptr));
							this->lastIsError = 1;
						}
					}
				}
				else
				{
					errWriter->WriteLine(CSTR("Google JSON invalid status"));
					this->lastIsError = 1;
				}
				obj->EndUse();
			}
			else
			{
				errWriter->WriteLine(CSTR("Google non-json Error"));
				IO::FileStream fs(CSTR("nonjsonerr.txt"), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
				fs.Write(sb.ToString(), sb.GetLength());
				this->lastIsError = 1;
			}
		}
		else
		{
			errWriter->WriteLine(CSTRP(url, sptr));
			sptr = Text::StrConcatC(url, UTF8STRC("Google HTTP "));
			sptr = Text::StrInt32(sptr, status);
			sptr = Text::StrConcatC(sptr, UTF8STRC(" Error"));
			errWriter->WriteLine(CSTRP(url, sptr));
			*buff = 0;
			this->lastIsError = 1;
		}
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		*buff = 0;
		this->lastIsError = 2;
		sb.AppendC(UTF8STRC("Cannot connect: "));
		sb.AppendC(url, (UOSInt)(sptr - url));
		errWriter->WriteLine(sb.ToCString());
	}

	this->lastSrchDate.SetCurrTimeUTC();
	cli.Delete();
	return buff;
}

UnsafeArrayOpt<UTF8Char> Map::GoogleMap::GoogleWSSearcherJSON::SearchName(UnsafeArray<UTF8Char> buff, UOSInt buffSize, Math::Coord2DDbl pos, UInt32 lcid)
{
	if (this->lastIsError == 2)
	{
		Data::DateTime dt;
		dt.SetCurrTimeUTC();
		if (dt.DiffMS(this->lastSrchDate) < 60000)
			return 0;
	}
	NN<Text::Locale::LocaleEntry> ent;
	if (!Text::Locale::GetLocaleEntry(lcid).SetTo(ent))
		return 0;
	return SearchName(buff, buffSize, pos, ent->shortName);
}

UnsafeArrayOpt<UTF8Char> Map::GoogleMap::GoogleWSSearcherJSON::CacheName(UnsafeArray<UTF8Char> buff, UOSInt buffSize, Math::Coord2DDbl pos, UInt32 lcid)
{
	if (this->lastIsError != 0)
	{
		Data::DateTime dt;
		dt.SetCurrTimeUTC();
		if (dt.DiffMS(this->lastSrchDate) < 60000)
			return 0;
	}
	NN<Text::Locale::LocaleEntry> ent;
	if (!Text::Locale::GetLocaleEntry(lcid).SetTo(ent))
		return 0;
	return SearchName(buff, buffSize, pos, ent->shortName);
}

UInt32 Map::GoogleMap::GoogleWSSearcherJSON::GetSrchCnt()
{
	return this->srchCnt;
}
