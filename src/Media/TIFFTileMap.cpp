#include "Stdafx.h"
#include "Math/CoordinateSystemManager.h"
#include "Media/TIFFTileMap.h"

Media::TIFFTileMap::TIFFTileMap(UOSInt tileWidth, UOSInt tileHeight)
{
	this->tileWidth = tileWidth;
	this->tileHeight = tileHeight;
	this->csys = Math::CoordinateSystemManager::CreateWGS84Csys();
}

Media::TIFFTileMap::~TIFFTileMap()
{
	this->csys.Delete();
}

Text::CStringNN Media::TIFFTileMap::GetName() const
{
	return CSTR("TIFFTile");
}

Bool Media::TIFFTileMap::IsError() const
{
	return false;
}

Map::TileMap::TileType Media::TIFFTileMap::GetTileType() const
{
	return Map::TileMap::TT_TIFF;
}

UOSInt Media::TIFFTileMap::GetMinLevel() const
{
	return 0;
}

UOSInt Media::TIFFTileMap::GetMaxLevel() const
{
	return this->layers.GetCount() - 1;
}

Double Media::TIFFTileMap::GetLevelScale(UOSInt level) const
{
	NN<LayerInfo> layer;
	if (this->layers.GetItem(level).SetTo(layer))
	{
		///////////////////////////////////
	}
	return 0;
}

UOSInt Media::TIFFTileMap::GetNearestLevel(Double scale) const
{
	///////////////////////////////////
	return 0;
}

UOSInt Media::TIFFTileMap::GetConcurrentCount() const
{
	return 1;
}

Bool Media::TIFFTileMap::GetBounds(OutParam<Math::RectAreaDbl> bounds) const
{
	///////////////////////////////////
	return false;
}

NN<Math::CoordinateSystem> Media::TIFFTileMap::GetCoordinateSystem() const
{
	return this->csys;
}

Bool Media::TIFFTileMap::IsMercatorProj() const
{
	return false;
}

UOSInt Media::TIFFTileMap::GetTileSize() const
{
	return this->tileWidth;
}

Map::TileMap::ImageType Media::TIFFTileMap::GetImageType() const
{
	return Map::TileMap::ImageType::IT_JPG;
}

Bool Media::TIFFTileMap::CanQuery() const
{
	return false;
}

Bool Media::TIFFTileMap::QueryInfos(Math::Coord2DDbl coord, UOSInt level, NN<Data::ArrayListNN<Math::Geometry::Vector2D>> vecList, NN<Data::ArrayList<UOSInt>> valueOfstList, NN<Data::ArrayListStringNN> nameList, NN<Data::ArrayListNN<Text::String>> valueList) const
{
	return false;
}

UOSInt Media::TIFFTileMap::GetTileImageIDs(UOSInt level, Math::RectAreaDbl rect, NN<Data::ArrayList<Math::Coord2D<Int32>>> ids)
{
	//////////////////////////////
	return 0;
}

Optional<Media::ImageList> Media::TIFFTileMap::LoadTileImage(UOSInt level, Math::Coord2D<Int32> tileId, NN<Parser::ParserList> parsers, OutParam<Math::RectAreaDbl> bounds, Bool localOnly)
{
	//////////////////////////////
	return 0;
}

UnsafeArrayOpt<UTF8Char> Media::TIFFTileMap::GetTileImageURL(UnsafeArray<UTF8Char> sbuff, UOSInt level, Math::Coord2D<Int32> tileId)
{
	return 0;
}

Bool Media::TIFFTileMap::GetTileImageURL(NN<Text::StringBuilderUTF8> sb, UOSInt level, Math::Coord2D<Int32> tileId)
{
	return false;
}

Optional<IO::StreamData> Media::TIFFTileMap::LoadTileImageData(UOSInt level, Math::Coord2D<Int32> tileId, OutParam<Math::RectAreaDbl> bounds, Bool localOnly, OptOut<ImageType> it)
{
	///////////////////////////////
	return 0;
}

UOSInt Media::TIFFTileMap::GetLayerCount() const
{
	return this->layers.GetCount();
}
