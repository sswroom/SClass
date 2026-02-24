#include "Stdafx.h"
#include "Math/Math_C.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Map/TileMapUtil.h"
#include "Map/ESRI/ESRITileMap.h"
#include "Text/MyString.h"

Map::ESRI::ESRITileMap::ESRITileMap(NN<Map::ESRI::ESRIMapServer> esriMap, Bool toRelease, Text::CStringNN cacheDir)
{
	this->esriMap = esriMap;
	this->toRelease = toRelease;
	this->cacheDir = Text::String::New(cacheDir);
	this->dispBounds = this->esriMap->GetInitBounds();
	this->dispSize = Math::Size2DDbl(640, 480);
	this->dispDPI = 96.0;
}

Map::ESRI::ESRITileMap::~ESRITileMap()
{
	this->cacheDir->Release();
	if (this->toRelease)
	{
		this->esriMap.Delete();
		this->toRelease = false;
	}
}

Text::CStringNN Map::ESRI::ESRITileMap::GetName() const
{
	return CSTR("ESRITileMap");
}

Bool Map::ESRI::ESRITileMap::IsError() const
{
	return this->esriMap->IsError() || !this->esriMap->HasTile();
}

Map::TileMap::TileType Map::ESRI::ESRITileMap::GetTileType() const
{
	return Map::TileMap::TileType::ESRI;
}

UIntOS Map::ESRI::ESRITileMap::GetMinLevel() const
{
	return 0;
}

UIntOS Map::ESRI::ESRITileMap::GetMaxLevel() const
{
	return this->esriMap->TileGetLevelCount() - 1;
}


Double Map::ESRI::ESRITileMap::GetLevelScale(UIntOS index) const
{
	Double scaleDiv = Map::TileMapUtil::CalcScaleDiv(this->esriMap->GetCoordinateSystem());
	Double level = this->esriMap->TileGetLevelResolution(index);
	if (level == 0)
		return 0;

	return level / scaleDiv;
}

UIntOS Map::ESRI::ESRITileMap::GetNearestLevel(Double scale) const
{
	Double scaleDiv = Map::TileMapUtil::CalcScaleDiv(this->esriMap->GetCoordinateSystem());
	Double ldiff;
	Double minDiff;
	UIntOS minInd;
	UIntOS i;
	Double logResol = Math_Log10(scale * scaleDiv);
	minInd = 0;
	minDiff = 100000.0;
	i = this->esriMap->TileGetLevelCount();
	while (i-- > 0)
	{
		ldiff = Math_Log10(this->esriMap->TileGetLevelResolution(i)) - logResol;
		if (ldiff < 0)
			ldiff = -ldiff;
		if (ldiff < minDiff)
		{
			minDiff = ldiff;
			minInd = i;
		}
	}
	return minInd;
}

UIntOS Map::ESRI::ESRITileMap::GetConcurrentCount() const
{
	return 1;
}

Bool Map::ESRI::ESRITileMap::GetBounds(OutParam<Math::RectAreaDbl> bounds) const
{
	Math::RectAreaDbl bnd;
	bounds.Set(bnd = this->esriMap->GetBounds());
	return bnd.min.x != 0 || bnd.min.y != 0 || bnd.max.x != 0 || bnd.max.y != 0;
}

NN<Math::CoordinateSystem> Map::ESRI::ESRITileMap::GetCoordinateSystem() const
{
	return this->esriMap->GetCoordinateSystem();
}

Bool Map::ESRI::ESRITileMap::IsMercatorProj() const
{
	return false;
}

UIntOS Map::ESRI::ESRITileMap::GetTileSize() const
{
	return this->esriMap->TileGetWidth();
}

Map::TileMap::TileFormat Map::ESRI::ESRITileMap::GetTileFormat() const
{
	return Map::TileMap::TileFormat::PNG;
}

Bool Map::ESRI::ESRITileMap::CanQuery() const
{
	return true;
}

Bool Map::ESRI::ESRITileMap::QueryInfos(Math::Coord2DDbl coord, UIntOS level, NN<Data::ArrayListNN<Math::Geometry::Vector2D>> vecList, NN<Data::ArrayListNative<UIntOS>> valueOfstList, NN<Data::ArrayListStringNN> nameList, NN<Data::ArrayListNN<Text::String>> valueList) const
{
	return this->esriMap->QueryInfos(coord, this->dispBounds, (UInt32)Double2Int32(this->dispSize.x), (UInt32)Double2Int32(this->dispSize.y), this->dispDPI, vecList, valueOfstList, nameList, valueList);
}

void Map::ESRI::ESRITileMap::SetDispSize(Math::Size2DDbl size, Double dpi)
{
	this->dispSize = size;
	this->dispDPI = dpi;
}

UIntOS Map::ESRI::ESRITileMap::GetTileImageIDs(UIntOS level, Math::RectAreaDbl rect, NN<Data::ArrayListT<Math::Coord2D<Int32>>> ids)
{
	Double resol = this->esriMap->TileGetLevelResolution(level);
	Int32 i;
	Int32 j;
	if (resol == 0)
		return 0;
	Math::RectAreaDbl bounds = this->esriMap->GetBounds();
	rect.min = rect.min.Min(bounds.max).Max(bounds.min);
	rect.max = rect.max.Min(bounds.max).Max(bounds.min);

	if (rect.min.x == rect.max.x)
		return 0;
	if (rect.min.y == rect.max.y)
		return 0;
	this->dispBounds = rect;
	UIntOS tileWidth = this->esriMap->TileGetWidth();
	UIntOS tileHeight = this->esriMap->TileGetHeight();
	Math::Coord2DDbl origin = this->esriMap->TileGetOrigin();
	Int32 pixX1 = (Int32)((rect.min.x - origin.x) / resol / UIntOS2Double(tileWidth));
	Int32 pixX2 = (Int32)((rect.max.x - origin.x) / resol / UIntOS2Double(tileWidth));
	Int32 pixY1 = (Int32)((origin.y - rect.min.y) / resol / UIntOS2Double(tileHeight));
	Int32 pixY2 = (Int32)((origin.y - rect.max.y) / resol / UIntOS2Double(tileHeight));
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
	return (UIntOS)((pixX2 - pixX1 + 1) * (pixY2 - pixY1 + 1));
}

Optional<Media::ImageList> Map::ESRI::ESRITileMap::LoadTileImage(UIntOS level, Math::Coord2D<Int32> tileId, NN<Parser::ParserList> parsers, OutParam<Math::RectAreaDbl> bounds, Bool localOnly)
{
	UTF8Char filePath[512];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> filePathEnd;
	NN<IO::ParsedObject> pobj;
	Double resol = this->esriMap->TileGetLevelResolution(level);
	if (resol == 0)
		return nullptr;
	UIntOS tileWidth = this->esriMap->TileGetWidth();
	UIntOS tileHeight = this->esriMap->TileGetHeight();
	Math::Coord2DDbl origin = this->esriMap->TileGetOrigin();
	Double x1 = tileId.x * UIntOS2Double(tileWidth) * resol + origin.x;
	Double y1 = origin.y - tileId.y * UIntOS2Double(tileHeight) * resol;
	Double x2 = x1 + UIntOS2Double(tileWidth) * resol;
	Double y2 = y1 - UIntOS2Double(tileHeight) * resol;

	Math::RectAreaDbl b = Math::RectAreaDbl(Math::Coord2DDbl(x1, y1), Math::Coord2DDbl(x2, y2));
	bounds.Set(b);
	if (!b.OverlapOrTouch(this->esriMap->GetBounds()))
		return nullptr;

	sptr = this->cacheDir->ConcatTo(filePath);
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
		*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrInt32(sptr, (Int32)level);
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrInt32(sptr, tileId.y);
	IO::Path::CreateDirectory(CSTRP(filePath, sptr));
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrInt32(sptr, tileId.x);
	filePathEnd = Text::StrConcatC(sptr, UTF8STRC(".dat"));
	{
		IO::StmData::FileData fd({filePath, (UIntOS)(filePathEnd - filePath)}, false);
		if (fd.GetDataSize() > 0)
		{
			if (parsers->ParseFile(fd).SetTo(pobj))
			{
				if (pobj->GetParserType() == IO::ParserType::ImageList)
				{
					return NN<Media::ImageList>::ConvertFrom(pobj);
				}
				pobj.Delete();
			}
		}
	}

	if (localOnly)
		return nullptr;

	this->esriMap->TileLoadToFile(CSTRP(filePath, filePathEnd), level, tileId.x, tileId.y);
	IO::StmData::FileData fd({filePath, (UIntOS)(filePathEnd - filePath)}, false);
	if (fd.GetDataSize() > 0)
	{
		if (parsers->ParseFile(fd).SetTo(pobj))
		{
			if (pobj->GetParserType() == IO::ParserType::ImageList)
			{
				return NN<Media::ImageList>::ConvertFrom(pobj);
			}
			pobj.Delete();
		}
	}
	return nullptr;
}

UnsafeArrayOpt<UTF8Char> Map::ESRI::ESRITileMap::GetTileImageURL(UnsafeArray<UTF8Char> sbuff, UIntOS level, Math::Coord2D<Int32> tileId)
{
	return this->esriMap->TileGetURL(sbuff, level, tileId.x, tileId.y);
}

Bool Map::ESRI::ESRITileMap::GetTileImageURL(NN<Text::StringBuilderUTF8> sb, UIntOS level, Math::Coord2D<Int32> tileId)
{
	return this->esriMap->TileGetURL(sb, level, tileId.x, tileId.y);
}

Optional<IO::StreamData> Map::ESRI::ESRITileMap::LoadTileImageData(UIntOS level, Math::Coord2D<Int32> tileId, OutParam<Math::RectAreaDbl> bounds, Bool localOnly, OptOut<TileFormat> format)
{
	UTF8Char filePath[512];
	UnsafeArray<UTF8Char> sptr;
	IO::StmData::FileData *fd;
	Double resol = this->esriMap->TileGetLevelResolution(level);
	if (resol == 0)
		return nullptr;
	UIntOS tileWidth = this->esriMap->TileGetWidth();
	UIntOS tileHeight = this->esriMap->TileGetHeight();
	Math::Coord2DDbl origin = this->esriMap->TileGetOrigin();
	Double x1 = tileId.x * UIntOS2Double(tileWidth) * resol + origin.x;
	Double y1 = origin.y - tileId.y * UIntOS2Double(tileHeight) * resol;
	Double x2 = x1 + UIntOS2Double(tileWidth) * resol;
	Double y2 = y1 - UIntOS2Double(tileHeight) * resol;

	Math::RectAreaDbl b = Math::RectAreaDbl(Math::Coord2DDbl(x1, y1), Math::Coord2DDbl(x2, y2));
	bounds.Set(b);
	if (!b.OverlapOrTouch(this->esriMap->GetBounds()))
		return nullptr;

	sptr = this->cacheDir->ConcatTo(filePath);
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
		*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrUIntOS(sptr, level);
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrInt32(sptr, tileId.y);
	IO::Path::CreateDirectory(CSTRP(filePath, sptr));
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrInt32(sptr, tileId.x);
	sptr = Text::StrConcatC(sptr, UTF8STRC(".dat"));
	NEW_CLASS(fd, IO::StmData::FileData({filePath, (UIntOS)(sptr - filePath)}, false));
	if (fd->GetDataSize() > 0)
	{
		format.Set(TileFormat::PNG);
		return fd;
	}
	DEL_CLASS(fd);

	if (localOnly)
		return nullptr;

	this->esriMap->TileLoadToFile(CSTRP(filePath, sptr), level, tileId.x, tileId.y);
	NEW_CLASS(fd, IO::StmData::FileData({filePath, (UIntOS)(sptr - filePath)}, false));
	if (fd->GetDataSize() > 0)
	{
		format.Set(TileFormat::PNG);
		return fd;
	}
	DEL_CLASS(fd);
	return nullptr;
}
