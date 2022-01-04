#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Encrypt/Base64.h"
#include "Crypto/Hash/HMAC.h"
#include "Crypto/Hash/SHA1.h"
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "IO/StreamReader.h"
#include "Map/GoogleMap/GoogleWSSearcherJSONDotNet.h"
#include "Sync/Thread.h"
#include "Text/JSON.h"
#include "Text/Locale.h"
#include "Text/StringBuilderUTF8.h"

Map::GoogleMap::GoogleWSSearcherJSONDotNet::GoogleWSSearcherJSONDotNet(Net::SocketFactory *sockf, IO::Writer *errWriter, Text::EncodingFactory *encFact)
{
	this->sockf = sockf;
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

Map::GoogleMap::GoogleWSSearcherJSONDotNet::~GoogleWSSearcherJSONDotNet()
{
	DEL_CLASS(this->lastSrchDate);
	DEL_CLASS(this->mut);
	if (this->gooCliId)
	{
		Text::StrDelNew(this->gooCliId);
		this->gooCliId = 0;
	}
	if (this->gooPrivKey)
	{
		MemFree(this->gooPrivKey);
		this->gooPrivKey = 0;
	}
	SDEL_TEXT(this->gooAPIKey);
}

void Map::GoogleMap::GoogleWSSearcherJSONDotNet::SetGoogleClientId(const UTF8Char *gooCliId, const UTF8Char *gooPrivKey)
{
	if (this->gooCliId)
	{
		Text::StrDelNew(this->gooCliId);
		this->gooCliId = 0;
	}
	if (this->gooPrivKey)
	{
		MemFree(this->gooPrivKey);
		this->gooPrivKey = 0;
	}
	this->gooPrivKeyLeng = 0;
	if (gooCliId)
	{
		Crypto::Encrypt::Base64 b64;
		const UTF8Char *tmpKeyStr;
		OSInt tmpKeyStrLeng;

		this->gooCliId = Text::StrCopyNew(gooCliId);
		tmpKeyStr = Text::StrCopyNew(gooPrivKey);
		tmpKeyStrLeng = Text::StrCharCnt(tmpKeyStr);
		this->gooPrivKey = MemAlloc(UInt8, tmpKeyStrLeng + 1);
		Text::StrReplace((Char*)tmpKeyStr, '-', '+');
		Text::StrReplace((Char*)tmpKeyStr, '_', '/');
		this->gooPrivKeyLeng = b64.Decrypt(tmpKeyStr, tmpKeyStrLeng, this->gooPrivKey, 0);
		Text::StrDelNew(tmpKeyStr);
	}
}

void Map::GoogleMap::GoogleWSSearcherJSONDotNet::SetGoogleAPIKey(const UTF8Char *gooAPIKey)
{
	SDEL_TEXT(this->gooAPIKey);
	if (gooAPIKey)
	{
		this->gooAPIKey = Text::StrCopyNew(gooAPIKey);
	}
}

UTF8Char *Map::GoogleMap::GoogleWSSearcherJSONDotNet::SearchName(UTF8Char *buff, Double lat, Double lon, const UTF8Char *lang)
{
	UTF8Char url[1024];
	UTF8Char *urlStart;
	UTF8Char *sptr;
	Data::DateTime currDt;
	Int32 i;
	UInt8 databuff[2048];
	OSInt readSize;

	this->mut->Lock();
	this->srchCnt++;
	currDt.SetCurrTimeUTC();
	this->lastIsError = 0;
	if ((i = (Int32)currDt.DiffMS(this->lastSrchDate)) < 200)
	{
		if (i >= 0)
		{
			Sync::Thread::Sleep(200 - i);
		}
	}

	System::Net::WebRequest ^cli;
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
		sptr = Text::StrConcat(sptr, this->gooCliId);

		UInt8 result[20];
		UInt8 result2[40];
		OSInt size;
		Crypto::Hash::SHA1 sha;
		Crypto::Hash::HMAC hmac(&sha, this->gooPrivKey, this->gooPrivKeyLeng);
		hmac.Calc(urlStart, sptr - urlStart);
		hmac.GetValue(result);
		Crypto::Encrypt::Base64 b64;
		size = b64.Encrypt(result, 20, result2, 0);
		result2[size] = 0;
		Text::StrReplace((Char*)result2, '+', '-');
		Text::StrReplace((Char*)result2, '/', '_');
		sptr = Text::StrConcat(sptr, (const UTF8Char*)"&signature=");
		sptr = Text::StrConcatC(sptr, result2, size);
	}
	else if (this->gooAPIKey)
	{
		sptr = Text::StrConcat(sptr, (const UTF8Char*)"&key=");
		sptr = Text::StrConcat(sptr, this->gooAPIKey);
	}

	try
	{
		cli = System::Net::WebRequest::Create(gcnew System::Uri(gcnew System::String((const char*)url)));
		if (lang)
		{
			cli->Headers->Add(gcnew System::String((const char*)"Accept-Language"), gcnew System::String((const char*)lang));
		}
		System::Net::WebResponse ^resp = cli->GetResponse();
		array<System::Byte> ^readBuff = gcnew array<System::Byte>(2048);
		Int32 status = (Int32)static_cast<System::Net::HttpWebResponse^>(resp)->StatusCode;
		IO::StreamReader *reader;

		IO::MemoryStream *mstm;
		System::IO::Stream ^respStm = resp->GetResponseStream();
		NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"Map.GoogleMap.GoogleWSSearcherJSON.SearchName"));
		while ((readSize = respStm->Read(readBuff, 0, 2048)) > 0)
		{
			System::Runtime::InteropServices::Marshal::Copy(readBuff, 0, System::IntPtr(databuff), readSize);
			mstm->Write(databuff, readSize);
		}
		mstm->SeekFromBeginning(0);
		NEW_CLASS(reader, IO::StreamReader(mstm, 65001));
		Text::StringBuilderUTF8 sb;
		reader->ReadToEnd(&sb);
//		sb.Append(reader);
		DEL_CLASS(reader);
		DEL_CLASS(mstm);
		delete cli;

		if (status == 200)
		{
			Text::JSONBase *obj = Text::JSONBase::ParseJSONStr(sb.ToString());
			if (obj)
			{
				if (obj->GetJSType() == Text::JSONType::Object)
				{
					Text::JSONObject *jobj = (Text::JSONObject*)obj;
					if (jobj->GetObjectValue((const UTF8Char*)"status")->Equals((const UTF8Char*)"OK"))
					{
						OSInt i;
						OSInt j;
						OSInt bestResult = 0;
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
								buff = Text::StrConcat(buff, ((Text::JSONString*)result->GetObjectValue((const UTF8Char*)"formatted_address"))->GetValue());
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
						if (Text::StrCompare(jstr->GetValue(), (const UTF8Char*)"ZERO_RESULTS") == 0)
						{
							buff = Text::StrConcat(buff, (const UTF8Char*)"-");
						}
						else
						{
							sptr = Text::StrConcat(url, (const UTF8Char*)"Google JSON Status ");
							sptr = Text::StrConcat(sptr, jstr->GetValue());
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
				OSInt buffSize;
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
	catch (System::Exception ^)
	{
		Text::StringBuilderUTF8 sb;
		*buff = 0;
		this->lastIsError = 2;
		sb.AppendC(UTF8STRC("Cannot connect: "));
		sb.Append(url);
		errWriter->WriteLine(sb.ToString());
	}

	this->lastSrchDate->SetCurrTimeUTC();
	this->mut->Unlock();
	return buff;
}

UTF8Char *Map::GoogleMap::GoogleWSSearcherJSONDotNet::SearchName(UTF8Char *buff, Double lat, Double lon, Int32 lcid)
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
	return SearchName(buff, lat, lon, ent->shortName);
}

UTF8Char *Map::GoogleMap::GoogleWSSearcherJSONDotNet::CacheName(UTF8Char *buff, Double lat, Double lon, Int32 lcid)
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
	return SearchName(buff, lat, lon, ent->shortName);
}

Int32 Map::GoogleMap::GoogleWSSearcherJSONDotNet::GetSrchCnt()
{
	return this->srchCnt;
}
