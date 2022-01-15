#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Math/Math.h"
#include "Map/GoogleMap/GoogleTileMap.h"
#include "Map/OSM/OSMTileMap.h"
#include "Net/HTTPClient.h"
#include "Text/MyString.h"

#define GMAPURL "http://mt1.google.com/vt/"

Map::GoogleMap::GoogleTileMap::GoogleTileMap(const UTF8Char *cacheDir, MapType mapType, Net::SocketFactory *sockf, Net::SSLEngine *ssl)
{
	this->cacheDir = Text::StrCopyNew(cacheDir);
	this->spkg = 0;
	this->sockf = sockf;
	this->ssl = ssl;
	this->tileWidth = 256;
	this->tileHeight = 256;
	this->maxLevel = 18;
	this->mapType = mapType;
}

Map::GoogleMap::GoogleTileMap::~GoogleTileMap()
{
	SDEL_TEXT(this->cacheDir);
	SDEL_CLASS(this->spkg);
}

Bool Map::GoogleMap::GoogleTileMap::HasSPackageFile()
{
	return this->spkg != 0;
}

Bool Map::GoogleMap::GoogleTileMap::ImportTiles(IO::PackageFile *pkg)
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

Bool Map::GoogleMap::GoogleTileMap::OptimizeToFile(const UTF8Char *fileName)
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

const UTF8Char *Map::GoogleMap::GoogleTileMap::GetName()
{
	return (const UTF8Char*)"GoogleTileMap";
}

Bool Map::GoogleMap::GoogleTileMap::IsError()
{
	return false;
}

Map::TileMap::TileType Map::GoogleMap::GoogleTileMap::GetTileType()
{
	return Map::TileMap::TT_GOOGLE;
}

UOSInt Map::GoogleMap::GoogleTileMap::GetLevelCount()
{
	return this->maxLevel + 1;
}


Double Map::GoogleMap::GoogleTileMap::GetLevelScale(OSInt index)
{
	return 204094080000.0 / this->tileWidth / (1 << index);
}

UOSInt Map::GoogleMap::GoogleTileMap::GetNearestLevel(Double scale)
{
	Int32 level = Math::Double2Int32(Math_Log10(204094080000.0 / scale / this->tileWidth) / Math_Log10(2));
	if (level < 0)
		level = 0;
	else if ((UOSInt)level >= GetLevelCount())
		level = (Int32)GetLevelCount() - 1;
	return level;
}

UOSInt Map::GoogleMap::GoogleTileMap::GetConcurrentCount()
{
	return 2;
}

Bool Map::GoogleMap::GoogleTileMap::GetBounds(Double *minX, Double *minY, Double *maxX, Double *maxY)
{
	*minX = -180;
	*minY = -85.051128779806592377796715521925;
	*maxX = 180;
	*maxY = 85.051128779806592377796715521925;
	return true;
}

Map::TileMap::ProjectionType Map::GoogleMap::GoogleTileMap::GetProjectionType()
{
	return Map::TileMap::PT_MERCATOR;
}

UOSInt Map::GoogleMap::GoogleTileMap::GetTileSize()
{
	return this->tileWidth;
}

UOSInt Map::GoogleMap::GoogleTileMap::GetImageIDs(UOSInt level, Double x1, Double y1, Double x2, Double y2, Data::ArrayList<Int64> *ids)
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
	Int32 pixX1 = Map::OSM::OSMTileMap::Lon2TileX(x1, level);
	Int32 pixX2 = Map::OSM::OSMTileMap::Lon2TileX(x2, level);
	Int32 pixY1 = Map::OSM::OSMTileMap::Lat2TileY(y1, level);
	Int32 pixY2 = Map::OSM::OSMTileMap::Lat2TileY(y2, level);
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
	return (pixX2 - pixX1 + 1) * (pixY2 - pixY1 + 1);
}

Media::ImageList *Map::GoogleMap::GoogleTileMap::LoadTileImage(UOSInt level, Int64 imgId, Parser::ParserList *parsers, Double *boundsXY, Bool localOnly)
{
	OSInt readSize;
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
	Double x1 = Map::OSM::OSMTileMap::TileX2Lon(imgX, level);
	Double y1 = Map::OSM::OSMTileMap::TileY2Lat(imgY, level);
	Double x2 = Map::OSM::OSMTileMap::TileX2Lon(imgX + 1, level);
	Double y2 = Map::OSM::OSMTileMap::TileY2Lat(imgY + 1, level);

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
		sptru = Text::StrConcatC(sptru, UTF8STRC(".png"));
		NEW_CLASS(fd, IO::StmData::FileData(filePathU, false));
		if (fd->GetDataSize() > 0)
		{
			currTime.SetCurrTimeUTC();
			currTime.AddDay(-7);
			((IO::StmData::FileData*)fd)->GetFileStream()->GetFileTimes(&dt, 0, 0);
			if (dt.CompareTo(&currTime) > 0)
			{
				IO::ParserType pt;
				pobj = parsers->ParseFile(fd, &pt);
				if (pobj)
				{
					if (pt == IO::ParserType::ImageList)
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
		sptru = Text::StrInt32(filePathU, (Int32)level);
		*sptru++ = '/';
		sptru = Text::StrInt32(sptru, imgX);
		*sptru++ = '/';
		sptru = Text::StrInt32(sptru, imgY);
		sptru = Text::StrConcatC(sptru, UTF8STRC(".png"));
		fd = this->spkg->CreateStreamData(filePathU);
		if (fd)
		{
			IO::ParserType pt;
			pobj = parsers->ParseFile(fd, &pt);
			if (pobj)
			{
				if (pt == IO::ParserType::ImageList)
				{
					DEL_CLASS(fd);
					return (Media::ImageList*)pobj;
				}
				DEL_CLASS(pobj);
			}
			DEL_CLASS(fd);
		}
	}

	if (localOnly)
		return 0;

	urlSb.ClearStr();
	urlSb.AppendC(UTF8STRC(GMAPURL));
	urlSb.AppendC(UTF8STRC("lyrs="));
	urlSb.AppendChar((UTF32Char)this->mapType, 1);
	urlSb.AppendC(UTF8STRC("&x="));
	urlSb.AppendI32(imgX);
	urlSb.AppendC(UTF8STRC("&y="));
	urlSb.AppendI32(imgY);
	urlSb.AppendC(UTF8STRC("&z="));
	urlSb.AppendUOSInt(level);

	cli = Net::HTTPClient::CreateClient(this->sockf, this->ssl, (const UTF8Char*)"GoogleTileMap/1.0 SSWR/1.0", true, urlSb.StartsWith((const UTF8Char*)"https://"));
	cli->Connect(urlSb.ToString(), "GET", 0, 0, true);
	if (hasTime)
	{
		Net::HTTPClient::Date2Str(u8buff, &dt);
		cli->AddHeader((const UTF8Char*)"If-Modified-Since", u8buff);
	}
	if (cli->GetRespStatus() == 304)
	{
		NEW_CLASS(fs, IO::FileStream(filePathU, IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		dt.SetCurrTimeUTC();
		fs->SetFileTimes(&dt, 0, 0);
		DEL_CLASS(fs);
	}
	else if (!cli->IsError())
	{
		Int64 contLeng = cli->GetContentLength();
		OSInt currPos = 0;
		UInt8 *imgBuff;
		if (contLeng > 0 && contLeng <= 10485760)
		{
			imgBuff = MemAlloc(UInt8, (OSInt)contLeng);
			while ((readSize = cli->Read(&imgBuff[currPos], (OSInt)contLeng - currPos)) > 0)
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
					NEW_CLASS(fs, IO::FileStream(filePathU, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer));
					fs->Write(imgBuff, (OSInt)contLeng);
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
					this->spkg->AddFile(imgBuff, (OSInt)contLeng, filePathU, dt.ToTicks());
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
			IO::ParserType pt;
			pobj = parsers->ParseFile(fd, &pt);
			if (pobj)
			{
				if (pt == IO::ParserType::ImageList)
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

UTF8Char *Map::GoogleMap::GoogleTileMap::GetImageURL(UTF8Char *sbuff, UOSInt level, Int64 imgId)
{
	Int32 imgX = (Int32)(imgId >> 32);
	Int32 imgY = (Int32)(imgId & 0xffffffffLL);
	UTF8Char *sptr;
	sptr = Text::StrConcatC(sbuff, UTF8STRC(GMAPURL));
	sptr = Text::StrConcatC(sptr, UTF8STRC("lyrs="));
	*sptr++ = (UTF8Char)this->mapType;
	sptr = Text::StrConcatC(sptr, UTF8STRC("&x="));
	sptr = Text::StrInt32(sptr, imgX);
	sptr = Text::StrConcatC(sptr, UTF8STRC("&y="));
	sptr = Text::StrInt32(sptr, imgY);
	sptr = Text::StrConcatC(sptr, UTF8STRC("&z="));
	sptr = Text::StrUOSInt(sptr, level);
	return sptr;
}

IO::IStreamData *Map::GoogleMap::GoogleTileMap::LoadTileImageData(UOSInt level, Int64 imgId, Double *boundsXY, Bool localOnly, Int32 *blockX, Int32 *blockY, ImageType *it)
{
	OSInt readSize;
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
	if (level < 0 || level > this->maxLevel)
		return 0;
	Int32 imgX = (Int32)(imgId >> 32);
	Int32 imgY = (Int32)(imgId & 0xffffffffLL);
	Double x1 = Map::OSM::OSMTileMap::TileX2Lon(imgX, level);
	Double y1 = Map::OSM::OSMTileMap::TileY2Lat(imgY, level);
	Double x2 = Map::OSM::OSMTileMap::TileX2Lon(imgX + 1, level);
	Double y2 = Map::OSM::OSMTileMap::TileY2Lat(imgY + 1, level);

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
		sptru = Text::StrConcatC(sptru, UTF8STRC(".png"));
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
		sptru = Text::StrConcatC(sptru, UTF8STRC(".png"));
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

	urlSb.ClearStr();
	urlSb.AppendC(UTF8STRC(GMAPURL));
	urlSb.AppendC(UTF8STRC("lyrs="));
	urlSb.AppendChar((UTF32Char)this->mapType, 1);
	urlSb.AppendC(UTF8STRC("&x="));
	urlSb.AppendI32(imgX);
	urlSb.AppendC(UTF8STRC("&y="));
	urlSb.AppendI32(imgY);
	urlSb.AppendC(UTF8STRC("&z="));
	urlSb.AppendOSInt(level);

	cli = Net::HTTPClient::CreateClient(this->sockf, this->ssl, (const UTF8Char*)"GoogleTileMap/1.0 SSWR/1.0", true, urlSb.StartsWith((const UTF8Char*)"https://"));
	cli->Connect(urlSb.ToString(), "GET", 0, 0, true);
	if (hasTime)
	{
		Net::HTTPClient::Date2Str(u8buff, &dt);
		cli->AddHeader((const UTF8Char*)"If-Modified-Since", u8buff);
	}
	if (cli->GetRespStatus() == 304)
	{
		NEW_CLASS(fs, IO::FileStream(filePathU, IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		dt.SetCurrTimeUTC();
		fs->SetFileTimes(&dt, 0, 0);
		DEL_CLASS(fs);
	}
	else
	{
		Int64 contLeng = cli->GetContentLength();
		OSInt currPos = 0;
		UInt8 *imgBuff;
		if (contLeng > 0 && contLeng <= 10485760)
		{
			imgBuff = MemAlloc(UInt8, (OSInt)contLeng);
			while ((readSize = cli->Read(&imgBuff[currPos], (OSInt)contLeng - currPos)) > 0)
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
					NEW_CLASS(fs, IO::FileStream(filePathU, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer));
					fs->Write(imgBuff, (OSInt)contLeng);
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
					this->spkg->AddFile(imgBuff, (OSInt)contLeng, filePathU, dt.ToTicks());
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
