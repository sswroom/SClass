#include "Stdafx.h"
#include "IO/Path.h"
#include "Map/OSM/OSMCacheHandler.h"
#include "Net/HTTPClient.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

IO::SeekableStream *Map::OSM::OSMCacheHandler::GetTileData(Int32 lev, Int32 xTile, Int32 yTile)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = Text::StrConcat(sbuff, this->cacheDir);
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
	sptr = Text::StrConcat(sptr, (const UTF8Char*)".png");

	IO::FileStream *fs;
	Data::DateTime dt;
	Data::DateTime currTime;

	NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	if (fs->IsError())
	{
		DEL_CLASS(fs);
		fs = 0;

		const UTF8Char *thisUrl;
		Sync::MutexUsage mutUsage(this->urlMut);
		thisUrl = this->urls->GetItem(this->urlNext);
		this->urlNext = (this->urlNext + 1) % this->urls->GetCount();
		mutUsage.EndUse();
		Text::StringBuilderUTF8 urlSb;
		urlSb.Append(thisUrl);
		urlSb.AppendI32(lev);
		urlSb.Append((const UTF8Char*)"/");
		urlSb.AppendI32(xTile);
		urlSb.Append((const UTF8Char*)"/");
		urlSb.AppendI32(yTile);
		urlSb.Append((const UTF8Char*)".png");

		Net::HTTPClient *cli;
		cli = Net::HTTPClient::CreateClient(this->sockf, this->ssl, (const UTF8Char*)"OSMTileMap/1.0 SSWR/1.0", true, urlSb.StartsWith((const UTF8Char*)"https://"));
		cli->Connect(urlSb.ToString(), "GET", 0, 0, true);

		if (cli->GetRespStatus() == 304)
		{
			NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileStream::FILE_MODE_APPEND, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
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
					NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_READ, IO::FileStream::BT_NO_WRITE_BUFFER));
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

Map::OSM::OSMCacheHandler::OSMCacheHandler(const UTF8Char *url, const UTF8Char *cacheDir, Int32 maxLevel, Net::SocketFactory *sockf, Net::SSLEngine *ssl)
{
	NEW_CLASS(this->urls, Data::ArrayListStrUTF8());
	this->urls->Add(Text::StrCopyNew(url));
	NEW_CLASS(this->urlMut, Sync::Mutex());
	this->urlNext = 0;
	this->cacheDir = Text::StrCopyNew(cacheDir);
	this->maxLevel = maxLevel;
	this->sockf = sockf;
	this->ssl = ssl;
	MemClear(&this->status, sizeof(this->status));
}

Map::OSM::OSMCacheHandler::~OSMCacheHandler()
{
	UOSInt i;
	i = this->urls->GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(this->urls->GetItem(i));
	}
	DEL_CLASS(this->urls);
	DEL_CLASS(this->urlMut);
	if (this->cacheDir)
	{
		Text::StrDelNew(this->cacheDir);
	}
}

void Map::OSM::OSMCacheHandler::AddAlternateURL(const UTF8Char *url)
{
	this->urls->Add(Text::StrCopyNew(url));
}

void Map::OSM::OSMCacheHandler::GetStatus(CacheStatus *status)
{
	MemCopyNO(status, &this->status, sizeof(CacheStatus));
}

Bool Map::OSM::OSMCacheHandler::ProcessRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq)
{
	UTF8Char sbuff[256];
	UTF8Char *sarr[5];
	UOSInt i;
	Text::StrConcatS(sbuff, subReq, 255);
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

	IO::SeekableStream *stm = GetTileData(lev, xTile, yTile);
	if (stm == 0)
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
	}
	else
	{
		UInt8 buff[1024];
		resp->AddDefHeaders(req);
		resp->AddContentType((const UTF8Char*)"image/png");
		resp->AddContentLength(stm->GetLength());
		resp->AddHeader((const UTF8Char*)"Cache-Control", (const UTF8Char*)"private");
		resp->AddHeader((const UTF8Char*)"Access-Control-Allow-Origin", (const UTF8Char*)"*");
		Data::DateTime dt;
		dt.SetCurrTimeUTC();
		dt.AddMinute(1440);
		resp->AddExpireTime(&dt);

		Manage::HiResClock clk;
		while (true)
		{
			i = stm->Read(buff, 1024);
			if (i <= 0)
				break;
			resp->Write(buff, i);
			if (clk.GetTimeDiff() >= 5.0)
			{
				break;
			}
		}
		DEL_CLASS(stm);
	}
	return true;
}
