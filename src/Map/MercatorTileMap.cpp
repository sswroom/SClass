#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/StmData/BufferedStreamData.h"
#include "IO/StmData/FileData.h"
#include "Math/Math_C.h"
#include "Map/MercatorTileMap.h"
#include "Math/CoordinateSystemManager.h"
#include "Net/HTTPClient.h"
#include "Text/MyString.h"

Map::MercatorTileMap::MercatorTileMap(Text::CString cacheDir, UIntOS minLevel, UIntOS maxLevel, NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl)
{
	this->cacheDir = Text::String::NewOrNull(cacheDir);
	this->spkg = nullptr;
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
	this->spkg.Delete();
	this->csys.Delete();
}

void Map::MercatorTileMap::SetSPackageFile(Optional<IO::SPackageFile> spkg)
{
	this->spkg.Delete();
	this->spkg = spkg;
}

Bool Map::MercatorTileMap::HasSPackageFile()
{
	return this->spkg.NotNull();
}

Bool Map::MercatorTileMap::ImportTiles(NN<IO::PackageFile> pkg)
{
	NN<IO::SPackageFile> spkg;
	if (this->spkg.SetTo(spkg))
	{
		return spkg->AddPackage(pkg, '/');
	}
	else
	{
		return false;
	}
}

Bool Map::MercatorTileMap::OptimizeToFile(Text::CStringNN fileName)
{
	NN<IO::SPackageFile> spkg;
	if (this->spkg.SetTo(spkg))
	{
		return spkg->OptimizeFile(fileName);
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

UIntOS Map::MercatorTileMap::GetMinLevel() const
{
	return this->minLevel;
}

UIntOS Map::MercatorTileMap::GetMaxLevel() const
{
	return this->maxLevel;
}


Double Map::MercatorTileMap::GetLevelScale(UIntOS index) const
{
	return 204094080000.0 / UIntOS2Double(this->tileWidth) / (Double)(1 << index);
}

UIntOS Map::MercatorTileMap::GetNearestLevel(Double scale) const
{
	Int32 level = Double2Int32(Math_Log10(204094080000.0 / scale / UIntOS2Double(this->tileWidth)) / Math_Log10(2));
	if (level < (Int32)this->minLevel)
		return this->minLevel;
	else if (level > (Int32)this->maxLevel)
		return this->maxLevel;
	return (UIntOS)level;
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

UIntOS Map::MercatorTileMap::GetTileSize() const
{
	return this->tileWidth;
}

UIntOS Map::MercatorTileMap::GetTileImageIDs(UIntOS level, Math::RectAreaDbl rect, NN<Data::ArrayListT<Math::Coord2D<Int32>>> ids)
{
	Int32 i;
	Int32 j;
	Double tmp;
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
	if (rect.min.x > rect.max.x)
	{
		tmp = rect.min.x;
		rect.min.x = rect.max.x;
		rect.max.x = tmp;
	}
	while (rect.max.x < -180)
	{
		rect.max.x += 360;
		rect.min.x += 360;
	}
	while (rect.min.x >= 180)
	{
		rect.max.x -= 360;
		rect.min.x -= 360;
	}
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
	Int32 x = pixX2 - pixX1 + 1;
	Int32 y = pixY2 - pixY1 + 1;
	if (x <= 0 || y <= 0)
		return 0;
	return (UIntOS)(x * y);
}

Optional<Media::ImageList> Map::MercatorTileMap::LoadTileImage(UIntOS level, Math::Coord2D<Int32> tileId, NN<Parser::ParserList> parsers, OutParam<Math::RectAreaDbl> bounds, Bool localOnly)
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
		return nullptr;
	Int32 loadTileIdX = tileId.x;
	Int32 maxX = (1 << level);
	while (loadTileIdX < 0)
	{
		loadTileIdX += maxX;
	}
	while (loadTileIdX >= maxX)
	{
		loadTileIdX -= maxX;
	}
	Double x1 = TileX2Lon(tileId.x, level);
	Double y1 = TileY2Lat(tileId.y, level);
	Double x2 = TileX2Lon(tileId.x + 1, level);
	Double y2 = TileY2Lat(tileId.y + 1, level);

	bounds.Set(Math::RectAreaDbl(Math::Coord2DDbl(x1, y1), Math::Coord2DDbl(x2, y2)));
	if (y1 < -90)
		return nullptr;

	NN<IO::SPackageFile> spkg;
	NN<Text::String> s;
	if (this->cacheDir.SetTo(s))
	{
		sptru = s->ConcatTo(filePathU);
		if (sptru[-1] != IO::Path::PATH_SEPERATOR)
			*sptru++ = IO::Path::PATH_SEPERATOR;
		sptru = Text::StrUIntOS(sptru, level);
		*sptru++ = IO::Path::PATH_SEPERATOR;
		sptru = Text::StrInt32(sptru, loadTileIdX);
		IO::Path::CreateDirectory(CSTRP(filePathU, sptru));
		*sptru++ = IO::Path::PATH_SEPERATOR;
		sptru = Text::StrInt32(sptru, tileId.y);
		sptru = Text::StrConcatC(sptru, UTF8STRC(".png"));
		if (IO::Path::GetPathType(CSTRP(filePathU, sptru)) == IO::Path::PathType::File)
		{
			NN<IO::StmData::FileData> fd;
			NEW_CLASSNN(fd, IO::StmData::FileData({filePathU, (UIntOS)(sptru - filePathU)}, false));
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
							return NN<Media::ImageList>::ConvertFrom(pobj);
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
				return nullptr;
			}
		}
	}
	else if (this->spkg.SetTo(spkg))
	{
		sptru = Text::StrUIntOS(filePathU, level);
		*sptru++ = '/';
		sptru = Text::StrInt32(sptru, loadTileIdX);
		*sptru++ = '/';
		sptru = Text::StrInt32(sptru, tileId.y);
		sptru = Text::StrConcatC(sptru, UTF8STRC(".png"));
		NN<IO::StreamData> fd;
		if (spkg->CreateStreamData({filePathU, (UIntOS)(sptru - filePathU)}).SetTo(fd))
		{
			IO::StmData::BufferedStreamData bsd(fd);
			if (parsers->ParseFile(bsd).SetTo(pobj))
			{
				if (pobj->GetParserType() == IO::ParserType::ImageList)
				{
					return NN<Media::ImageList>::ConvertFrom(pobj);
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
		return nullptr;

	urlPtr = this->GetTileImageURL(url, level, Math::Coord2D<Int32>(loadTileIdX, tileId.y)).Or(url);

	Net::WebStatus::StatusCode status;
	UIntOS redirCnt = 10;
	while (redirCnt-- > 0)
	{
		cli = Net::HTTPClient::CreateClient(this->clif, this->ssl, CSTR("MercatorTileMap/1.0 SSWR/1.0"), true, Text::StrStartsWithC(url, (UIntOS)(urlPtr - url), UTF8STRC("https://")));
		cli->Connect(CSTRP(url, urlPtr), Net::WebUtil::RequestMethod::HTTP_GET, 0, 0, true);
		cli->SetTimeout(5000);
		if (hasTime)
		{
			sptr = Net::WebUtil::Date2Str(sbuff, ts);
			cli->AddHeaderC(CSTR("If-Modified-Since"), {sbuff, (UIntOS)(sptr - sbuff)});
		}
		status = cli->GetRespStatus();
		if (status != Net::WebStatus::SC_MOVED_PERMANENTLY)
		{
			break;
		}
		if (!cli->GetRespHeader(CSTR("Location"), url).SetTo(urlPtr))
		{
			break;
		}
		printf("Redirecting URL: %s\r\n", url);
		cli.Delete();
	}
	if (status == 304)
	{
		IO::FileStream fs({filePathU, (UIntOS)(sptru - filePathU)}, IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
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
				IO::FileStream fs({filePathU, (UIntOS)(sptru - filePathU)}, IO::FileMode::Create, IO::FileShare::DenyAll, IO::FileStream::BufferType::NoWriteBuffer);
				fs.Write(mstm.GetArray());
				Data::DateTime dt;
				if (cli->GetLastModified(dt))
				{
					Data::DateTime currTime;
					currTime.SetCurrTimeUTC();
					fs.SetFileTimes(&currTime, nullptr, &dt);
				}
				else
				{
					fs.SetFileTimes(Data::Timestamp::UtcNow(), Data::Timestamp(0), Data::Timestamp(0));
				}
			}
			else if (this->spkg.SetTo(spkg))
			{
				spkg->AddFile(mstm.GetBuff(), (UIntOS)mstm.GetLength(), {filePathU, (UIntOS)(sptru - filePathU)}, ts);
//					printf("Add File: %d, %d, %s\r\n", ret, (Int32)contLeng, filePathU);
			}
		}
	}
	else
	{
	}
	cli.Delete();

	NN<IO::StreamData> nnfd;
	Optional<IO::StreamData> fd = nullptr;
	if (!this->cacheDir.IsNull())
	{
		NEW_CLASSNN(nnfd, IO::StmData::FileData({filePathU, (UIntOS)(sptru - filePathU)}, false));
		fd = nnfd;
	}
	else if (this->spkg.SetTo(spkg))
	{
		fd = spkg->CreateStreamData({filePathU, (UIntOS)(sptru - filePathU)});
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
					return NN<Media::ImageList>::ConvertFrom(pobj);
				}
				pobj.Delete();
			}
		}
		else
		{
			nnfd.Delete();
		}
	}
	return nullptr;
}

Optional<IO::StreamData> Map::MercatorTileMap::LoadTileImageData(UIntOS level, Math::Coord2D<Int32> tileId, OutParam<Math::RectAreaDbl> bounds, Bool localOnly, OptOut<ImageType> it)
{
	UIntOS readSize;
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
		return nullptr;
	Int32 loadTileIdX = tileId.x;
	Int32 maxX = (1 << level);
	while (loadTileIdX < 0)
	{
		loadTileIdX += maxX;
	}
	while (loadTileIdX >= maxX)
	{
		loadTileIdX -= maxX;
	}
	Double x1 = TileX2Lon(tileId.x, level);
	Double y1 = TileY2Lat(tileId.y, level);
	Double x2 = TileX2Lon(tileId.x + 1, level);
	Double y2 = TileY2Lat(tileId.y + 1, level);
	NN<IO::SPackageFile> spkg;

	bounds.Set(Math::RectAreaDbl(Math::Coord2DDbl(x1, y1), Math::Coord2DDbl(x2, y2)));
	if (y1 < -90)
		return nullptr;

	if (this->cacheDir.SetTo(s))
	{
		sptru = s->ConcatTo(filePathU);
		if (sptru[-1] != IO::Path::PATH_SEPERATOR)
			*sptru++ = IO::Path::PATH_SEPERATOR;
		sptru = Text::StrUIntOS(sptru, level);
		*sptru++ = IO::Path::PATH_SEPERATOR;
		sptru = Text::StrInt32(sptru, loadTileIdX);
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
		NEW_CLASSNN(fd, IO::StmData::FileData({filePathU, (UIntOS)(sptru - filePathU)}, false));
		if (fd->GetDataSize() > 0)
		{
			currTime.SetCurrTimeUTC();
			currTime.AddDay(-7);
			NN<IO::StmData::FileData>::ConvertFrom(fd)->GetFileStream()->GetFileTimes(&dt, nullptr, nullptr);
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
	else if (this->spkg.SetTo(spkg))
	{
		sptru = Text::StrInt32(filePathU, (Int32)level);
		*sptru++ = '/';
		sptru = Text::StrInt32(sptru, loadTileIdX);
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
		if (spkg->CreateStreamData({filePathU, (UIntOS)(sptru - filePathU)}).SetTo(fd))
		{
			it.Set(imgt);
			return fd;
		}
	}

	if (localOnly)
		return nullptr;

	urlPtr = this->GetTileImageURL(url, level, Math::Coord2D<Int32>(loadTileIdX, tileId.y)).Or(url);

	cli = Net::HTTPClient::CreateClient(this->clif, this->ssl, CSTR("MercatorTileMap/1.0 SSWR/1.0"), true, Text::StrStartsWithC(url, (UIntOS)(urlPtr - url), UTF8STRC("https://")));
	cli->Connect(CSTRP(url, urlPtr), Net::WebUtil::RequestMethod::HTTP_GET, 0, 0, true);
	if (hasTime)
	{
		sptr = Net::WebUtil::Date2Str(sbuff, dt);
		cli->AddHeaderC(CSTR("If-Modified-Since"), CSTRP(sbuff, sptr));
	}
	Net::WebStatus::StatusCode status = cli->GetRespStatus();
	if (status == 304)
	{
		IO::FileStream fs({filePathU, (UIntOS)(sptru - filePathU)}, IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		dt.SetCurrTimeUTC();
		fs.SetFileTimes(&dt, nullptr, nullptr);
	}
	else if (status >= 200 && status < 300)
	{
		UInt64 contLeng = cli->GetContentLength();
		UIntOS currPos = 0;
		UInt8 *imgBuff;
		if (contLeng > 0 && contLeng <= 10485760)
		{
			imgBuff = MemAlloc(UInt8, (UIntOS)contLeng);
			while ((readSize = cli->Read(Data::ByteArray(&imgBuff[currPos], (UIntOS)contLeng - currPos))) > 0)
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
					IO::FileStream fs({filePathU, (UIntOS)(sptru - filePathU)}, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer);
					fs.Write(Data::ByteArrayR(imgBuff, (UIntOS)contLeng));
					if (cli->GetLastModified(dt))
					{
						currTime.SetCurrTimeUTC();
						fs.SetFileTimes(&currTime, nullptr, &dt);
					}
					else
					{
						currTime.SetCurrTimeUTC();
						fs.SetFileTimes(&currTime, nullptr, nullptr);
					}
				}
				else if (this->spkg.SetTo(spkg))
				{
					spkg->AddFile(imgBuff, (UIntOS)contLeng, {filePathU, (UIntOS)(sptru - filePathU)}, Data::Timestamp::UtcNow());
				}
			}
			MemFree(imgBuff);
		}

	}
	cli.Delete();

	Optional<IO::StreamData> optfd = nullptr;
	if (!this->cacheDir.IsNull())
	{
		NEW_CLASSNN(fd, IO::StmData::FileData({filePathU, (UIntOS)(sptru - filePathU)}, false));
		optfd = fd;
	}
	else if (this->spkg.SetTo(spkg))
	{
		optfd = spkg->CreateStreamData({filePathU, (UIntOS)(sptru - filePathU)});
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
	return nullptr;
}

Int32 Map::MercatorTileMap::Lon2TileX(Double lon, UIntOS level)
{
	Double x = ((lon + 180.0) / 360.0 * (1 << level));
	Int32 ix = (Int32)x;
	if (x - ix < 0)
		return ix - 1;
	else
		return ix;
}

Int32 Map::MercatorTileMap::Lat2TileY(Double lat, UIntOS level)
{
	Double y = ((1.0 - Math_Ln( Math_Tan(lat * Math::PI / 180.0) + 1.0 / Math_Cos(lat * Math::PI / 180.0)) / Math::PI) / 2.0 * (1 << level));
	Int32 iy = (Int32)y;
	if (y - iy < 0)
		return iy - 1;
	else
		return iy;
}

Int32 Map::MercatorTileMap::Lon2TileXR(Double lon, UIntOS level)
{
	return Double2Int32((lon + 180.0) / 360.0 * (1 << level)); 
}

Int32 Map::MercatorTileMap::Lat2TileYR(Double lat, UIntOS level)
{
	return Double2Int32((1.0 - Math_Ln( Math_Tan(lat * Math::PI / 180.0) + 1.0 / Math_Cos(lat * Math::PI / 180.0)) / Math::PI) / 2.0 * (1 << level));
}

Double Map::MercatorTileMap::TileX2Lon(Int32 x, UIntOS level)
{
	return x * 360.0 / (1 << level) - 180;
}

Double Map::MercatorTileMap::TileY2Lat(Int32 y, UIntOS level)
{
	Double n = Math::PI - 2.0 * Math::PI * y / (1 << level);
	return 180.0 / Math::PI * Math_ArcTan(0.5 * (Math_Exp(n) - Math_Exp(-n)));
}
