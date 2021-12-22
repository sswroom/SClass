#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Hash/HMAC.h"
#include "Crypto/Hash/SHA1.h"
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "IO/StreamReader.h"
#include "Map/GoogleMap/GoogleWSSearcherJSON.h"
#include "Net/HTTPClient.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/JSON.h"
#include "Text/MyStringFloat.h"
#include "Text/Locale.h"
#include "Text/TextBinEnc/Base64Enc.h"

Map::GoogleMap::GoogleWSSearcherJSON::GoogleWSSearcherJSON(Net::SocketFactory *sockf, Net::SSLEngine *ssl, IO::Writer *errWriter, Text::EncodingFactory *encFact)
{
	this->sockf = sockf;
	this->ssl = ssl;
	this->errWriter = errWriter;
	this->encFact = encFact;
	this->lastIsError = 0;
	this->srchCnt = 0;
	NEW_CLASS(this->lastSrchDate, Data::DateTime());
	NEW_CLASS(this->mut, Sync::Mutex());
	this->lastSrchDate->SetCurrTimeUTC();
	this->gooCliId = 0;
	this->gooPrivKey = 0;
	this->gooPrivKeyLeng = 0;
	this->gooAPIKey = 0;
}

Map::GoogleMap::GoogleWSSearcherJSON::~GoogleWSSearcherJSON()
{
	DEL_CLASS(this->lastSrchDate);
	DEL_CLASS(this->mut);
	SDEL_STRING(this->gooCliId);
	if (this->gooPrivKey)
	{
		MemFree(this->gooPrivKey);
		this->gooPrivKey = 0;
	}
	SDEL_STRING(this->gooAPIKey);
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
		this->gooCliId = gooCliId->Clone();
		this->gooPrivKey = MemAlloc(UInt8, gooPrivKey->leng + 1);
		this->gooPrivKeyLeng = b64.DecodeBin(gooPrivKey->v, gooPrivKey->leng, this->gooPrivKey);
	}
}

void Map::GoogleMap::GoogleWSSearcherJSON::SetGoogleClientId(const UTF8Char *gooCliId, const UTF8Char *gooPrivKey)
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
		UOSInt len = Text::StrCharCnt(gooPrivKey);
		this->gooCliId = Text::String::NewNotNull(gooCliId);
		this->gooPrivKey = MemAlloc(UInt8, len + 1);
		this->gooPrivKeyLeng = b64.DecodeBin(gooPrivKey, len, this->gooPrivKey);
	}
}

void Map::GoogleMap::GoogleWSSearcherJSON::SetGoogleAPIKey(Text::String *gooAPIKey)
{
	SDEL_STRING(this->gooAPIKey);
	this->gooAPIKey = SCOPY_STRING(gooAPIKey);
}

void Map::GoogleMap::GoogleWSSearcherJSON::SetGoogleAPIKey(const UTF8Char *gooAPIKey)
{
	SDEL_STRING(this->gooAPIKey);
	this->gooAPIKey = Text::String::NewOrNull(gooAPIKey);
}

UTF8Char *Map::GoogleMap::GoogleWSSearcherJSON::SearchName(UTF8Char *buff, UOSInt buffSize, Double lat, Double lon, const UTF8Char *lang)
{
	UTF8Char url[1024];
	UTF8Char *urlStart;
	UTF8Char *sptr;
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
			Sync::Thread::Sleep((UOSInt)(200 - i));
		}
	}

	Net::HTTPClient *cli;
	urlStart = sptr = Text::StrConcat(url, (const UTF8Char*)"https://maps.googleapis.com");
	sptr = Text::StrConcat(sptr, (const UTF8Char*)"/maps/api/geocode/json?latlng=");
	sptr = Text::StrDouble(sptr, lat);
	sptr = Text::StrConcat(sptr, (const UTF8Char*)",");
	sptr = Text::StrDouble(sptr, lon);
//	sptr = Text::StrConcat(sptr, L"&sensor=false");
	if (lang)
	{
		sptr = Text::StrConcat(sptr, (const UTF8Char*)"&language=");
		sptr = Text::StrConcat(sptr, lang);
	}
	if (this->gooCliId)
	{
		sptr = Text::StrConcat(sptr, (const UTF8Char*)"&client=");
		sptr = this->gooCliId->ConcatTo(sptr);

		UInt8 result[20];
		Crypto::Hash::SHA1 sha;
		Crypto::Hash::HMAC hmac(&sha, this->gooPrivKey, this->gooPrivKeyLeng);
		hmac.Calc(urlStart, (UOSInt)(sptr - urlStart));
		hmac.GetValue(result);
		Text::TextBinEnc::Base64Enc b64(Text::TextBinEnc::Base64Enc::Charset::URL, false);
		sptr = Text::StrConcat(sptr, (const UTF8Char*)"&signature=");
		sptr = b64.EncodeBin(sptr, result, 20);
	}
	else if (this->gooAPIKey)
	{
		sptr = Text::StrConcat(sptr, (const UTF8Char*)"&key=");
		sptr = this->gooAPIKey->ConcatTo(sptr);
	}

	cli = Net::HTTPClient::CreateConnect(this->sockf, this->ssl, url, "GET", true);
	if (!cli->IsError())
	{
		if (lang)
		{
			cli->AddHeader((const UTF8Char*)"Accept-Language", lang);
		}
		Int32 status = cli->GetRespStatus();
		IO::StreamReader *reader;

		IO::MemoryStream *mstm;
		NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"Map.GoogleMap.GoogleWSSearcherJSON.SearchName"));
		while ((readSize = cli->Read(databuff, 2048)) > 0)
		{
			mstm->Write(databuff, readSize);
		}
		mstm->SeekFromBeginning(0);
		NEW_CLASS(reader, IO::StreamReader(mstm, 65001));
		Text::StringBuilderUTF8 sb;
		reader->ReadToEnd(&sb);
//		sb.Append(reader);
		DEL_CLASS(reader);
		DEL_CLASS(mstm);

		if (status == 200)
		{
			Text::JSONBase *obj = Text::JSONBase::ParseJSONStr(sb.ToString());
			if (obj)
			{
				if (obj->GetType() == Text::JSONType::Object)
				{
					Text::JSONObject *jobj = (Text::JSONObject*)obj;
					if (jobj->GetObjectValue((const UTF8Char*)"status")->Equals((const UTF8Char*)"OK"))
					{
						UOSInt i;
						UOSInt j;
						UOSInt bestResult = 0;
						Text::JSONObject *result;
						Text::JSONArray *resultType;
						Text::JSONArray *arr = (Text::JSONArray*)jobj->GetObjectValue((const UTF8Char*)"results");
						if (arr)
						{
							j = arr->GetArrayLength();
							i = 0;
							while (i < j)
							{
								result = (Text::JSONObject*)arr->GetArrayValue(i);
								if (result)
								{
									resultType = (Text::JSONArray*)result->GetObjectValue((const UTF8Char*)"types");
									if (resultType && resultType->GetArrayLength() > 0)
									{
										if (resultType->GetArrayValue(0)->Equals((const UTF8Char*)"street_address"))
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
								buff = Text::StrConcatS(buff, ((Text::JSONString*)result->GetObjectValue((const UTF8Char*)"formatted_address"))->GetValue()->v, buffSize);
							}
							else
							{
								errWriter->WriteLine((const UTF8Char*)"Google JSON empty results");
							}
						}
						else
						{
							errWriter->WriteLine((const UTF8Char*)"Google JSON results not found");
						}
					}
					else
					{
						Text::JSONString *jstr = (Text::JSONString*)jobj->GetObjectValue((const UTF8Char*)"status");
						if (jstr->GetValue()->Equals((const UTF8Char*)"ZERO_RESULTS"))
						{
							buff = Text::StrConcatS(buff, (const UTF8Char*)"-", buffSize);
						}
						else
						{
							sptr = Text::StrConcat(url, (const UTF8Char*)"Google JSON Status ");
							sptr = jstr->GetValue()->ConcatTo(sptr);
							sptr = Text::StrConcat(sptr, (const UTF8Char*)" Error");
							errWriter->WriteLine(url);
							this->lastIsError = 1;
						}
					}
				}
				else
				{
					errWriter->WriteLine((const UTF8Char*)"Google JSON invalid status");
					this->lastIsError = 1;
				}
				obj->EndUse();
			}
			else
			{
				errWriter->WriteLine((const UTF8Char*)"Google non-json Error");
				IO::FileStream *fs;
				UInt8 *buff;
				UOSInt buffSize;
				NEW_CLASS(fs, IO::FileStream((const UTF8Char*)"nonjsonerr.txt", IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
				buff = mstm->GetBuff(&buffSize);
				fs->Write(buff, buffSize);
				DEL_CLASS(fs);
				this->lastIsError = 1;
			}
		}
		else
		{
			errWriter->WriteLine(url);
			sptr = Text::StrConcat(url, (const UTF8Char*)"Google HTTP ");
			sptr = Text::StrInt32(sptr, status);
			sptr = Text::StrConcat(sptr, (const UTF8Char*)" Error");
			errWriter->WriteLine(url);
			*buff = 0;
			this->lastIsError = 1;
		}
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		*buff = 0;
		this->lastIsError = 2;
		sb.Append((const UTF8Char*)"Cannot connect: ");
		sb.Append(url);
		errWriter->WriteLine(sb.ToString());
	}

	this->lastSrchDate->SetCurrTimeUTC();
	DEL_CLASS(cli);
	mutUsage.EndUse();
	return buff;
}

UTF8Char *Map::GoogleMap::GoogleWSSearcherJSON::SearchName(UTF8Char *buff, UOSInt buffSize, Double lat, Double lon, UInt32 lcid)
{
	if (this->lastIsError == 2)
	{
		Data::DateTime dt;
		dt.SetCurrTimeUTC();
		if (dt.DiffMS(this->lastSrchDate) < 60000)
			return 0;
	}
	Text::Locale::LocaleEntry *ent = Text::Locale::GetLocaleEntry(lcid);
	if (ent == 0)
		return 0;
	return SearchName(buff, buffSize, lat, lon, ent->shortName);
}

UTF8Char *Map::GoogleMap::GoogleWSSearcherJSON::CacheName(UTF8Char *buff, UOSInt buffSize, Double lat, Double lon, UInt32 lcid)
{
	if (this->lastIsError != 0)
	{
		Data::DateTime dt;
		dt.SetCurrTimeUTC();
		if (dt.DiffMS(this->lastSrchDate) < 60000)
			return 0;
	}
	Text::Locale::LocaleEntry *ent = Text::Locale::GetLocaleEntry(lcid);
	if (ent == 0)
		return 0;
	return SearchName(buff, buffSize, lat, lon, ent->shortName);
}

UInt32 Map::GoogleMap::GoogleWSSearcherJSON::GetSrchCnt()
{
	return this->srchCnt;
}
