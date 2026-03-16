#include "Stdafx.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "Map/StmDataTileMap.h"
#include "Map/TileMapUtil.h"
#include "Math/ProjectedCoordinateSystem.h"

Optional<Map::StmDataTileMap::TileLayer> Map::StmDataTileMap::GetLayer(UIntOS level) const
{
	UIntOS i = 0;
	UIntOS j = this->layers.GetCount();
	while (i < j)
	{
		NN<TileLayer> layer = this->layers.GetItemNoCheck(i);
		if (layer->level == level)
		{
			return layer;
		}
		else if (layer->level > level)
		{
			return nullptr;
		}
		i++;
	}
	return nullptr;
}

Optional<Map::StmDataTileMap::TileInfo> Map::StmDataTileMap::GetTile(NN<TileLayer> layer, Math::Coord2D<Int32> tileId) const
{
	IntOS i = 0;
	IntOS j = (IntOS)layer->tiles.GetCount() - 1;
	IntOS k;
	while (i <= j)
	{
		k = (i + j) >> 1;
		NN<TileInfo> tile = layer->tiles.GetItemNoCheck((UIntOS)k);
		if (tile->tileId == tileId)
		{
			return tile;
		}
		else if (tile->tileId.y > tileId.y || (tile->tileId.y == tileId.y && tile->tileId.x > tileId.x))
		{
			j = k - 1;
		}
		else
		{
			i = k + 1;
		}
	}
	return nullptr;
}

Map::StmDataTileMap::StmDataTileMap(UIntOS tileSize, Math::Coord2DDbl tileOrigin, Math::RectAreaDbl bounds, NN<Math::CoordinateSystem> csys, TileFormat tileFormat, Text::CStringNN name)
{
	this->tileSize = tileSize;
	this->tileOrigin = tileOrigin;
	this->bounds = bounds;
	this->csys = csys;
	this->name = Text::String::New(name);
	this->tileFormat = tileFormat;
}

Map::StmDataTileMap::~StmDataTileMap()
{
	this->csys.Delete();
	this->name->Release();
}

Text::CStringNN Map::StmDataTileMap::GetName() const
{
	return this->name->ToCString();
}

Bool Map::StmDataTileMap::IsError() const
{
	return this->layers.GetCount() <= 0;
}

Map::TileMap::TileType Map::StmDataTileMap::GetTileType() const
{
	return TileType::StmData;
}

UIntOS Map::StmDataTileMap::GetMinLevel() const
{
	if (this->layers.GetCount() > 0)
	{
		return this->layers.GetItemNoCheck(0)->level;
	}
	return 0;
}

UIntOS Map::StmDataTileMap::GetMaxLevel() const
{
	if (this->layers.GetCount() > 0)
	{
		return this->layers.GetItemNoCheck(this->layers.GetCount() - 1)->level;
	}
	return 0;
}

Double Map::StmDataTileMap::GetLevelScale(UIntOS level) const
{
	NN<TileLayer> layer;
	if (this->GetLayer(level).SetTo(layer))
	{
		Double scaleDiv = Map::TileMapUtil::CalcScaleDiv(this->csys);
		return layer->resolution / scaleDiv;
	}
	return 0;
}

UIntOS Map::StmDataTileMap::GetNearestLevel(Double scale) const
{
	Double minDiff = 1.0E+100;
	UIntOS minLevel = 0;
	UIntOS i = this->layers.GetCount();
	NN<TileLayer> layer;
	Double layerScale;
	Double thisDiff;
	Double scaleDiv = Map::TileMapUtil::CalcScaleDiv(this->csys);
	while (i-- > 0)
	{
		layer = this->layers.GetItemNoCheck(i);
		layerScale = layer->resolution / scaleDiv;
		thisDiff = Math_Ln(scale / layerScale);
		if (thisDiff < 0)
		{
			thisDiff = -thisDiff;
		}
		if (thisDiff < minDiff)
		{
			minDiff = thisDiff;
			minLevel = i;
		}
	}
	return minLevel;

}

UIntOS Map::StmDataTileMap::GetConcurrentCount() const
{
	return 1;
}

Bool Map::StmDataTileMap::GetBounds(OutParam<Math::RectAreaDbl> bounds) const
{
	bounds.Set(this->bounds);
	return true;
}

NN<Math::CoordinateSystem> Map::StmDataTileMap::GetCoordinateSystem() const
{
	return this->csys;
}

Bool Map::StmDataTileMap::IsMercatorProj() const
{
	return this->csys->IsProjected() && NN<Math::ProjectedCoordinateSystem>::ConvertFrom(this->csys)->GetCoordSysType() == Math::CoordinateSystem::CoordinateSystemType::Mercator1SPProjected;
}

UIntOS Map::StmDataTileMap::GetTileSize() const
{
	return this->tileSize;
}

Map::TileMap::TileFormat Map::StmDataTileMap::GetTileFormat() const
{
	return this->tileFormat;
}

UIntOS Map::StmDataTileMap::GetTileImageIDs(UIntOS level, Math::RectAreaDbl rect, NN<Data::ArrayListT<Math::Coord2D<Int32>>> ids)
{
	NN<TileLayer> layer;
	if (!this->GetLayer(level).SetTo(layer))
	{
		return 0;
	}
	
	rect = rect.OverlapArea(this->bounds);
	UIntOS ret = 0;
	Int32 minX = (Int32)((rect.min.x - tileOrigin.x) / (layer->resolution * UIntOS2Double(tileSize)));
	Int32 maxX = (Int32)((rect.max.x - tileOrigin.x) / (layer->resolution * UIntOS2Double(tileSize)));
	Int32 minY = (Int32)((tileOrigin.y - rect.max.y) / (layer->resolution * UIntOS2Double(tileSize)));
	Int32 maxY = (Int32)((tileOrigin.y - rect.min.y) / (layer->resolution * UIntOS2Double(tileSize)));
	Int32 i = minY;
	Int32 j;
	while (i <= maxY)
	{
		j = minX;
		while (j <= maxX)
		{
			ids->Add(Math::Coord2D<Int32>(j, i));
			ret++;
			j++;
		}
		i++;
	}
	return ret;	
}

Optional<Media::ImageList> Map::StmDataTileMap::LoadTileImage(UIntOS level, Math::Coord2D<Int32> tileId, NN<Parser::ParserList> parsers, OutParam<Math::RectAreaDbl> bounds, Bool localOnly)
{
	NN<IO::StreamData> data;
	if (this->LoadTileImageData(level, tileId, bounds, localOnly, nullptr).SetTo(data))
	{
		NN<Media::ImageList> imgList;
		if (Optional<Media::ImageList>::ConvertFrom(parsers->ParseFile(data, nullptr, IO::ParserType::ImageList)).SetTo(imgList))
		{
			data.Delete();
			return imgList;
		}
		data.Delete();
	}
	return nullptr;
}

UnsafeArrayOpt<UTF8Char> Map::StmDataTileMap::GetTileImageURL(UnsafeArray<UTF8Char> sbuff, UIntOS level, Math::Coord2D<Int32> tileId)
{
	return nullptr;
}

Bool Map::StmDataTileMap::GetTileImageURL(NN<Text::StringBuilderUTF8> sb, UIntOS level, Math::Coord2D<Int32> tileId)
{
	return false;
}

Optional<IO::StreamData> Map::StmDataTileMap::LoadTileImageData(UIntOS level, Math::Coord2D<Int32> tileId, OutParam<Math::RectAreaDbl> bounds, Bool localOnly, OptOut<TileFormat> format)
{
	NN<TileLayer> layer;
	if (this->GetLayer(level).SetTo(layer))
	{
		NN<TileInfo> tile;
		if (this->GetTile(layer, tileId).SetTo(tile))
		{
			format.Set(this->tileFormat);
			Double x1 = tileId.x * layer->resolution * UIntOS2Double(tileSize) + tileOrigin.x;
			Double y1 = tileOrigin.y - tileId.y * layer->resolution * UIntOS2Double(tileSize);
			Double x2 = (tileId.x + 1) * layer->resolution * UIntOS2Double(tileSize) + tileOrigin.x;
			Double y2 = tileOrigin.y - (tileId.y + 1) * layer->resolution * UIntOS2Double(tileSize);
			bounds.Set(Math::RectAreaDbl(x1, y1, x2 - x1, y2 - y1));
			return tile->data->GetPartialData(0, tile->data->GetDataSize());
		}
	}
	return nullptr;
}

void Map::StmDataTileMap::AddLayer(UIntOS level, Double resolution)
{
	NN<TileLayer> layer;
	UIntOS i = 0;
	UIntOS j = this->layers.GetCount();
	while (i < j)
	{
		layer = this->layers.GetItemNoCheck(i);
		if (layer->level == level)
		{
			return;
		}
		else if (layer->level > level)
		{
			NEW_CLASSNN(layer, TileLayer());
			layer->level = level;
			layer->resolution = resolution;
			this->layers.Insert(i, layer);
			return;
		}
		i++;
	}
	NEW_CLASSNN(layer, TileLayer());
	layer->level = level;
	layer->resolution = resolution;
	this->layers.Add(layer);
}

void Map::StmDataTileMap::AddTile(UIntOS level, Math::Coord2D<Int32> tileId, NN<IO::StreamData> fd, UInt64 ofst, UInt64 size)
{
	NN<TileLayer> layer;
	if (this->GetLayer(level).SetTo(layer))
	{
		NN<TileInfo> tile;
		NEW_CLASSNN(tile, TileInfo());
		tile->tileId = tileId;
		tile->data = fd->GetPartialData(ofst, size);
		layer->tiles.Add(tile);
	}
}

void Map::StmDataTileMap::BuildIndex()
{
	TileLayerComparer comparer;
	UIntOS i = 0;
	UIntOS j = this->layers.GetCount();
	while (i < j)
	{
		NN<TileLayer> layer = this->layers.GetItemNoCheck(i);
		Data::Sort::ArtificialQuickSort::Sort<NN<TileInfo>>(layer->tiles, comparer);
		i++;
	}
}
