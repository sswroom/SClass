#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Math/Math.h"
#include "Map/OSM/OSMTileMap.h"
#include "Net/HTTPClient.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"

const UTF8Char *Map::OSM::OSMTileMap::GetNextURL()
{
	Sync::MutexUsage mutUsage(this->urlMut);
	const UTF8Char *thisUrl = this->urls->GetItem(this->urlNext);
	this->urlNext = (this->urlNext + 1) % this->urls->GetCount();
	return thisUrl;
}

Map::OSM::OSMTileMap::OSMTileMap(const UTF8Char *url, const UTF8Char *cacheDir, UOSInt maxLevel, Net::SocketFactory *sockf, Net::SSLEngine *ssl)
{
	NEW_CLASS(this->urls, Data::ArrayListStrUTF8());
	this->urls->Add(Text::StrCopyNew(url));
	NEW_CLASS(this->urlMut, Sync::Mutex());
	this->urlNext = 0;
	this->cacheDir = Text::StrCopyNew(cacheDir);
	this->spkg = 0;
	this->sockf = sockf;
	this->ssl = ssl;
	this->tileWidth = 256;
	this->tileHeight = 256;
	this->maxLevel = maxLevel;
}

Map::OSM::OSMTileMap::OSMTileMap(const UTF8Char *url, IO::SPackageFile *spkg, UOSInt maxLevel, Net::SocketFactory *sockf, Net::SSLEngine *ssl)
{
	NEW_CLASS(this->urls, Data::ArrayListStrUTF8());
	this->urls->Add(Text::StrCopyNew(url));
	NEW_CLASS(this->urlMut, Sync::Mutex());
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
	i = this->urls->GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(this->urls->GetItem(i));
	}
	DEL_CLASS(this->urls);
	DEL_CLASS(this->urlMut);
	SDEL_TEXT(this->cacheDir);
	SDEL_CLASS(this->spkg);
}

void Map::OSM::OSMTileMap::AddAlternateURL(const UTF8Char *url)
{
	this->urls->Add(Text::StrCopyNew(url));
}

const UTF8Char *Map::OSM::OSMTileMap::GetOSMURL(UOSInt index)
{
	return this->urls->GetItem(index);
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

Bool Map::OSM::OSMTileMap::OptimizeToFile(const UTF8Char *fileName)
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

const UTF8Char *Map::OSM::OSMTileMap::GetName()
{
	return (const UTF8Char*)"OSMTileMap";
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
	return 204094080000.0 / Math::UOSInt2Double(this->tileWidth) / (Double)(1 << index);
}

UOSInt Map::OSM::OSMTileMap::GetNearestLevel(Double scale)
{
	Int32 level = Math::Double2Int32(Math::Log10(204094080000.0 / scale / Math::UOSInt2Double(this->tileWidth)) / Math::Log10(2));
	if (level < 0)
		level = 0;
	else if (level >= (Int32)GetLevelCount())
		level = (Int32)GetLevelCount() - 1;
	return (UOSInt)level;
}

UOSInt Map::OSM::OSMTileMap::GetConcurrentCount()
{
	return 2 * this->urls->GetCount();
}

Bool Map::OSM::OSMTileMap::GetBounds(Double *minX, Double *minY, Double *maxX, Double *maxY)
{
	*minX = -180;
	*minY = -85.051128779806592377796715521925;
	*maxX = 180;
	*maxY = 85.051128779806592377796715521925;
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

UOSInt Map::OSM::OSMTileMap::GetImageIDs(UOSInt level, Double x1, Double y1, Double x2, Double y2, Data::ArrayList<Int64> *ids)
{
	Int32 i;
	Int32 j;
	Double max = 85.051128779806592377796715521925;
	if (y1 < -max)
		y1 = -max;
	else if (y1 > max)
		y1 = max;
	if (y2 < -max)
		y2 = -max;
	else if (y2 > max)
		y2 = max;
	
	if (x1 == x2)
		return 0;
	if (y1 == y2)
		return 0;
	Int32 pixX1 = Lon2TileX(x1, level);
	Int32 pixX2 = Lon2TileX(x2, level);
	Int32 pixY1 = Lat2TileY(y1, level);
	Int32 pixY2 = Lat2TileY(y2, level);
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
	UTF8Char u8buff[64];
	Text::StringBuilderUTF8 urlSb;
	UTF8Char *sptru;
	Bool hasTime = false;
	Data::DateTime dt;
	Data::DateTime currTime;
	Net::HTTPClient *cli;
	IO::FileStream *fs;
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
		sptru = Text::StrConcat(filePathU, this->cacheDir);
		if (sptru[-1] != IO::Path::PATH_SEPERATOR)
			*sptru++ = IO::Path::PATH_SEPERATOR;
		sptru = Text::StrUOSInt(sptru, level);
		*sptru++ = IO::Path::PATH_SEPERATOR;
		sptru = Text::StrInt32(sptru, imgX);
		IO::Path::CreateDirectory(filePathU);
		*sptru++ = IO::Path::PATH_SEPERATOR;
		sptru = Text::StrInt32(sptru, imgY);
		sptru = Text::StrConcat(sptru, (const UTF8Char*)".png");
		NEW_CLASS(fd, IO::StmData::FileData(filePathU, false));
		if (fd->GetDataSize() > 0)
		{
			currTime.SetCurrTimeUTC();
			currTime.AddDay(-7);
			((IO::StmData::FileData*)fd)->GetFileStream()->GetFileTimes(&dt, 0, 0);
			if (dt.CompareTo(&currTime) > 0)
			{
				IO::ParsedObject::ParserType pt;
				pobj = parsers->ParseFile(fd, &pt);
				if (pobj)
				{
					if (pt == IO::ParsedObject::PT_IMAGE_LIST_PARSER)
					{
						DEL_CLASS(fd);
						return (Media::ImageList*)pobj;
					}
					DEL_CLASS(pobj);
				}
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
		sptru = Text::StrUOSInt(filePathU, level);
		*sptru++ = '/';
		sptru = Text::StrInt32(sptru, imgX);
		*sptru++ = '/';
		sptru = Text::StrInt32(sptru, imgY);
		sptru = Text::StrConcat(sptru, (const UTF8Char*)".png");
		fd = this->spkg->CreateStreamData(filePathU);
		if (fd)
		{
			IO::ParsedObject::ParserType pt;
			pobj = parsers->ParseFile(fd, &pt);
			if (pobj)
			{
				if (pt == IO::ParsedObject::PT_IMAGE_LIST_PARSER)
				{
					DEL_CLASS(fd);
					return (Media::ImageList*)pobj;
				}
				DEL_CLASS(pobj);
			}
			DEL_CLASS(fd);
//			printf("Get SPKG Img error parsing: %s\r\n", filePathU);
		}
		else
		{
//			printf("Get SPKG Img error: %s\r\n", filePathU);
		}
	}

	if (localOnly)
		return 0;

	const UTF8Char *thisUrl;
	thisUrl = this->GetNextURL();
	urlSb.ClearStr();
	urlSb.Append(thisUrl);
	urlSb.AppendUOSInt(level);
	urlSb.Append((const UTF8Char*)"/");
	urlSb.AppendI32(imgX);
	urlSb.Append((const UTF8Char*)"/");
	urlSb.AppendI32(imgY);
	urlSb.Append((const UTF8Char*)".png");

//	printf("Request URL: %s\r\n", urlSb.ToString());
	cli = Net::HTTPClient::CreateClient(this->sockf, this->ssl, (const UTF8Char*)"OSMTileMap/1.0 SSWR/1.0", true, urlSb.StartsWith((const UTF8Char*)"https://"));
	cli->Connect(urlSb.ToString(), "GET", 0, 0, true);
	if (hasTime)
	{
		Net::HTTPClient::Date2Str(u8buff, &dt);
		cli->AddHeader((const UTF8Char*)"If-Modified-Since", u8buff);
	}
	if (cli->GetRespStatus() == 304)
	{
		NEW_CLASS(fs, IO::FileStream(filePathU, IO::FileStream::FileMode::Append, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		dt.SetCurrTimeUTC();
		fs->SetFileTimes(&dt, 0, 0);
		DEL_CLASS(fs);
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
					NEW_CLASS(fs, IO::FileStream(filePathU, IO::FileStream::FileMode::Create, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer));
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
					DEL_CLASS(fs);
				}
				else if (this->spkg)
				{
					this->spkg->AddFile(imgBuff, (UOSInt)contLeng, filePathU, dt.ToTicks());
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
		NEW_CLASS(fd, IO::StmData::FileData(filePathU, false));
	}
	else if (this->spkg)
	{
		fd = this->spkg->CreateStreamData(filePathU);
	}
	if (fd)
	{
		if (fd->GetDataSize() > 0)
		{
			IO::ParsedObject::ParserType pt;
			pobj = parsers->ParseFile(fd, &pt);
			if (pobj)
			{
				if (pt == IO::ParsedObject::PT_IMAGE_LIST_PARSER)
				{
					DEL_CLASS(fd);
					return (Media::ImageList*)pobj;
				}
				DEL_CLASS(pobj);
			}
		}
		DEL_CLASS(fd);
	}
	return 0;
}

UTF8Char *Map::OSM::OSMTileMap::GetImageURL(UTF8Char *sbuff, UOSInt level, Int64 imgId)
{
	Int32 imgX = (Int32)(imgId >> 32);
	Int32 imgY = (Int32)(imgId & 0xffffffffLL);
	UTF8Char *sptr;
	const UTF8Char *thisUrl = this->GetNextURL();
	sptr = Text::StrConcat(sbuff, thisUrl);
	sptr = Text::StrUOSInt(sptr, level);
	sptr = Text::StrConcat(sptr, (const UTF8Char*)"/");
	sptr = Text::StrInt32(sptr, imgX);
	sptr = Text::StrConcat(sptr, (const UTF8Char*)"/");
	sptr = Text::StrInt32(sptr, imgY);
	sptr = Text::StrConcat(sptr, (const UTF8Char*)".png");
	return sptr;
}

IO::IStreamData *Map::OSM::OSMTileMap::LoadTileImageData(UOSInt level, Int64 imgId, Double *boundsXY, Bool localOnly, Int32 *blockX, Int32 *blockY, ImageType *it)
{
	UOSInt readSize;
	UTF8Char filePathU[512];
	UTF8Char u8buff[64];
	Text::StringBuilderUTF8 urlSb;
	UTF8Char *sptru;
	Bool hasTime = false;
	Data::DateTime dt;
	Data::DateTime currTime;
	Net::HTTPClient *cli;
	IO::FileStream *fs;
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
		sptru = Text::StrConcat(filePathU, this->cacheDir);
		if (sptru[-1] != IO::Path::PATH_SEPERATOR)
			*sptru++ = IO::Path::PATH_SEPERATOR;
		sptru = Text::StrUOSInt(sptru, level);
		*sptru++ = IO::Path::PATH_SEPERATOR;
		sptru = Text::StrInt32(sptru, imgX);
		IO::Path::CreateDirectory(filePathU);
		*sptru++ = IO::Path::PATH_SEPERATOR;
		sptru = Text::StrInt32(sptru, imgY);
		sptru = Text::StrConcat(sptru, (const UTF8Char*)".png");
		NEW_CLASS(fd, IO::StmData::FileData(filePathU, false));
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
		sptru = Text::StrConcat(sptru, (const UTF8Char*)".png");
		fd = this->spkg->CreateStreamData(filePathU);
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

	const UTF8Char *thisUrl = this->GetNextURL();
	urlSb.ClearStr();
	urlSb.Append(thisUrl);
	urlSb.AppendUOSInt(level);
	urlSb.Append((const UTF8Char*)"/");
	urlSb.AppendI32(imgX);
	urlSb.Append((const UTF8Char*)"/");
	urlSb.AppendI32(imgY);
	urlSb.Append((const UTF8Char*)".png");

	cli = Net::HTTPClient::CreateClient(this->sockf, this->ssl, (const UTF8Char*)"OSMTileMap/1.0 SSWR/1.0", true, urlSb.StartsWith((const UTF8Char*)"https://"));
	cli->Connect(urlSb.ToString(), "GET", 0, 0, true);
	if (hasTime)
	{
		Net::HTTPClient::Date2Str(u8buff, &dt);
		cli->AddHeader((const UTF8Char*)"If-Modified-Since", u8buff);
	}
	if (cli->GetRespStatus() == 304)
	{
		NEW_CLASS(fs, IO::FileStream(filePathU, IO::FileStream::FileMode::Append, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		dt.SetCurrTimeUTC();
		fs->SetFileTimes(&dt, 0, 0);
		DEL_CLASS(fs);
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
					NEW_CLASS(fs, IO::FileStream(filePathU, IO::FileStream::FileMode::Create, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer));
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
					DEL_CLASS(fs);
				}
				else if (this->spkg)
				{
					this->spkg->AddFile(imgBuff, (UOSInt)contLeng, filePathU, dt.ToTicks());
				}
			}
			MemFree(imgBuff);
		}

	}
	DEL_CLASS(cli);

	fd = 0;
	if (this->cacheDir)
	{
		NEW_CLASS(fd, IO::StmData::FileData(filePathU, false));
	}
	else if (this->spkg)
	{
		fd = this->spkg->CreateStreamData(filePathU);
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
	return (Int32)((1.0 - Math::Ln( Math::Tan(lat * Math::PI / 180.0) + 1.0 / Math::Cos(lat * Math::PI / 180.0)) / Math::PI) / 2.0 * (1 << level));
}

Int32 Map::OSM::OSMTileMap::Lon2TileXR(Double lon, UOSInt level)
{
	return Math::Double2Int32((lon + 180.0) / 360.0 * (1 << level)); 
}

Int32 Map::OSM::OSMTileMap::Lat2TileYR(Double lat, UOSInt level)
{
	return Math::Double2Int32((1.0 - Math::Ln( Math::Tan(lat * Math::PI / 180.0) + 1.0 / Math::Cos(lat * Math::PI / 180.0)) / Math::PI) / 2.0 * (1 << level));
}

Double Map::OSM::OSMTileMap::TileX2Lon(Int32 x, UOSInt level)
{
	return x * 360.0 / (1 << level) - 180;
}

Double Map::OSM::OSMTileMap::TileY2Lat(Int32 y, UOSInt level)
{
	Double n = Math::PI - 2.0 * Math::PI * y / (1 << level);
	return 180.0 / Math::PI * Math::ArcTan(0.5 * (Math::Exp(n) - Math::Exp(-n)));
}
