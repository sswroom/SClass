#include "Stdafx.h"
#include "Math/Math.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Map/TileMapUtil.h"
#include "Map/ESRI/ESRITileMap.h"
#include "Text/MyString.h"

Map::ESRI::ESRITileMap::ESRITileMap(Map::ESRI::ESRIMapServer *esriMap, Bool toRelease, Text::CString cacheDir)
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
		DEL_CLASS(this->esriMap);
		this->esriMap = 0;
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
	return Map::TileMap::TT_ESRI;
}

UOSInt Map::ESRI::ESRITileMap::GetMinLevel() const
{
	return 0;
}

UOSInt Map::ESRI::ESRITileMap::GetMaxLevel() const
{
	return this->esriMap->TileGetLevelCount() - 1;
}


Double Map::ESRI::ESRITileMap::GetLevelScale(UOSInt index) const
{
	Double scaleDiv = Map::TileMapUtil::CalcScaleDiv(this->esriMap->GetCoordinateSystem());
	Double level = this->esriMap->TileGetLevelResolution(index);
	if (level == 0)
		return 0;

	return level / scaleDiv;
}

UOSInt Map::ESRI::ESRITileMap::GetNearestLevel(Double scale) const
{
	Double scaleDiv = Map::TileMapUtil::CalcScaleDiv(this->esriMap->GetCoordinateSystem());
	Double ldiff;
	Double minDiff;
	UOSInt minInd;
	UOSInt i;
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

UOSInt Map::ESRI::ESRITileMap::GetConcurrentCount() const
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

UOSInt Map::ESRI::ESRITileMap::GetTileSize() const
{
	return this->esriMap->TileGetWidth();
}

Map::TileMap::ImageType Map::ESRI::ESRITileMap::GetImageType() const
{
	return IT_PNG;
}

Bool Map::ESRI::ESRITileMap::CanQuery() const
{
	return true;
}

Bool Map::ESRI::ESRITileMap::QueryInfos(Math::Coord2DDbl coord, UOSInt level, NN<Data::ArrayListNN<Math::Geometry::Vector2D>> vecList, NN<Data::ArrayList<UOSInt>> valueOfstList, NN<Data::ArrayListStringNN> nameList, NN<Data::ArrayListNN<Text::String>> valueList) const
{
	return this->esriMap->QueryInfos(coord, this->dispBounds, (UInt32)Double2Int32(this->dispSize.x), (UInt32)Double2Int32(this->dispSize.y), this->dispDPI, vecList, valueOfstList, nameList, valueList);
}

void Map::ESRI::ESRITileMap::SetDispSize(Math::Size2DDbl size, Double dpi)
{
	this->dispSize = size;
	this->dispDPI = dpi;
}

UOSInt Map::ESRI::ESRITileMap::GetTileImageIDs(UOSInt level, Math::RectAreaDbl rect, Data::ArrayList<Math::Coord2D<Int32>> *ids)
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
	UOSInt tileWidth = this->esriMap->TileGetWidth();
	UOSInt tileHeight = this->esriMap->TileGetHeight();
	Math::Coord2DDbl origin = this->esriMap->TileGetOrigin();
	Int32 pixX1 = (Int32)((rect.min.x - origin.x) / resol / UOSInt2Double(tileWidth));
	Int32 pixX2 = (Int32)((rect.max.x - origin.x) / resol / UOSInt2Double(tileWidth));
	Int32 pixY1 = (Int32)((origin.y - rect.min.y) / resol / UOSInt2Double(tileHeight));
	Int32 pixY2 = (Int32)((origin.y - rect.max.y) / resol / UOSInt2Double(tileHeight));
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
	return (UOSInt)((pixX2 - pixX1 + 1) * (pixY2 - pixY1 + 1));
}

Media::ImageList *Map::ESRI::ESRITileMap::LoadTileImage(UOSInt level, Math::Coord2D<Int32> tileId, NN<Parser::ParserList> parsers, OutParam<Math::RectAreaDbl> bounds, Bool localOnly)
{
	UTF8Char filePath[512];
	UTF8Char *sptr;
	UTF8Char *filePathEnd;
	IO::ParsedObject *pobj;
	Double resol = this->esriMap->TileGetLevelResolution(level);
	if (resol == 0)
		return 0;
	UOSInt tileWidth = this->esriMap->TileGetWidth();
	UOSInt tileHeight = this->esriMap->TileGetHeight();
	Math::Coord2DDbl origin = this->esriMap->TileGetOrigin();
	Double x1 = tileId.x * UOSInt2Double(tileWidth) * resol + origin.x;
	Double y1 = origin.y - tileId.y * UOSInt2Double(tileHeight) * resol;
	Double x2 = x1 + UOSInt2Double(tileWidth) * resol;
	Double y2 = y1 - UOSInt2Double(tileHeight) * resol;

	Math::RectAreaDbl b = Math::RectAreaDbl(Math::Coord2DDbl(x1, y1), Math::Coord2DDbl(x2, y2));
	bounds.Set(b);
	if (!b.OverlapOrTouch(this->esriMap->GetBounds()))
		return 0;


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
		IO::StmData::FileData fd({filePath, (UOSInt)(filePathEnd - filePath)}, false);
		if (fd.GetDataSize() > 0)
		{
			pobj = parsers->ParseFile(fd);
			if (pobj)
			{
				if (pobj->GetParserType() == IO::ParserType::ImageList)
				{
					return (Media::ImageList*)pobj;
				}
				DEL_CLASS(pobj);
			}
		}
	}

	if (localOnly)
		return 0;

	this->esriMap->TileLoadToFile(CSTRP(filePath, filePathEnd), level, tileId.x, tileId.y);
	IO::StmData::FileData fd({filePath, (UOSInt)(filePathEnd - filePath)}, false);
	if (fd.GetDataSize() > 0)
	{
		pobj = parsers->ParseFile(fd);
		if (pobj)
		{
			if (pobj->GetParserType() == IO::ParserType::ImageList)
			{
				return (Media::ImageList*)pobj;
			}
			DEL_CLASS(pobj);
		}
	}
	return 0;
}

UTF8Char *Map::ESRI::ESRITileMap::GetTileImageURL(UTF8Char *sbuff, UOSInt level, Math::Coord2D<Int32> tileId)
{
	return this->esriMap->TileGetURL(sbuff, level, tileId.x, tileId.y);
}

Bool Map::ESRI::ESRITileMap::GetTileImageURL(NN<Text::StringBuilderUTF8> sb, UOSInt level, Math::Coord2D<Int32> tileId)
{
	return this->esriMap->TileGetURL(sb, level, tileId.x, tileId.y);
}

Optional<IO::StreamData> Map::ESRI::ESRITileMap::LoadTileImageData(UOSInt level, Math::Coord2D<Int32> tileId, OutParam<Math::RectAreaDbl> bounds, Bool localOnly, OptOut<ImageType> it)
{
	UTF8Char filePath[512];
	UTF8Char *sptr;
	IO::StmData::FileData *fd;
	Double resol = this->esriMap->TileGetLevelResolution(level);
	if (resol == 0)
		return 0;
	UOSInt tileWidth = this->esriMap->TileGetWidth();
	UOSInt tileHeight = this->esriMap->TileGetHeight();
	Math::Coord2DDbl origin = this->esriMap->TileGetOrigin();
	Double x1 = tileId.x * UOSInt2Double(tileWidth) * resol + origin.x;
	Double y1 = origin.y - tileId.y * UOSInt2Double(tileHeight) * resol;
	Double x2 = x1 + UOSInt2Double(tileWidth) * resol;
	Double y2 = y1 - UOSInt2Double(tileHeight) * resol;

	Math::RectAreaDbl b = Math::RectAreaDbl(Math::Coord2DDbl(x1, y1), Math::Coord2DDbl(x2, y2));
	bounds.Set(b);
	if (!b.OverlapOrTouch(this->esriMap->GetBounds()))
		return 0;

	sptr = this->cacheDir->ConcatTo(filePath);
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
		*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrUOSInt(sptr, level);
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrInt32(sptr, tileId.y);
	IO::Path::CreateDirectory(CSTRP(filePath, sptr));
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrInt32(sptr, tileId.x);
	sptr = Text::StrConcatC(sptr, UTF8STRC(".dat"));
	NEW_CLASS(fd, IO::StmData::FileData({filePath, (UOSInt)(sptr - filePath)}, false));
	if (fd->GetDataSize() > 0)
	{
		it.Set(IT_PNG);
		return fd;
	}
	DEL_CLASS(fd);

	if (localOnly)
		return 0;

	this->esriMap->TileLoadToFile(CSTRP(filePath, sptr), level, tileId.x, tileId.y);
	NEW_CLASS(fd, IO::StmData::FileData({filePath, (UOSInt)(sptr - filePath)}, false));
	if (fd->GetDataSize() > 0)
	{
		it.Set(IT_PNG);
		return fd;
	}
	DEL_CLASS(fd);
	return 0;
}
