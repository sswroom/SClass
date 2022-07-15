#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "IO/Path.h"
#include "IO/StmData/BufferedStreamData.h"
#include "IO/StmData/FileData.h"
#include "Math/Math.h"
#include "Map/OSM/OSMTileMap.h"
#include "Net/HTTPClient.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"

Text::String *Map::OSM::OSMTileMap::GetNextURL()
{
	Sync::MutexUsage mutUsage(&this->urlMut);
	Text::String *thisUrl = this->urls.GetItem(this->urlNext);
	this->urlNext = (this->urlNext + 1) % this->urls.GetCount();
	return thisUrl;
}

Map::OSM::OSMTileMap::OSMTileMap(Text::CString url, Text::CString cacheDir, UOSInt maxLevel, Net::SocketFactory *sockf, Net::SSLEngine *ssl)
{
	this->urls.Add(Text::String::New(url.v, url.leng));
	this->urlNext = 0;
	this->cacheDir = Text::String::New(cacheDir.v, cacheDir.leng);
	this->spkg = 0;
	this->sockf = sockf;
	this->ssl = ssl;
	this->tileWidth = 256;
	this->tileHeight = 256;
	this->maxLevel = maxLevel;
}

Map::OSM::OSMTileMap::OSMTileMap(Text::CString url, IO::SPackageFile *spkg, UOSInt maxLevel, Net::SocketFactory *sockf, Net::SSLEngine *ssl)
{
	this->urls.Add(Text::String::New(url.v, url.leng));
	this->urlNext = 0;
	this->cacheDir = 0;
	this->spkg = spkg;
	this->sockf = sockf;
	this->ssl = ssl;
	this->tileWidth = 256;
	this->tileHeight = 256;
	this->maxLevel = maxLevel;
}

Map::OSM::OSMTileMap::~OSMTileMap()
{
	UOSInt i;
	i = this->urls.GetCount();
	while (i-- > 0)
	{
		this->urls.GetItem(i)->Release();
	}
	SDEL_STRING(this->cacheDir);
	SDEL_CLASS(this->spkg);
}

void Map::OSM::OSMTileMap::AddAlternateURL(Text::CString url)
{
	this->urls.Add(Text::String::New(url.v, url.leng));
}

Text::String *Map::OSM::OSMTileMap::GetOSMURL(UOSInt index)
{
	return this->urls.GetItem(index);
}

Bool Map::OSM::OSMTileMap::HasSPackageFile()
{
	return this->spkg != 0;
}

Bool Map::OSM::OSMTileMap::ImportTiles(IO::PackageFile *pkg)
{
	if (this->spkg)
	{
		return this->spkg->AddPackage(pkg, '/');
	}
	else
	{
		return false;
	}
}

Bool Map::OSM::OSMTileMap::OptimizeToFile(Text::CString fileName)
{
	if (this->spkg)
	{
		return this->spkg->OptimizeFile(fileName);
	}
	else
	{
		return false;
	}
}

Text::CString Map::OSM::OSMTileMap::GetName()
{
	return CSTR("OSMTileMap");
}

Bool Map::OSM::OSMTileMap::IsError()
{
	return false;
}

Map::TileMap::TileType Map::OSM::OSMTileMap::GetTileType()
{
	return Map::TileMap::TT_OSM;
}

UOSInt Map::OSM::OSMTileMap::GetLevelCount()
{
	return this->maxLevel + 1;
}


Double Map::OSM::OSMTileMap::GetLevelScale(UOSInt index)
{
	return 204094080000.0 / UOSInt2Double(this->tileWidth) / (Double)(1 << index);
}

UOSInt Map::OSM::OSMTileMap::GetNearestLevel(Double scale)
{
	Int32 level = Double2Int32(Math_Log10(204094080000.0 / scale / UOSInt2Double(this->tileWidth)) / Math_Log10(2));
	if (level < 0)
		level = 0;
	else if (level >= (Int32)GetLevelCount())
		level = (Int32)GetLevelCount() - 1;
	return (UOSInt)level;
}

UOSInt Map::OSM::OSMTileMap::GetConcurrentCount()
{
	return 2 * this->urls.GetCount();
}

Bool Map::OSM::OSMTileMap::GetBounds(Math::RectAreaDbl *bounds)
{
	*bounds = Math::RectAreaDbl(Math::Coord2DDbl(-180, -85.051128779806592377796715521925),
		Math::Coord2DDbl(180, 85.051128779806592377796715521925));
	return true;
}

Map::TileMap::ProjectionType Map::OSM::OSMTileMap::GetProjectionType()
{
	return Map::TileMap::PT_MERCATOR;
}

UOSInt Map::OSM::OSMTileMap::GetTileSize()
{
	return this->tileWidth;
}

UOSInt Map::OSM::OSMTileMap::GetImageIDs(UOSInt level, Math::RectAreaDbl rect, Data::ArrayList<Int64> *ids)
{
	Int32 i;
	Int32 j;
	Double max = 85.051128779806592377796715521925;
	if (rect.tl.y < -max)
		rect.tl.y = -max;
	else if (rect.tl.y > max)
		rect.tl.y = max;
	if (rect.br.y < -max)
		rect.br.y = -max;
	else if (rect.br.y > max)
		rect.br.y = max;
	
	if (rect.tl.x == rect.br.x)
		return 0;
	if (rect.tl.y == rect.br.y)
		return 0;
	Int32 pixX1 = Lon2TileX(rect.tl.x, level);
	Int32 pixX2 = Lon2TileX(rect.br.x, level);
	Int32 pixY1 = Lat2TileY(rect.tl.y, level);
	Int32 pixY2 = Lat2TileY(rect.br.y, level);
	if (pixX1 > pixX2)
	{
		i = pixX1;
		pixX1 = pixX2;
		pixX2 = i;
	}
	if (pixY1 > pixY2)
	{
		i = pixY1;
		pixY1 = pixY2;
		pixY2 = i;
	}
	if (pixX1 < 0)
		pixX1 = 0;
	if (pixY1 < 0)
		pixY1 = 0;
	if (pixX2 < 0)
		pixX2 = 0;
	if (pixY2 < 0)
		pixY2 = 0;
	if (pixX2 >= (1 << level))
	{
		pixX2 = (1 << level) - 1;
	}
	i = pixY1;
	while (i <= pixY2)
	{
		j = pixX1;
		while (j <= pixX2)
		{
			ids->Add((((Int64)(UInt32)j) << 32) | (UInt32)i);
			j++;
		}
		i++;
	}
	return (UOSInt)((pixX2 - pixX1 + 1) * (pixY2 - pixY1 + 1));
}

Media::ImageList *Map::OSM::OSMTileMap::LoadTileImage(UOSInt level, Int64 imgId, Parser::ParserList *parsers, Double *boundsXY, Bool localOnly)
{
	UOSInt readSize;
	//WChar filePath[512];
	UTF8Char filePathU[512];
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	Text::StringBuilderUTF8 urlSb;
	UTF8Char *sptru = filePathU;
	Bool hasTime = false;
	Data::DateTime dt;
	Data::DateTime currTime;
	Net::HTTPClient *cli;
	IO::IStreamData *fd;
	IO::ParsedObject *pobj;
	if (level > this->maxLevel)
		return 0;
	Int32 imgX = (Int32)(imgId >> 32);
	Int32 imgY = (Int32)(imgId & 0xffffffffLL);
	Double x1 = TileX2Lon(imgX, level);
	Double y1 = TileY2Lat(imgY, level);
	Double x2 = TileX2Lon(imgX + 1, level);
	Double y2 = TileY2Lat(imgY + 1, level);

	boundsXY[0] = x1;
	boundsXY[1] = y1;
	boundsXY[2] = x2;
	boundsXY[3] = y2;
	if (x1 > 180 || y1 < -90)
		return 0;

	if (this->cacheDir)
	{
		sptru = this->cacheDir->ConcatTo(filePathU);
		if (sptru[-1] != IO::Path::PATH_SEPERATOR)
			*sptru++ = IO::Path::PATH_SEPERATOR;
		sptru = Text::StrUOSInt(sptru, level);
		*sptru++ = IO::Path::PATH_SEPERATOR;
		sptru = Text::StrInt32(sptru, imgX);
		IO::Path::CreateDirectory(CSTRP(filePathU, sptru));
		*sptru++ = IO::Path::PATH_SEPERATOR;
		sptru = Text::StrInt32(sptru, imgY);
		sptru = Text::StrConcatC(sptru, UTF8STRC(".png"));
		NEW_CLASS(fd, IO::StmData::FileData({filePathU, (UOSInt)(sptru - filePathU)}, false));
		if (fd->GetDataSize() > 0)
		{
			currTime.SetCurrTimeUTC();
			currTime.AddDay(-7);
			((IO::StmData::FileData*)fd)->GetFileStream()->GetFileTimes(&dt, 0, 0);
			if (dt.CompareTo(&currTime) > 0)
			{
				IO::ParserType pt;
				IO::StmData::BufferedStreamData bsd(fd);
				pobj = parsers->ParseFile(&bsd, &pt);
				if (pobj)
				{
					if (pt == IO::ParserType::ImageList)
					{
						return (Media::ImageList*)pobj;
					}
					DEL_CLASS(pobj);
				}
			}
			else
			{
				hasTime = true;
				DEL_CLASS(fd);
			}
		}
		else
		{
			DEL_CLASS(fd);
		}
	}
	else if (this->spkg)
	{
		sptru = Text::StrUOSInt(filePathU, level);
		*sptru++ = '/';
		sptru = Text::StrInt32(sptru, imgX);
		*sptru++ = '/';
		sptru = Text::StrInt32(sptru, imgY);
		sptru = Text::StrConcatC(sptru, UTF8STRC(".png"));
		fd = this->spkg->CreateStreamData({filePathU, (UOSInt)(sptru - filePathU)});
		if (fd)
		{
			IO::ParserType pt;
			IO::StmData::BufferedStreamData bsd(fd);
			pobj = parsers->ParseFile(&bsd, &pt);
			if (pobj)
			{
				if (pt == IO::ParserType::ImageList)
				{
					return (Media::ImageList*)pobj;
				}
				DEL_CLASS(pobj);
			}
//			printf("Get SPKG Img error parsing: %s\r\n", filePathU);
		}
		else
		{
//			printf("Get SPKG Img error: %s\r\n", filePathU);
		}
	}

	if (localOnly)
		return 0;

	Text::String *thisUrl;
	thisUrl = this->GetNextURL();
	urlSb.ClearStr();
	urlSb.Append(thisUrl);
	urlSb.AppendUOSInt(level);
	urlSb.AppendC(UTF8STRC("/"));
	urlSb.AppendI32(imgX);
	urlSb.AppendC(UTF8STRC("/"));
	urlSb.AppendI32(imgY);
	urlSb.AppendC(UTF8STRC(".png"));

//	printf("Request URL: %s\r\n", urlSb.ToString());
	cli = Net::HTTPClient::CreateClient(this->sockf, this->ssl, CSTR("OSMTileMap/1.0 SSWR/1.0"), true, urlSb.StartsWith(UTF8STRC("https://")));
	cli->Connect(urlSb.ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, 0, 0, true);
	cli->SetTimeout(5000);
	if (hasTime)
	{
		sptr = Net::WebUtil::Date2Str(sbuff, &dt);
		cli->AddHeaderC(CSTR("If-Modified-Since"), {sbuff, (UOSInt)(sptr - sbuff)});
	}
	if (cli->GetRespStatus() == 304)
	{
		IO::FileStream fs({filePathU, (UOSInt)(sptru - filePathU)}, IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		dt.SetCurrTimeUTC();
		fs.SetFileTimes(&dt, 0, 0);
//		printf("Reply 304\r\n");
	}
	else if (!cli->IsError())
	{
		UInt64 contLeng = cli->GetContentLength();
		UOSInt currPos = 0;
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
//			printf("Reply size = %d, leng = %d\r\n", (Int32)currPos, (Int32)contLeng);
			if (currPos >= contLeng)
			{
				if (this->cacheDir)
				{
					IO::FileStream fs({filePathU, (UOSInt)(sptru - filePathU)}, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer);
					fs.Write(imgBuff, (UOSInt)contLeng);
					if (cli->GetLastModified(&dt))
					{
						currTime.SetCurrTimeUTC();
						fs.SetFileTimes(&currTime, 0, &dt);
					}
					else
					{
						currTime.SetCurrTimeUTC();
						fs.SetFileTimes(&currTime, 0, 0);
					}
				}
				else if (this->spkg)
				{
					this->spkg->AddFile(imgBuff, (UOSInt)contLeng, {filePathU, (UOSInt)(sptru - filePathU)}, dt.ToTicks());
//					printf("Add File: %d, %d, %s\r\n", ret, (Int32)contLeng, filePathU);
				}
			}
			MemFree(imgBuff);
		}

	}
	DEL_CLASS(cli);

	fd = 0;
	if (this->cacheDir)
	{
		NEW_CLASS(fd, IO::StmData::FileData({filePathU, (UOSInt)(sptru - filePathU)}, false));
	}
	else if (this->spkg)
	{
		fd = this->spkg->CreateStreamData({filePathU, (UOSInt)(sptru - filePathU)});
	}
	if (fd)
	{
		if (fd->GetDataSize() > 0)
		{
			IO::ParserType pt;
			IO::StmData::BufferedStreamData bsd(fd);
			pobj = parsers->ParseFile(&bsd, &pt);
			if (pobj)
			{
				if (pt == IO::ParserType::ImageList)
				{
					return (Media::ImageList*)pobj;
				}
				DEL_CLASS(pobj);
			}
		}
		else
		{
			DEL_CLASS(fd);
		}
	}
	return 0;
}

UTF8Char *Map::OSM::OSMTileMap::GetImageURL(UTF8Char *sbuff, UOSInt level, Int64 imgId)
{
	Int32 imgX = (Int32)(imgId >> 32);
	Int32 imgY = (Int32)(imgId & 0xffffffffLL);
	UTF8Char *sptr;
	Text::String *thisUrl = this->GetNextURL();
	sptr = thisUrl->ConcatTo(sbuff);
	sptr = Text::StrUOSInt(sptr, level);
	sptr = Text::StrConcatC(sptr, UTF8STRC("/"));
	sptr = Text::StrInt32(sptr, imgX);
	sptr = Text::StrConcatC(sptr, UTF8STRC("/"));
	sptr = Text::StrInt32(sptr, imgY);
	sptr = Text::StrConcatC(sptr, UTF8STRC(".png"));
	return sptr;
}

IO::IStreamData *Map::OSM::OSMTileMap::LoadTileImageData(UOSInt level, Int64 imgId, Double *boundsXY, Bool localOnly, Int32 *blockX, Int32 *blockY, ImageType *it)
{
	UOSInt readSize;
	UTF8Char filePathU[512];
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	Text::StringBuilderUTF8 urlSb;
	UTF8Char *sptru = filePathU;
	Bool hasTime = false;
	Data::DateTime dt;
	Data::DateTime currTime;
	Net::HTTPClient *cli;
	IO::IStreamData *fd;
	if (level > this->maxLevel)
		return 0;
	Int32 imgX = (Int32)(imgId >> 32);
	Int32 imgY = (Int32)(imgId & 0xffffffffLL);
	Double x1 = TileX2Lon(imgX, level);
	Double y1 = TileY2Lat(imgY, level);
	Double x2 = TileX2Lon(imgX + 1, level);
	Double y2 = TileY2Lat(imgY + 1, level);

	boundsXY[0] = x1;
	boundsXY[1] = y1;
	boundsXY[2] = x2;
	boundsXY[3] = y2;
	if (x1 > 180 || y1 < -90)
		return 0;

	if (this->cacheDir)
	{
		sptru = this->cacheDir->ConcatTo(filePathU);
		if (sptru[-1] != IO::Path::PATH_SEPERATOR)
			*sptru++ = IO::Path::PATH_SEPERATOR;
		sptru = Text::StrUOSInt(sptru, level);
		*sptru++ = IO::Path::PATH_SEPERATOR;
		sptru = Text::StrInt32(sptru, imgX);
		IO::Path::CreateDirectory(CSTRP(filePathU, sptru));
		*sptru++ = IO::Path::PATH_SEPERATOR;
		sptru = Text::StrInt32(sptru, imgY);
		sptru = Text::StrConcatC(sptru, UTF8STRC(".png"));
		NEW_CLASS(fd, IO::StmData::FileData({filePathU, (UOSInt)(sptru - filePathU)}, false));
		if (fd->GetDataSize() > 0)
		{
			currTime.SetCurrTimeUTC();
			currTime.AddDay(-7);
			((IO::StmData::FileData*)fd)->GetFileStream()->GetFileTimes(&dt, 0, 0);
			if (dt.CompareTo(&currTime) > 0)
			{
				if (blockX)
					*blockX = imgX;
				if (blockY)
					*blockY = imgY;
				if (it)
					*it = IT_PNG;
				return fd;
			}
			else
			{
				hasTime = true;
			}
		}
		DEL_CLASS(fd);
	}
	else if (this->spkg)
	{
		sptru = Text::StrInt32(filePathU, (Int32)level);
		*sptru++ = '/';
		sptru = Text::StrInt32(sptru, imgX);
		*sptru++ = '/';
		sptru = Text::StrInt32(sptru, imgY);
		sptru = Text::StrConcatC(sptru, UTF8STRC(".png"));
		fd = this->spkg->CreateStreamData({filePathU, (UOSInt)(sptru - filePathU)});
		if (fd)
		{
			if (blockX)
				*blockX = imgX;
			if (blockY)
				*blockY = imgY;
			if (it)
				*it = IT_PNG;
			return fd;
		}
		DEL_CLASS(fd);
	}

	if (localOnly)
		return 0;

	Text::String *thisUrl = this->GetNextURL();
	urlSb.ClearStr();
	urlSb.Append(thisUrl);
	urlSb.AppendUOSInt(level);
	urlSb.AppendC(UTF8STRC("/"));
	urlSb.AppendI32(imgX);
	urlSb.AppendC(UTF8STRC("/"));
	urlSb.AppendI32(imgY);
	urlSb.AppendC(UTF8STRC(".png"));

	cli = Net::HTTPClient::CreateClient(this->sockf, this->ssl, CSTR("OSMTileMap/1.0 SSWR/1.0"), true, urlSb.StartsWith(UTF8STRC("https://")));
	cli->Connect(urlSb.ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, 0, 0, true);
	if (hasTime)
	{
		sptr = Net::WebUtil::Date2Str(sbuff, &dt);
		cli->AddHeaderC(CSTR("If-Modified-Since"), CSTRP(sbuff, sptr));
	}
	if (cli->GetRespStatus() == 304)
	{
		IO::FileStream fs({filePathU, (UOSInt)(sptru - filePathU)}, IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		dt.SetCurrTimeUTC();
		fs.SetFileTimes(&dt, 0, 0);
	}
	else
	{
		UInt64 contLeng = cli->GetContentLength();
		UOSInt currPos = 0;
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
				if (this->cacheDir)
				{
					IO::FileStream fs({filePathU, (UOSInt)(sptru - filePathU)}, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer);
					fs.Write(imgBuff, (UOSInt)contLeng);
					if (cli->GetLastModified(&dt))
					{
						currTime.SetCurrTimeUTC();
						fs.SetFileTimes(&currTime, 0, &dt);
					}
					else
					{
						currTime.SetCurrTimeUTC();
						fs.SetFileTimes(&currTime, 0, 0);
					}
				}
				else if (this->spkg)
				{
					this->spkg->AddFile(imgBuff, (UOSInt)contLeng, {filePathU, (UOSInt)(sptru - filePathU)}, dt.ToTicks());
				}
			}
			MemFree(imgBuff);
		}

	}
	DEL_CLASS(cli);

	fd = 0;
	if (this->cacheDir)
	{
		NEW_CLASS(fd, IO::StmData::FileData({filePathU, (UOSInt)(sptru - filePathU)}, false));
	}
	else if (this->spkg)
	{
		fd = this->spkg->CreateStreamData({filePathU, (UOSInt)(sptru - filePathU)});
	}
	if (fd)
	{
		if (fd->GetDataSize() > 0)
		{
			if (blockX)
				*blockX = imgX;
			if (blockY)
				*blockY = imgY;
			if (it)
				*it = IT_PNG;
			return fd;
		}
		DEL_CLASS(fd);
	}
	return 0;
}

Int32 Map::OSM::OSMTileMap::Lon2TileX(Double lon, UOSInt level)
{
	return (Int32)((lon + 180.0) / 360.0 * (1 << level)); 
}

Int32 Map::OSM::OSMTileMap::Lat2TileY(Double lat, UOSInt level)
{
	return (Int32)((1.0 - Math_Ln( Math_Tan(lat * Math::PI / 180.0) + 1.0 / Math_Cos(lat * Math::PI / 180.0)) / Math::PI) / 2.0 * (1 << level));
}

Int32 Map::OSM::OSMTileMap::Lon2TileXR(Double lon, UOSInt level)
{
	return Double2Int32((lon + 180.0) / 360.0 * (1 << level)); 
}

Int32 Map::OSM::OSMTileMap::Lat2TileYR(Double lat, UOSInt level)
{
	return Double2Int32((1.0 - Math_Ln( Math_Tan(lat * Math::PI / 180.0) + 1.0 / Math_Cos(lat * Math::PI / 180.0)) / Math::PI) / 2.0 * (1 << level));
}

Double Map::OSM::OSMTileMap::TileX2Lon(Int32 x, UOSInt level)
{
	return x * 360.0 / (1 << level) - 180;
}

Double Map::OSM::OSMTileMap::TileY2Lat(Int32 y, UOSInt level)
{
	Double n = Math::PI - 2.0 * Math::PI * y / (1 << level);
	return 180.0 / Math::PI * Math_ArcTan(0.5 * (Math_Exp(n) - Math_Exp(-n)));
}
