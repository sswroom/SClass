#include "Stdafx.h"
#include "IO/Path.h"
#include "Map/OSM/OSMCacheHandler.h"
#include "Net/HTTPClient.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

IO::SeekableStream *Map::OSM::OSMCacheHandler::GetTileData(Int32 lev, Int32 xTile, Int32 yTile, Sync::MutexUsage *mutUsage)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = this->cacheDir->ConcatTo(sbuff);
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
	{
		*sptr++ = IO::Path::PATH_SEPERATOR;
	}
	sptr = Text::StrInt32(sptr, lev);
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrInt32(sptr, xTile);
	IO::Path::CreateDirectory(sbuff);
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrInt32(sptr, yTile);
	sptr = Text::StrConcatC(sptr, UTF8STRC(".png"));

	IO::FileStream *fs;
	Data::DateTime dt;
	Data::DateTime currTime;

	if (this->ioMut)
	{
		mutUsage->ReplaceMutex(this->ioMut);
	}
	NEW_CLASS(fs, IO::FileStream(CSTRP(sbuff, sptr), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (fs->IsError())
	{
		DEL_CLASS(fs);
		fs = 0;
		mutUsage->EndUse();

		Text::String *thisUrl;
		Sync::MutexUsage urlMutUsage(this->urlMut);
		thisUrl = this->urls->GetItem(this->urlNext);
		this->urlNext = (this->urlNext + 1) % this->urls->GetCount();
		urlMutUsage.EndUse();
		Text::StringBuilderUTF8 urlSb;
		urlSb.Append(thisUrl);
		urlSb.AppendI32(lev);
		urlSb.AppendC(UTF8STRC("/"));
		urlSb.AppendI32(xTile);
		urlSb.AppendC(UTF8STRC("/"));
		urlSb.AppendI32(yTile);
		urlSb.AppendC(UTF8STRC(".png"));

		Net::HTTPClient *cli;
		cli = Net::HTTPClient::CreateClient(this->sockf, this->ssl, CSTR("OSMTileMap/1.0 SSWR/1.0"), true, urlSb.StartsWith(UTF8STRC("https://")));
		cli->Connect(urlSb.ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, 0, 0, true);

		if (cli->GetRespStatus() == 304)
		{
			NEW_CLASS(fs, IO::FileStream(CSTRP(sbuff, sptr), IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
			dt.SetCurrTimeUTC();
			fs->SetFileTimes(&dt, 0, 0);
			DEL_CLASS(fs);
			fs = 0;
		}
		else
		{
			UInt64 contLeng = cli->GetContentLength();
			UOSInt currPos = 0;
			UOSInt readSize;
			UInt8 *imgBuff;
			if (contLeng > 0 && contLeng <= 10485760)
			{
				imgBuff = MemAlloc(UInt8, (UOSInt)contLeng);
				while ((readSize = cli->Read(&imgBuff[currPos], (UOSInt)contLeng - currPos)) > 0)
				{
					currPos += readSize;
					if (currPos >= contLeng)
					{
						break;
					}
				}
				if (currPos >= contLeng)
				{
					if (this->ioMut)
					{
						mutUsage->ReplaceMutex(this->ioMut);
					}
					NEW_CLASS(fs, IO::FileStream(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyRead, IO::FileStream::BufferType::NoWriteBuffer));
					fs->Write(imgBuff, (UOSInt)contLeng);
					if (cli->GetLastModified(&dt))
					{
						currTime.SetCurrTimeUTC();
						fs->SetFileTimes(&currTime, 0, &dt);
					}
					else
					{
						currTime.SetCurrTimeUTC();
						fs->SetFileTimes(&currTime, 0, 0);
					}
					fs->SeekFromBeginning(0);
					Sync::Interlocked::Increment(&this->status.remoteSuccCnt);
				}
				else
				{
					Sync::Interlocked::Increment(&this->status.remoteErrCnt);
				}
				MemFree(imgBuff);
			}
			else
			{
				Sync::Interlocked::Increment(&this->status.remoteErrCnt);
			}
		}
		DEL_CLASS(cli);
		return fs;
	}
	else
	{
		Sync::Interlocked::Increment(&this->status.localCnt);
		fs->GetFileTimes(&dt, 0, 0);
		currTime.SetCurrTimeUTC();
		if (currTime.DiffMS(&dt) >= 3600000)
		{
			fs->SetFileTimes(&currTime, 0, 0);
			//////////////////////////
			Sync::Interlocked::Increment(&this->status.cacheCnt);
		}

		return fs;
	}
}

Map::OSM::OSMCacheHandler::OSMCacheHandler(Text::CString url, Text::CString cacheDir, Int32 maxLevel, Net::SocketFactory *sockf, Net::SSLEngine *ssl)
{
	NEW_CLASS(this->urls, Data::ArrayListString());
	this->urls->Add(Text::String::New(url));
	NEW_CLASS(this->urlMut, Sync::Mutex());
	this->urlNext = 0;
	this->ioMut = 0;
	this->cacheDir = Text::String::New(cacheDir);
	this->maxLevel = maxLevel;
	this->sockf = sockf;
	this->ssl = ssl;
	MemClear(&this->status, sizeof(this->status));
}

Map::OSM::OSMCacheHandler::~OSMCacheHandler()
{
	LIST_FREE_STRING(this->urls);
	DEL_CLASS(this->urls);
	DEL_CLASS(this->urlMut);
	SDEL_STRING(this->cacheDir);
}

void Map::OSM::OSMCacheHandler::AddAlternateURL(const UTF8Char *url, UOSInt len)
{
	this->urls->Add(Text::String::New(url, len));
}

void Map::OSM::OSMCacheHandler::GetStatus(CacheStatus *status)
{
	MemCopyNO(status, &this->status, sizeof(CacheStatus));
}

void Map::OSM::OSMCacheHandler::SetIOMut(Sync::Mutex *ioMut)
{
	this->ioMut = ioMut;
}

Bool Map::OSM::OSMCacheHandler::ProcessRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, UOSInt subReqLen)
{
	UTF8Char sbuff[256];
	UTF8Char *sarr[5];
	UOSInt i;
	if (subReqLen > 255)
	{
		subReqLen = 255;
	}
	Text::StrConcatC(sbuff, subReq, subReqLen);
	i = Text::StrSplit(sarr, 5, sbuff, '/');

	if (i != 4)
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
		return true;
	}

	Sync::Interlocked::Increment(&this->status.reqCnt);

	Int32 lev = Text::StrToInt32(sarr[1]);
	Int32 xTile = Text::StrToInt32(sarr[2]);
	Int32 yTile;
	i = Text::StrIndexOf(sarr[3], (const UTF8Char*)".png");
	sarr[3][i] = 0;
	yTile = Text::StrToInt32(sarr[3]);

	Sync::MutexUsage mutUsage;
	IO::SeekableStream *stm = GetTileData(lev, xTile, yTile, &mutUsage);
	if (stm == 0)
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
	}
	else
	{
		UInt64 stmLeng = stm->GetLength();
		resp->AddDefHeaders(req);
		resp->AddContentType(UTF8STRC("image/png"));
		resp->AddContentLength(stmLeng);
		resp->AddHeaderC(UTF8STRC("Cache-Control"), UTF8STRC("private"));
		resp->AddHeaderC(UTF8STRC("Access-Control-Allow-Origin"), UTF8STRC("*"));
		Data::DateTime dt;
		dt.SetCurrTimeUTC();
		dt.AddMinute(1440);
		resp->AddExpireTime(&dt);

		UInt8 *buff = MemAlloc(UInt8, (UOSInt)stmLeng);
		UOSInt buffSize = 0;
		UOSInt readSize;
		while (buffSize < stmLeng)
		{
			readSize = stm->Read(&buff[buffSize], stmLeng - buffSize);
			if (readSize == 0)
			{
				break;
			}
			buffSize += readSize;
		}
		DEL_CLASS(stm);
		mutUsage.EndUse();
		if (buffSize < stmLeng)
		{
			MemClear(&buff[buffSize], (UOSInt)(stmLeng - buffSize));
		}
		buffSize = 0;
		Manage::HiResClock clk;
		while (buffSize < stmLeng)
		{
			readSize = resp->Write(&buff[buffSize], (UOSInt)(stmLeng - buffSize));
			if (readSize == 0)
			{
				break;
			}
			buffSize += readSize;
			if (clk.GetTimeDiff() >= 5.0)
			{
				break;
			}
		}
		MemFree(buff);
	}
	return true;
}
