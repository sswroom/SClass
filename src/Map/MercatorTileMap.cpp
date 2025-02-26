#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/StmData/BufferedStreamData.h"
#include "IO/StmData/FileData.h"
#include "Math/Math.h"
#include "Map/MercatorTileMap.h"
#include "Math/CoordinateSystemManager.h"
#include "Net/HTTPClient.h"
#include "Text/MyString.h"

Map::MercatorTileMap::MercatorTileMap(Text::CString cacheDir, UOSInt minLevel, UOSInt maxLevel, NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl)
{
	this->cacheDir = Text::String::NewOrNull(cacheDir);
	this->spkg = 0;
	this->clif = clif;
	this->ssl = ssl;
	this->tileWidth = 256;
	this->tileHeight = 256;
	this->minLevel = minLevel;
	this->maxLevel = maxLevel;
	this->csys = Math::CoordinateSystemManager::CreateWGS84Csys();
}

Map::MercatorTileMap::~MercatorTileMap()
{
	OPTSTR_DEL(this->cacheDir);
	SDEL_CLASS(this->spkg);
	this->csys.Delete();
}

void Map::MercatorTileMap::SetSPackageFile(IO::SPackageFile *spkg)
{
	SDEL_CLASS(this->spkg);
	this->spkg = spkg;
}

Bool Map::MercatorTileMap::HasSPackageFile()
{
	return this->spkg != 0;
}

Bool Map::MercatorTileMap::ImportTiles(NN<IO::PackageFile> pkg)
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

Bool Map::MercatorTileMap::OptimizeToFile(Text::CStringNN fileName)
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

Bool Map::MercatorTileMap::IsError() const
{
	return false;
}

UOSInt Map::MercatorTileMap::GetMinLevel() const
{
	return this->minLevel;
}

UOSInt Map::MercatorTileMap::GetMaxLevel() const
{
	return this->maxLevel;
}


Double Map::MercatorTileMap::GetLevelScale(UOSInt index) const
{
	return 204094080000.0 / UOSInt2Double(this->tileWidth) / (Double)(1 << index);
}

UOSInt Map::MercatorTileMap::GetNearestLevel(Double scale) const
{
	Int32 level = Double2Int32(Math_Log10(204094080000.0 / scale / UOSInt2Double(this->tileWidth)) / Math_Log10(2));
	if (level < (Int32)this->minLevel)
		return this->minLevel;
	else if (level > (Int32)this->maxLevel)
		return this->maxLevel;
	return (UOSInt)level;
}

Bool Map::MercatorTileMap::GetBounds(OutParam<Math::RectAreaDbl> bounds) const
{
	bounds.Set(Math::RectAreaDbl(Math::Coord2DDbl(-180, -85.051128779806592377796715521925),
		Math::Coord2DDbl(180, 85.051128779806592377796715521925)));
	return true;
}

NN<Math::CoordinateSystem> Map::MercatorTileMap::GetCoordinateSystem() const
{
	return this->csys;
}

Bool Map::MercatorTileMap::IsMercatorProj() const
{
	return true;
}

UOSInt Map::MercatorTileMap::GetTileSize() const
{
	return this->tileWidth;
}

UOSInt Map::MercatorTileMap::GetTileImageIDs(UOSInt level, Math::RectAreaDbl rect, Data::ArrayList<Math::Coord2D<Int32>> *ids)
{
	Int32 i;
	Int32 j;
	Double max = 85.051128779806592377796715521925;
	if (rect.min.y < -max)
		rect.min.y = -max;
	else if (rect.min.y > max)
		rect.min.y = max;
	if (rect.max.y < -max)
		rect.max.y = -max;
	else if (rect.max.y > max)
		rect.max.y = max;
	
	if (rect.min.x == rect.max.x)
		return 0;
	if (rect.min.y == rect.max.y)
		return 0;
	Int32 pixX1 = Lon2TileX(rect.min.x, level);
	Int32 pixX2 = Lon2TileX(rect.max.x, level);
	Int32 pixY1 = Lat2TileY(rect.min.y, level);
	Int32 pixY2 = Lat2TileY(rect.max.y, level);
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
			ids->Add(Math::Coord2D<Int32>(j, i));
			j++;
		}
		i++;
	}
	return (UOSInt)((pixX2 - pixX1 + 1) * (pixY2 - pixY1 + 1));
}

Media::ImageList *Map::MercatorTileMap::LoadTileImage(UOSInt level, Math::Coord2D<Int32> tileId, NN<Parser::ParserList> parsers, OutParam<Math::RectAreaDbl> bounds, Bool localOnly)
{
	UTF8Char url[1024];
	UnsafeArray<UTF8Char> urlPtr;
	UTF8Char filePathU[512];
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptru = filePathU;
	Bool hasTime = false;
	Data::Timestamp ts;
	Data::Timestamp currTS;
	NN<Net::HTTPClient> cli;
	NN<IO::ParsedObject> pobj;
	if (level < this->minLevel || level > this->maxLevel)
		return 0;
	Double x1 = TileX2Lon(tileId.x, level);
	Double y1 = TileY2Lat(tileId.y, level);
	Double x2 = TileX2Lon(tileId.x + 1, level);
	Double y2 = TileY2Lat(tileId.y + 1, level);

	bounds.Set(Math::RectAreaDbl(Math::Coord2DDbl(x1, y1), Math::Coord2DDbl(x2, y2)));
	if (x1 > 180 || y1 < -90)
		return 0;

	NN<Text::String> s;
	if (this->cacheDir.SetTo(s))
	{
		sptru = s->ConcatTo(filePathU);
		if (sptru[-1] != IO::Path::PATH_SEPERATOR)
			*sptru++ = IO::Path::PATH_SEPERATOR;
		sptru = Text::StrUOSInt(sptru, level);
		*sptru++ = IO::Path::PATH_SEPERATOR;
		sptru = Text::StrInt32(sptru, tileId.x);
		IO::Path::CreateDirectory(CSTRP(filePathU, sptru));
		*sptru++ = IO::Path::PATH_SEPERATOR;
		sptru = Text::StrInt32(sptru, tileId.y);
		sptru = Text::StrConcatC(sptru, UTF8STRC(".png"));
		if (IO::Path::GetPathType(CSTRP(filePathU, sptru)) == IO::Path::PathType::File)
		{
			NN<IO::StmData::FileData> fd;
			NEW_CLASSNN(fd, IO::StmData::FileData({filePathU, (UOSInt)(sptru - filePathU)}, false));
			if (fd->GetDataSize() > 0)
			{
				currTS = Data::Timestamp::UtcNow().AddDay(-7);
				ts = fd->GetFileStream()->GetCreateTime();
				if (ts > currTS)
				{
					IO::StmData::BufferedStreamData bsd(fd);
					if (parsers->ParseFile(bsd).SetTo(pobj))
					{
						if (pobj->GetParserType() == IO::ParserType::ImageList)
						{
							return NN<Media::ImageList>::ConvertFrom(pobj).Ptr();
						}
						pobj.Delete();
					}
				}
				else
				{
					hasTime = true;
					fd.Delete();
				}
			}
			else
			{
				fd.Delete();
				return 0;
			}
		}
	}
	else if (this->spkg)
	{
		sptru = Text::StrUOSInt(filePathU, level);
		*sptru++ = '/';
		sptru = Text::StrInt32(sptru, tileId.x);
		*sptru++ = '/';
		sptru = Text::StrInt32(sptru, tileId.y);
		sptru = Text::StrConcatC(sptru, UTF8STRC(".png"));
		NN<IO::StreamData> fd;
		if (this->spkg->CreateStreamData({filePathU, (UOSInt)(sptru - filePathU)}).SetTo(fd))
		{
			IO::StmData::BufferedStreamData bsd(fd);
			if (parsers->ParseFile(bsd).SetTo(pobj))
			{
				if (pobj->GetParserType() == IO::ParserType::ImageList)
				{
					return NN<Media::ImageList>::ConvertFrom(pobj).Ptr();
				}
				pobj.Delete();
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

	urlPtr = this->GetTileImageURL(url, level, tileId).Or(url);

//	printf("Request URL: %s\r\n", urlSb.ToString());
	cli = Net::HTTPClient::CreateClient(this->clif, this->ssl, CSTR("MercatorTileMap/1.0 SSWR/1.0"), true, Text::StrStartsWithC(url, (UOSInt)(urlPtr - url), UTF8STRC("https://")));
	cli->Connect(CSTRP(url, urlPtr), Net::WebUtil::RequestMethod::HTTP_GET, 0, 0, true);
	cli->SetTimeout(5000);
	if (hasTime)
	{
		sptr = Net::WebUtil::Date2Str(sbuff, ts);
		cli->AddHeaderC(CSTR("If-Modified-Since"), {sbuff, (UOSInt)(sptr - sbuff)});
	}
	Net::WebStatus::StatusCode status = cli->GetRespStatus();
	if (status == 304)
	{
		IO::FileStream fs({filePathU, (UOSInt)(sptru - filePathU)}, IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		fs.SetFileTimes(Data::Timestamp::UtcNow(), Data::Timestamp(0), Data::Timestamp(0));
//		printf("Reply 304\r\n");
	}
	else if (status >= 200 && status < 300 && !cli->IsError())
	{
		IO::MemoryStream mstm;
		if (cli->ReadAllContent(mstm, 16384, 10485760))
		{
			if (!this->cacheDir.IsNull())
			{
				IO::FileStream fs({filePathU, (UOSInt)(sptru - filePathU)}, IO::FileMode::Create, IO::FileShare::DenyAll, IO::FileStream::BufferType::NoWriteBuffer);
				fs.Write(mstm.GetArray());
				Data::DateTime dt;
				if (cli->GetLastModified(dt))
				{
					Data::DateTime currTime;
					currTime.SetCurrTimeUTC();
					fs.SetFileTimes(&currTime, 0, &dt);
				}
				else
				{
					fs.SetFileTimes(Data::Timestamp::UtcNow(), Data::Timestamp(0), Data::Timestamp(0));
				}
			}
			else if (this->spkg)
			{
				this->spkg->AddFile(mstm.GetBuff(), (UOSInt)mstm.GetLength(), {filePathU, (UOSInt)(sptru - filePathU)}, ts);
//					printf("Add File: %d, %d, %s\r\n", ret, (Int32)contLeng, filePathU);
			}
		}
	}
	cli.Delete();

	NN<IO::StreamData> nnfd;
	Optional<IO::StreamData> fd = 0;
	if (!this->cacheDir.IsNull())
	{
		NEW_CLASSNN(nnfd, IO::StmData::FileData({filePathU, (UOSInt)(sptru - filePathU)}, false));
		fd = nnfd;
	}
	else if (this->spkg)
	{
		fd = this->spkg->CreateStreamData({filePathU, (UOSInt)(sptru - filePathU)});
	}
	if (fd.SetTo(nnfd))
	{
		if (nnfd->GetDataSize() > 0)
		{
			IO::StmData::BufferedStreamData bsd(nnfd);
			if (parsers->ParseFile(bsd).SetTo(pobj))
			{
				if (pobj->GetParserType() == IO::ParserType::ImageList)
				{
					return NN<Media::ImageList>::ConvertFrom(pobj).Ptr();
				}
				pobj.Delete();
			}
		}
		else
		{
			nnfd.Delete();
		}
	}
	return 0;
}

Optional<IO::StreamData> Map::MercatorTileMap::LoadTileImageData(UOSInt level, Math::Coord2D<Int32> tileId, OutParam<Math::RectAreaDbl> bounds, Bool localOnly, OptOut<ImageType> it)
{
	UOSInt readSize;
	UTF8Char url[1024];
	UnsafeArray<UTF8Char> urlPtr;
	UTF8Char filePathU[512];
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptru = filePathU;
	Bool hasTime = false;
	Data::DateTime dt;
	Data::DateTime currTime;
	NN<Net::HTTPClient> cli;
	NN<Text::String> s;
	NN<IO::StreamData> fd;
	if (level > this->maxLevel)
		return 0;
	Double x1 = TileX2Lon(tileId.x, level);
	Double y1 = TileY2Lat(tileId.y, level);
	Double x2 = TileX2Lon(tileId.x + 1, level);
	Double y2 = TileY2Lat(tileId.y + 1, level);

	bounds.Set(Math::RectAreaDbl(Math::Coord2DDbl(x1, y1), Math::Coord2DDbl(x2, y2)));
	if (x1 > 180 || y1 < -90)
		return 0;

	if (this->cacheDir.SetTo(s))
	{
		sptru = s->ConcatTo(filePathU);
		if (sptru[-1] != IO::Path::PATH_SEPERATOR)
			*sptru++ = IO::Path::PATH_SEPERATOR;
		sptru = Text::StrUOSInt(sptru, level);
		*sptru++ = IO::Path::PATH_SEPERATOR;
		sptru = Text::StrInt32(sptru, tileId.x);
		IO::Path::CreateDirectory(CSTRP(filePathU, sptru));
		*sptru++ = IO::Path::PATH_SEPERATOR;
		sptru = Text::StrInt32(sptru, tileId.y);
		ImageType imgt = this->GetImageType();
		switch (imgt)
		{
		case IT_JPG:
			sptru = Text::StrConcatC(sptru, UTF8STRC(".jpg"));
			break;
		case IT_WEBP:
			sptru = Text::StrConcatC(sptru, UTF8STRC(".webp"));
			break;
		default:
		case IT_PNG:
			sptru = Text::StrConcatC(sptru, UTF8STRC(".png"));
			break;
		}
		NEW_CLASSNN(fd, IO::StmData::FileData({filePathU, (UOSInt)(sptru - filePathU)}, false));
		if (fd->GetDataSize() > 0)
		{
			currTime.SetCurrTimeUTC();
			currTime.AddDay(-7);
			NN<IO::StmData::FileData>::ConvertFrom(fd)->GetFileStream()->GetFileTimes(&dt, 0, 0);
			if (dt.CompareTo(currTime) > 0)
			{
				it.Set(imgt);
				return fd;
			}
			else
			{
				hasTime = true;
			}
		}
		fd.Delete();
	}
	else if (this->spkg)
	{
		sptru = Text::StrInt32(filePathU, (Int32)level);
		*sptru++ = '/';
		sptru = Text::StrInt32(sptru, tileId.x);
		*sptru++ = '/';
		sptru = Text::StrInt32(sptru, tileId.y);
		ImageType imgt = this->GetImageType();
		switch (imgt)
		{
		case IT_JPG:
			sptru = Text::StrConcatC(sptru, UTF8STRC(".jpg"));
			break;
		case IT_WEBP:
			sptru = Text::StrConcatC(sptru, UTF8STRC(".webp"));
			break;
		default:
		case IT_PNG:
			sptru = Text::StrConcatC(sptru, UTF8STRC(".png"));
			break;
		}
		if (this->spkg->CreateStreamData({filePathU, (UOSInt)(sptru - filePathU)}).SetTo(fd))
		{
			it.Set(imgt);
			return fd;
		}
	}

	if (localOnly)
		return 0;

	urlPtr = this->GetTileImageURL(url, level, tileId).Or(url);

	cli = Net::HTTPClient::CreateClient(this->clif, this->ssl, CSTR("MercatorTileMap/1.0 SSWR/1.0"), true, Text::StrStartsWithC(url, (UOSInt)(urlPtr - url), UTF8STRC("https://")));
	cli->Connect(CSTRP(url, urlPtr), Net::WebUtil::RequestMethod::HTTP_GET, 0, 0, true);
	if (hasTime)
	{
		sptr = Net::WebUtil::Date2Str(sbuff, dt);
		cli->AddHeaderC(CSTR("If-Modified-Since"), CSTRP(sbuff, sptr));
	}
	Net::WebStatus::StatusCode status = cli->GetRespStatus();
	if (status == 304)
	{
		IO::FileStream fs({filePathU, (UOSInt)(sptru - filePathU)}, IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		dt.SetCurrTimeUTC();
		fs.SetFileTimes(&dt, 0, 0);
	}
	else if (status >= 200 && status < 300)
	{
		UInt64 contLeng = cli->GetContentLength();
		UOSInt currPos = 0;
		UInt8 *imgBuff;
		if (contLeng > 0 && contLeng <= 10485760)
		{
			imgBuff = MemAlloc(UInt8, (UOSInt)contLeng);
			while ((readSize = cli->Read(Data::ByteArray(&imgBuff[currPos], (UOSInt)contLeng - currPos))) > 0)
			{
				currPos += readSize;
				if (currPos >= contLeng)
				{
					break;
				}
			}
			if (currPos >= contLeng)
			{
				if (!this->cacheDir.IsNull())
				{
					IO::FileStream fs({filePathU, (UOSInt)(sptru - filePathU)}, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer);
					fs.Write(Data::ByteArrayR(imgBuff, (UOSInt)contLeng));
					if (cli->GetLastModified(dt))
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
					this->spkg->AddFile(imgBuff, (UOSInt)contLeng, {filePathU, (UOSInt)(sptru - filePathU)}, Data::Timestamp::UtcNow());
				}
			}
			MemFree(imgBuff);
		}

	}
	cli.Delete();

	Optional<IO::StreamData> optfd = 0;
	if (!this->cacheDir.IsNull())
	{
		NEW_CLASSNN(fd, IO::StmData::FileData({filePathU, (UOSInt)(sptru - filePathU)}, false));
		optfd = fd;
	}
	else if (this->spkg)
	{
		optfd = this->spkg->CreateStreamData({filePathU, (UOSInt)(sptru - filePathU)});
	}
	if (optfd.SetTo(fd))
	{
		if (fd->GetDataSize() > 0)
		{
			it.Set(this->GetImageType());
			return fd;
		}
		fd.Delete();
	}
	return 0;
}

Int32 Map::MercatorTileMap::Lon2TileX(Double lon, UOSInt level)
{
	return (Int32)((lon + 180.0) / 360.0 * (1 << level)); 
}

Int32 Map::MercatorTileMap::Lat2TileY(Double lat, UOSInt level)
{
	return (Int32)((1.0 - Math_Ln( Math_Tan(lat * Math::PI / 180.0) + 1.0 / Math_Cos(lat * Math::PI / 180.0)) / Math::PI) / 2.0 * (1 << level));
}

Int32 Map::MercatorTileMap::Lon2TileXR(Double lon, UOSInt level)
{
	return Double2Int32((lon + 180.0) / 360.0 * (1 << level)); 
}

Int32 Map::MercatorTileMap::Lat2TileYR(Double lat, UOSInt level)
{
	return Double2Int32((1.0 - Math_Ln( Math_Tan(lat * Math::PI / 180.0) + 1.0 / Math_Cos(lat * Math::PI / 180.0)) / Math::PI) / 2.0 * (1 << level));
}

Double Map::MercatorTileMap::TileX2Lon(Int32 x, UOSInt level)
{
	return x * 360.0 / (1 << level) - 180;
}

Double Map::MercatorTileMap::TileY2Lat(Int32 y, UOSInt level)
{
	Double n = Math::PI - 2.0 * Math::PI * y / (1 << level);
	return 180.0 / Math::PI * Math_ArcTan(0.5 * (Math_Exp(n) - Math_Exp(-n)));
}
