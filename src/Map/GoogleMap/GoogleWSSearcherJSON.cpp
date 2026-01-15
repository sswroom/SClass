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

Map::GoogleMap::GoogleWSSearcherJSON::GoogleWSSearcherJSON(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, NN<IO::Writer> errWriter, Optional<Text::EncodingFactory> encFact)
{
	this->clif = clif;
	this->ssl = ssl;
	this->errWriter = errWriter;
	this->encFact = encFact;
	this->lastIsError = 0;
	this->srchCnt = 0;
	this->lastSrchDate.SetCurrTimeUTC();
	this->gooCliId = nullptr;
	this->gooPrivKey = nullptr;
	this->gooPrivKeyLeng = 0;
	this->gooAPIKey = nullptr;
}

Map::GoogleMap::GoogleWSSearcherJSON::~GoogleWSSearcherJSON()
{
	OPTSTR_DEL(this->gooCliId);
	UnsafeArray<UInt8> gooPrivKey;
	if (this->gooPrivKey.SetTo(gooPrivKey))
	{
		MemFreeArr(gooPrivKey);
		this->gooPrivKey = nullptr;
	}
	OPTSTR_DEL(this->gooAPIKey);
}

void Map::GoogleMap::GoogleWSSearcherJSON::SetGoogleClientId(Optional<Text::String> gooCliId, Optional<Text::String> gooPrivKey)
{
	OPTSTR_DEL(this->gooCliId);
	UnsafeArray<UInt8> gooPrivKeyArr;
	if (this->gooPrivKey.SetTo(gooPrivKeyArr))
	{
		MemFreeArr(gooPrivKeyArr);
		this->gooPrivKey = nullptr;
	}
	this->gooPrivKeyLeng = 0;
	NN<Text::String> nngooCliId;
	NN<Text::String> nngooPrivKey;
	if (gooCliId.SetTo(nngooCliId) && gooPrivKey.SetTo(nngooPrivKey))
	{
		Text::TextBinEnc::Base64Enc b64(Text::TextBinEnc::Base64Enc::Charset::URL, false);
		this->gooCliId = nngooCliId->Clone();
		UnsafeArray<UInt8> gooPrivKeyArr;
		this->gooPrivKey = gooPrivKeyArr = MemAllocArr(UInt8, nngooPrivKey->leng + 1);
		this->gooPrivKeyLeng = b64.DecodeBin(nngooPrivKey->ToCString(), gooPrivKeyArr);
	}
}

void Map::GoogleMap::GoogleWSSearcherJSON::SetGoogleClientId(Text::CString gooCliId, Text::CString gooPrivKey)
{
	OPTSTR_DEL(this->gooCliId);
	UnsafeArray<UInt8> gooPrivKeyArr;
	if (this->gooPrivKey.SetTo(gooPrivKeyArr))
	{
		MemFreeArr(gooPrivKeyArr);
		this->gooPrivKey = nullptr;
	}
	this->gooPrivKeyLeng = 0;
	Text::CStringNN nngooCliId;
	if (gooCliId.SetTo(nngooCliId) && nngooCliId.leng > 0)
	{
		Text::TextBinEnc::Base64Enc b64(Text::TextBinEnc::Base64Enc::Charset::URL, false);
		this->gooCliId = Text::String::New(nngooCliId).Ptr();
		this->gooPrivKey = gooPrivKeyArr = MemAlloc(UInt8, gooPrivKey.leng + 1);
		this->gooPrivKeyLeng = b64.DecodeBin(gooPrivKey.OrEmpty(), gooPrivKeyArr);
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

UnsafeArrayOpt<UTF8Char> Map::GoogleMap::GoogleWSSearcherJSON::SearchName(UnsafeArray<UTF8Char> buff, UIntOS buffSize, Math::Coord2DDbl pos, UnsafeArrayOpt<const UTF8Char> lang)
{
	UTF8Char url[1024];
	UnsafeArray<UTF8Char> urlStart;
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<const UTF8Char> nnlang;
	Data::DateTime currDt;
	Int32 i;
	UInt8 databuff[2048];
	UIntOS readSize;

	Sync::MutexUsage mutUsage(this->mut);
	this->srchCnt++;
	currDt.SetCurrTimeUTC();
	this->lastIsError = 0;
	if ((i = (Int32)currDt.DiffMS(this->lastSrchDate)) < 200)
	{
		if (i >= 0)
		{
			Sync::SimpleThread::Sleep((UIntOS)(200 - i));
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
	NN<Text::String> gooCliId;
	UnsafeArray<UInt8> gooPrivKey;
	if (this->gooCliId.SetTo(gooCliId) && this->gooPrivKey.SetTo(gooPrivKey))
	{
		sptr = Text::StrConcatC(sptr, UTF8STRC("&client="));
		sptr = gooCliId->ConcatTo(sptr);

		UInt8 result[20];
		Crypto::Hash::SHA1 sha;
		Crypto::Hash::HMAC hmac(sha, gooPrivKey, this->gooPrivKeyLeng);
		hmac.Calc(urlStart, (UIntOS)(sptr - urlStart));
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

	cli = Net::HTTPClient::CreateConnect(this->clif, this->ssl, CSTRP(url, sptr), Net::WebUtil::RequestMethod::HTTP_GET, true);
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
			NN<Text::JSONBase> obj;
			if (Text::JSONBase::ParseJSONStr(sb.ToCString()).SetTo(obj))
			{
				if (obj->GetType() == Text::JSONType::Object)
				{
					NN<Text::JSONBase> base;
					NN<Text::JSONObject> jobj = NN<Text::JSONObject>::ConvertFrom(obj);
					if (jobj->GetObjectValue(CSTR("status")).SetTo(base) && base->Equals(CSTR("OK")))
					{
						UIntOS i;
						UIntOS j;
						UIntOS bestResult = 0;
						NN<Text::JSONObject> result;
						NN<Text::JSONArray> resultType;
						NN<Text::JSONArray> arr;
						if (jobj->GetObjectArray(CSTR("results")).SetTo(arr))
						{
							j = arr->GetArrayLength();
							i = 0;
							while (i < j)
							{
								if (arr->GetArrayObject(i).SetTo(result))
								{
									if (result->GetObjectArray(CSTR("types")).SetTo(resultType) && resultType->GetArrayLength() > 0)
									{
										if (resultType->GetArrayValue(0).SetTo(base) && base->Equals(CSTR("street_address")))
										{
											bestResult = i;
											break;
										}
									}
								}
								i++;
							}
							if (j > 0 && arr->GetArrayObject(bestResult).SetTo(result) && result->GetObjectValue(CSTR("formatted_address")).SetTo(base) && base->GetType() == Text::JSONType::String)
							{
								buff = Text::StrConcatS(buff, NN<Text::JSONString>::ConvertFrom(base)->GetValue()->v, buffSize);
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
						if (jobj->GetObjectValue(CSTR("status")).SetTo(base) && base->GetType() == Text::JSONType::String)
						{
							NN<Text::JSONString> jstr = NN<Text::JSONString>::ConvertFrom(base);
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
						else
						{
							errWriter->WriteLine(CSTR("Google JSON Status not found error"));
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
				fs.Write(sb.ToByteArray());
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
		sb.AppendC(url, (UIntOS)(sptr - url));
		errWriter->WriteLine(sb.ToCString());
	}

	this->lastSrchDate.SetCurrTimeUTC();
	cli.Delete();
	return buff;
}

UnsafeArrayOpt<UTF8Char> Map::GoogleMap::GoogleWSSearcherJSON::SearchName(UnsafeArray<UTF8Char> buff, UIntOS buffSize, Math::Coord2DDbl pos, UInt32 lcid)
{
	if (this->lastIsError == 2)
	{
		Data::DateTime dt;
		dt.SetCurrTimeUTC();
		if (dt.DiffMS(this->lastSrchDate) < 60000)
			return nullptr;
	}
	NN<Text::Locale::LocaleEntry> ent;
	if (!Text::Locale::GetLocaleEntry(lcid).SetTo(ent))
		return nullptr;
	return SearchName(buff, buffSize, pos, ent->shortName);
}

UnsafeArrayOpt<UTF8Char> Map::GoogleMap::GoogleWSSearcherJSON::CacheName(UnsafeArray<UTF8Char> buff, UIntOS buffSize, Math::Coord2DDbl pos, UInt32 lcid)
{
	if (this->lastIsError != 0)
	{
		Data::DateTime dt;
		dt.SetCurrTimeUTC();
		if (dt.DiffMS(this->lastSrchDate) < 60000)
			return nullptr;
	}
	NN<Text::Locale::LocaleEntry> ent;
	if (!Text::Locale::GetLocaleEntry(lcid).SetTo(ent))
		return nullptr;
	return SearchName(buff, buffSize, pos, ent->shortName);
}

UInt32 Map::GoogleMap::GoogleWSSearcherJSON::GetSrchCnt()
{
	return this->srchCnt;
}
