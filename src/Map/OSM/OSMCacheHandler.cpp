#include "Stdafx.h"
#include "Data/ByteBuffer.h"
#include "IO/Path.h"
#include "Map/OSM/OSMCacheHandler.h"
#include "Net/HTTPClient.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

//#define VERBOSE

#ifdef VERBOSE
#include <stdio.h>
#endif

Optional<IO::SeekableStream> Map::OSM::OSMCacheHandler::GetTileData(Int32 lev, Int32 xTile, Int32 yTile, NN<Sync::MutexUsage> mutUsage)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	sptr = this->cacheDir->ConcatTo(sbuff);
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
	{
		*sptr++ = IO::Path::PATH_SEPERATOR;
	}
	sptr = Text::StrInt32(sptr, lev);
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrInt32(sptr, xTile);
	IO::Path::CreateDirectory(CSTRP(sbuff, sptr));
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrInt32(sptr, yTile);
	sptr = Text::StrConcatC(sptr, UTF8STRC(".png"));

	Data::DateTime dt;
	Data::DateTime currTime;

	NN<Sync::Mutex> ioMut;
	if (this->ioMut.SetTo(ioMut))
	{
		mutUsage->ReplaceMutex(ioMut);
	}

	IO::FileStream *fs;
	NEW_CLASS(fs, IO::FileStream(CSTRP(sbuff, sptr), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (!fs->IsError())
	{
		Sync::Interlocked::IncrementI32(this->status.localCnt);
		fs->GetFileTimes(&dt, 0, 0);
		currTime.SetCurrTimeUTC();
		if (currTime.DiffMS(dt) >= 3600000)
		{
			fs->SetFileTimes(&currTime, 0, 0);
			//////////////////////////
			Sync::Interlocked::IncrementI32(this->status.cacheCnt);
		}

		return fs;
	}
	DEL_CLASS(fs);
	fs = 0;
	mutUsage->EndUse();

	NN<Text::String> thisUrl;
	Sync::MutexUsage urlMutUsage(this->urlMut);
	if (!this->urls.GetItem(this->urlNext).SetTo(thisUrl))
	{
#ifdef VERBOSE
		printf("OSMCacheHandler: Next url is error, urlNext = %d\r\n", (UInt32)this->urlNext);
#endif
		return 0;
	}
	this->urlNext = (this->urlNext + 1) % this->urls.GetCount();
	urlMutUsage.EndUse();
	Text::StringBuilderUTF8 urlSb;
	urlSb.Append(thisUrl);
	urlSb.AppendI32(lev);
	urlSb.AppendC(UTF8STRC("/"));
	urlSb.AppendI32(xTile);
	urlSb.AppendC(UTF8STRC("/"));
	urlSb.AppendI32(yTile);
	urlSb.AppendC(UTF8STRC(".png"));

	NN<Net::HTTPClient> cli;
	cli = Net::HTTPClient::CreateClient(this->clif, this->ssl, CSTR("OSMTileMap/1.0 SSWR/1.0"), true, urlSb.StartsWith(UTF8STRC("https://")));
	cli->Connect(urlSb.ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, 0, 0, true);

	if (cli->GetRespStatus() == 304)
	{
		IO::FileStream imgFS(CSTRP(sbuff, sptr), IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		dt.SetCurrTimeUTC();
		imgFS.SetFileTimes(&dt, 0, 0);
#ifdef VERBOSE
		printf("OSMCacheHandler: Server response 304: %s\r\n", urlSb.v);
#endif
	}
	else
	{
		UInt64 contLeng = cli->GetContentLength();
		UOSInt currPos = 0;
		UOSInt readSize;
		if (contLeng > 0 && contLeng <= 10485760)
		{
			Data::ByteBuffer imgBuff((UOSInt)contLeng);
			while ((readSize = cli->Read(imgBuff.SubArray(currPos, (UOSInt)contLeng - currPos))) > 0)
			{
				currPos += readSize;
				if (currPos >= contLeng)
				{
					break;
				}
			}
			if (currPos >= contLeng)
			{
				NN<Sync::Mutex> ioMut;
				if (this->ioMut.SetTo(ioMut))
				{
					mutUsage->ReplaceMutex(ioMut);
				}
				NEW_CLASS(fs, IO::FileStream(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyRead, IO::FileStream::BufferType::NoWriteBuffer));
				fs->Write(imgBuff);
				if (cli->GetLastModified(dt))
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
				Sync::Interlocked::IncrementI32(this->status.remoteSuccCnt);
			}
			else
			{
#ifdef VERBOSE
				printf("OSMCacheHandler: Read incomplete: %d, %lld\r\n", (UInt32)currPos, contLeng);
#endif
				Sync::Interlocked::IncrementI32(this->status.remoteErrCnt);
			}
		}
		else
		{
#ifdef VERBOSE
			printf("OSMCacheHandler: Content length out of range: %lld, url = %s\r\n", contLeng, urlSb.v);
#endif
			Sync::Interlocked::IncrementI32(this->status.remoteErrCnt);
		}
	}
	cli.Delete();
	return fs;
}

Map::OSM::OSMCacheHandler::OSMCacheHandler(Text::CString url, Text::CStringNN cacheDir, Int32 maxLevel, NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl)
{
	Text::CStringNN nnurl;
	if (url.SetTo(nnurl) && nnurl.leng > 0)
	{
		this->urls.Add(Text::String::New(nnurl));
	}
	this->urlNext = 0;
	this->ioMut = 0;
	this->cacheDir = Text::String::New(cacheDir);
	this->maxLevel = maxLevel;
	this->clif = clif;
	this->ssl = ssl;
	MemClear(&this->status, sizeof(this->status));
}

Map::OSM::OSMCacheHandler::~OSMCacheHandler()
{
	this->urls.FreeAll();
	this->cacheDir->Release();
}

void Map::OSM::OSMCacheHandler::AddAlternateURL(Text::CStringNN url)
{
	this->urls.Add(Text::String::New(url));
}

void Map::OSM::OSMCacheHandler::GetStatus(NN<CacheStatus> status)
{
	MemCopyNO(status.Ptr(), &this->status, sizeof(CacheStatus));
}

void Map::OSM::OSMCacheHandler::SetIOMut(Optional<Sync::Mutex> ioMut)
{
	this->ioMut = ioMut;
}

Bool Map::OSM::OSMCacheHandler::ProcessRequest(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sarr[5];
	UOSInt i;
	if (subReq.leng > 255)
	{
		subReq.leng = 255;
	}
	subReq.ConcatTo(sbuff);
	i = Text::StrSplit(sarr, 5, sbuff, '/');

	if (i != 4)
	{
#ifdef VERBOSE
		printf("OSMCacheHandler: Split not = 4 (%d)\r\n", (UInt32)i);
#endif
		resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
		return true;
	}

	Sync::Interlocked::IncrementI32(this->status.reqCnt);

	Int32 lev = Text::StrToInt32(sarr[1]);
	Int32 xTile = Text::StrToInt32(sarr[2]);
	Int32 yTile;
	i = Text::StrIndexOf(sarr[3], (const UTF8Char*)".png");
	sarr[3][i] = 0;
	yTile = Text::StrToInt32(sarr[3]);

	Sync::MutexUsage mutUsage;
	NN<IO::SeekableStream> stm;
	if (!GetTileData(lev, xTile, yTile, mutUsage).SetTo(stm))
	{
#ifdef VERBOSE
		printf("OSMCacheHandler: Get Tile Data failed, lev = %d, xTile = %d, yTile = %d\r\n", lev, xTile, yTile);
#endif
		resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
	}
	else
	{
		UInt64 stmLeng = stm->GetLength();
		resp->AddDefHeaders(req);
		resp->AddContentType(CSTR("image/png"));
		resp->AddContentLength(stmLeng);
		resp->AddHeader(CSTR("Cache-Control"), CSTR("private"));
		resp->AddHeader(CSTR("Access-Control-Allow-Origin"), CSTR("*"));
		Data::DateTime dt;
		dt.SetCurrTimeUTC();
		dt.AddMinute(1440);
		resp->AddExpireTime(&dt);

		Data::ByteBuffer buff((UOSInt)stmLeng);
		UOSInt buffSize = 0;
		UOSInt readSize;
		while (buffSize < stmLeng)
		{
			readSize = stm->Read(buff.SubArray(buffSize, (UOSInt)(stmLeng - buffSize)));
			if (readSize == 0)
			{
				break;
			}
			buffSize += readSize;
		}
		stm.Delete();
		mutUsage.EndUse();
		if (buffSize < stmLeng)
		{
			MemClear(&buff[buffSize], (UOSInt)(stmLeng - buffSize));
		}
		buffSize = 0;
		Manage::HiResClock clk;
		while (buffSize < stmLeng)
		{
			readSize = resp->Write(Data::ByteArrayR(&buff[buffSize], (UOSInt)(stmLeng - buffSize)));
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
	}
	return true;
}
