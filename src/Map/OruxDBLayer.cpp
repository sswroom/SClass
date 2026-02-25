#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/SQLBuilder.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "IO/StmData/MemoryDataRef.h"
#include "Map/MercatorMapView.h"
#include "Map/OruxDBLayer.h"
#include "Map/OSM/OSMTileMap.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/GeographicCoordinateSystem.h"
#include "Math/Math_C.h"
#include "Math/Geometry/VectorImage.h"
#include "Media/SharedImage.h"

Map::OruxDBLayer::OruxDBLayer(Text::CStringNN sourceName, Text::CString layerName, NN<Parser::ParserList> parsers) : Map::MapDrawLayer(sourceName, 0, layerName, Math::CoordinateSystemManager::CreateWGS84Csys())
{
	this->parsers = parsers;
	this->currLayer = (UInt32)-1;
	this->tileSize = 1;
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	sptr = sourceName.ConcatTo(sbuff);
	this->db = nullptr;
	sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("OruxMapsImages.db"));
	{
		IO::StmData::FileData fd(CSTRP(sbuff, sptr), false);
		NN<DB::ReadingDB> db;
		if (Optional<DB::ReadingDB>::ConvertFrom(parsers->ParseFileType(fd, IO::ParserType::ReadingDB)).SetTo(db))
		{
			if (db->IsFullConn())
			{
				this->db = NN<DB::DBConn>::ConvertFrom(db);
			}
			else
			{
				db.Delete();
			}
		}
	}
}

Map::OruxDBLayer::~OruxDBLayer()
{
	NN<Map::OruxDBLayer::LayerInfo> lyr;
	UIntOS i;
	i = this->layerMap.GetCount();
	while (i-- > 0)
	{
		lyr = this->layerMap.GetItemNoCheck(i);
		MemFreeANN(lyr);
	}
	this->db.Delete();
}

Bool Map::OruxDBLayer::IsError() const
{
	return this->db.IsNull();
}

void Map::OruxDBLayer::AddLayer(UInt32 layerId, Double mapXMin, Double mapYMin, Double mapXMax, Double mapYMax, UInt32 maxX, UInt32 maxY, UInt32 tileSize)
{
	NN<Map::OruxDBLayer::LayerInfo> lyr;
	if (!this->layerMap.ContainsKey(layerId))
	{
		this->tileSize = tileSize;
		lyr = MemAllocANN(Map::OruxDBLayer::LayerInfo);
		lyr->layerId = layerId;
		lyr->mapMin.x = mapXMin;
		lyr->mapMin.y = mapYMin;
		lyr->mapMax.x = mapXMax;
		lyr->mapMax.y = mapYMax;
		lyr->projYMin = (1.0 - Math_Ln( Math_Tan(mapYMin * Math::PI / 180.0) + 1.0 / Math_Cos(mapYMin * Math::PI / 180.0)) / Math::PI) / 2.0;
		lyr->projYMax = (1.0 - Math_Ln( Math_Tan(mapYMax * Math::PI / 180.0) + 1.0 / Math_Cos(mapYMax * Math::PI / 180.0)) / Math::PI) / 2.0;
		lyr->max.x = maxX;
		lyr->max.y = maxY;
		this->layerMap.Put(layerId, lyr);
		if (this->currLayer == (UInt32)-1 || this->currLayer < layerId)
		{
			this->currLayer = layerId;
		}
	}
}

void Map::OruxDBLayer::SetCurrLayer(UInt32 level)
{
	if (level >= this->layerMap.GetCount())
		level = (UInt32)this->layerMap.GetCount() - 1;
	this->currLayer = level;
}

void Map::OruxDBLayer::SetCurrScale(Double scale)
{
	Int32 level = Double2Int32(Math_Log10(204094080000.0 / scale / this->tileSize) / Math_Log10(2));
	if (level < 0)
		level = 0;
	else if ((UInt32)level >= this->layerMap.GetCount())
		level = (Int32)this->layerMap.GetCount() - 1;
	this->currLayer = (UInt32)level;
}

NN<Map::MapView> Map::OruxDBLayer::CreateMapView(Math::Size2DDbl scnSize)
{
	NN<Map::MapView> view;
	NN<Map::OruxDBLayer::LayerInfo> lyr;
	if (this->layerMap.Get(this->currLayer).SetTo(lyr))
	{
		NEW_CLASSNN(view, Map::MercatorMapView(scnSize, (lyr->mapMax + lyr->mapMin) * 0.5, this->layerMap.GetCount(), this->tileSize));
		return view;
	}
	else
	{
		NEW_CLASSNN(view, Map::MercatorMapView(scnSize, Math::Coord2DDbl(114.2, 22.4), 18, 256));
		return view;
	}
}

Map::DrawLayerType Map::OruxDBLayer::GetLayerType() const
{
	return Map::DRAW_LAYER_IMAGE;
}

UIntOS Map::OruxDBLayer::GetAllObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr)
{
	NN<Map::OruxDBLayer::LayerInfo> lyr;
	if (this->layerMap.Get(this->currLayer).SetTo(lyr))
	{
		UInt32 i;
		UInt32 j;
		i = 0;
		while (i < lyr->max.x)
		{
			j = 0;
			while (j < lyr->max.y)
			{
				outArr->Add((Int64)((((UInt64)i) << 32) | j));
				j++;
			}
			i++;
		}
		return lyr->max.x * lyr->max.y;
	}
	else
	{
		return 0;
	}
}

UIntOS Map::OruxDBLayer::GetObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty)
{
	return GetObjectIdsMapXY(outArr, nameArr, rect.ToDouble() / mapRate, keepEmpty);
}

UIntOS Map::OruxDBLayer::GetObjectIdsMapXY(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr, Math::RectAreaDbl rect, Bool keepEmpty)
{
	Int32 minX;
	Int32 minY;
	Int32 maxX;
	Int32 maxY;
	Int32 i;
	Int32 j;
	Math::Coord2DDbl diff;
	NN<Map::OruxDBLayer::LayerInfo> lyr;
	if (this->layerMap.Get(this->currLayer).SetTo(lyr))
	{
		rect = rect.Reorder();
		diff = (lyr->mapMax - lyr->mapMin) / lyr->max.ToDouble();
		minX = (Int32)((rect.min.x - lyr->mapMin.x) / diff.x);
		minY = (Int32)((lyr->mapMax.y - rect.max.y) / diff.y);
		maxX = 1 + (Int32)((rect.max.x - lyr->mapMin.x) / diff.x);
		maxY = 1 + (Int32)((lyr->mapMax.y - rect.min.y) / diff.y);
		if (maxX < 0)
			return 0;
		if (maxY < 0)
			return 0;
		if (minX >= (Int32)lyr->max.x)
			return 0;
		if (minY >= (Int32)lyr->max.y)
			return 0;
		if (minX < 0)
			minX = 0;
		if (minY < 0)
			minY = 0;
		if (maxX > (Int32)lyr->max.x)
			maxX = (Int32)lyr->max.x;
		if (maxY > (Int32)lyr->max.y)
			maxY = (Int32)lyr->max.y;
		i = minX;
		while (i < maxX)
		{
			j = minY;
			while (j < maxY)
			{
				outArr->Add((((Int64)i) << 32) | (UInt32)j);
				j++;
			}
			i++;
		}
		return (UIntOS)(maxX - minX) * (UIntOS)(maxY - minY);
	}
	else
	{
		return 0;
	}
}

Int64 Map::OruxDBLayer::GetObjectIdMax() const
{
	NN<Map::OruxDBLayer::LayerInfo> lyr;
	if (this->layerMap.Get(this->currLayer).SetTo(lyr))
	{
		return (((Int64)lyr->max.x - 1) << 32) | (UInt32)(lyr->max.y - 1);
	}
	else
	{
		return -1;
	}
}

UIntOS Map::OruxDBLayer::GetRecordCnt() const
{
	NN<Map::OruxDBLayer::LayerInfo> lyr;
	if (this->layerMap.Get(this->currLayer).SetTo(lyr))
	{
		return lyr->max.x * lyr->max.y;
	}
	else
	{
		return 0;
	}
}

void Map::OruxDBLayer::ReleaseNameArr(Optional<NameArray> nameArr)
{
}

Bool Map::OruxDBLayer::GetString(NN<Text::StringBuilderUTF8> sb, Optional<NameArray> nameArr, Int64 id, UIntOS strIndex)
{
	return false;
}

UIntOS Map::OruxDBLayer::GetColumnCnt() const
{
	return 0;
}

UnsafeArrayOpt<UTF8Char> Map::OruxDBLayer::GetColumnName(UnsafeArray<UTF8Char> buff, UIntOS colIndex) const
{
	return nullptr;
}

DB::DBUtil::ColType Map::OruxDBLayer::GetColumnType(UIntOS colIndex, OptOut<UIntOS> colSize) const
{
	return DB::DBUtil::CT_Unknown;
}

Bool Map::OruxDBLayer::GetColumnDef(UIntOS colIndex, NN<DB::ColDef> colDef) const
{
	return false;
}

UInt32 Map::OruxDBLayer::GetCodePage() const
{
	return 0;
}

Bool Map::OruxDBLayer::GetBounds(OutParam<Math::RectAreaDbl> bounds) const
{
	NN<Map::OruxDBLayer::LayerInfo> lyr;
	if (this->layerMap.Get(this->currLayer).SetTo(lyr))
	{
		bounds.Set(Math::RectAreaDbl(lyr->mapMin, lyr->mapMax));
		return lyr->mapMin.x != 0 || lyr->mapMin.y != 0 || lyr->mapMax.x != 0 || lyr->mapMax.y != 0;
	}
	else
	{
		bounds.Set(Math::RectAreaDbl(0, 0, 0, 0));
		return false;
	}
}

NN<Map::GetObjectSess> Map::OruxDBLayer::BeginGetObject()
{
	return NN<GetObjectSess>::ConvertFrom(NN<OruxDBLayer>(*this));
}

void Map::OruxDBLayer::EndGetObject(NN<GetObjectSess> session)
{
}

Optional<Math::Geometry::Vector2D> Map::OruxDBLayer::GetNewVectorById(NN<GetObjectSess> session, Int64 id)
{
	NN<DB::DBConn> db;
	if (!this->db.SetTo(db))
		return nullptr;
	NN<Map::OruxDBLayer::LayerInfo> lyr;
	if (!this->layerMap.Get(this->currLayer).SetTo(lyr))
		return nullptr;
	DB::SQLBuilder sql(DB::SQLType::SQLite, false, 0);
	Int32 x;
	Int32 y;
	x = (Int32)((id >> 32) & 0xffffffff);
	y = (Int32)(id & 0xffffffff);
	sql.AppendCmdC(CSTR("select image from tiles where x = "));
	sql.AppendInt32(x);
	sql.AppendCmdC(CSTR(" and y = "));
	sql.AppendInt32(y);
	sql.AppendCmdC(CSTR(" and z = "));
	sql.AppendInt32((Int32)this->currLayer);
	NN<DB::DBReader> r;
	if (!db->ExecuteReader(sql.ToCString()).SetTo(r))
		return nullptr;
	Optional<Media::ImageList> imgList = nullptr;
	if (r->ReadNext())
	{
		UIntOS size = r->GetBinarySize(0);
		UInt8 *buff = MemAlloc(UInt8, size);
		r->GetBinary(0, buff);
		IO::StmData::MemoryDataRef fd(buff, size);
		imgList = Optional<Media::ImageList>::ConvertFrom(this->parsers->ParseFileType(fd, IO::ParserType::ImageList));
		MemFree(buff);
	}
	db->CloseReader(r);
	NN<Media::ImageList> nnimgList;
	if (imgList.SetTo(nnimgList))
	{
		NN<Media::SharedImage> shImg;
		Math::Geometry::VectorImage *vimg;
		Double x1;
		Double y1;
		Double y2;
		Double n;
		Double projY1;
		Double projY2;
		NEW_CLASSNN(shImg, Media::SharedImage(nnimgList, nullptr));
		x1 = lyr->mapMin.x + (lyr->mapMax.x - lyr->mapMin.x) * x / lyr->max.x;
		projY1 = lyr->projYMax - (lyr->projYMax - lyr->projYMin) * y / lyr->max.y;
		projY2 = projY1 - (lyr->projYMax - lyr->projYMin) / lyr->max.y;
		n = Math::PI - 2.0 * Math::PI * projY1;
		y1 = 180.0 / Math::PI * Math_ArcTan(0.5 * (Math_Exp(n) - Math_Exp(-n)));
		n = Math::PI - 2.0 * Math::PI * projY2;
		y2 = 180.0 / Math::PI * Math_ArcTan(0.5 * (Math_Exp(n) - Math_Exp(-n)));
		NEW_CLASS(vimg, Math::Geometry::VectorImage(4326, shImg, Math::Coord2DDbl(x1, y2), Math::Coord2DDbl(x1 + (lyr->mapMax.x - lyr->mapMin.x) / lyr->max.x, y1), Math::Coord2DDbl((lyr->mapMax.x - lyr->mapMin.x) / lyr->max.x, (y1 - y2)), false, Text::CString(nullptr), 0, 0));
		shImg.Delete();
		return vimg;
	}
	else
	{
		return nullptr;
	}
}

Map::MapDrawLayer::FailReason Map::OruxDBLayer::GetFailReason() const
{
	return Map::MapDrawLayer::FailReason::IdNotFound;
}

UIntOS Map::OruxDBLayer::QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names)
{
	NN<DB::DBConn> db;
	if (this->db.SetTo(db))
	{
		return db->QueryTableNames(schemaName, names);
	}
	return 0;
}

Optional<DB::DBReader> Map::OruxDBLayer::QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> columnNames, UIntOS ofst, UIntOS maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition)
{
	NN<DB::DBConn> db;
	if (this->db.SetTo(db))
	{
		return db->QueryTableData(schemaName, tableName, columnNames, ofst, maxCnt, ordering, condition);
	}
	return nullptr;
}

Optional<DB::TableDef> Map::OruxDBLayer::GetTableDef(Text::CString schemaName, Text::CStringNN tableName)
{
	NN<DB::DBConn> db;
	if (this->db.SetTo(db))
	{
		return db->GetTableDef(schemaName, tableName);
	}
	return nullptr;
}

void Map::OruxDBLayer::CloseReader(NN<DB::DBReader> r)
{
	NN<DB::DBConn> db;
	if (this->db.SetTo(db))
	{
		db->CloseReader(r);
	}
}

void Map::OruxDBLayer::GetLastErrorMsg(NN<Text::StringBuilderUTF8> str)
{
	NN<DB::DBConn> db;
	if (this->db.SetTo(db))
	{
		db->GetLastErrorMsg(str);
	}
}

void Map::OruxDBLayer::Reconnect()
{
	NN<DB::DBConn> db;
	if (this->db.SetTo(db))
	{
		db->Reconnect();
	}
}

UIntOS Map::OruxDBLayer::GetGeomCol() const
{
	return INVALID_INDEX;
}

Map::MapDrawLayer::ObjectClass Map::OruxDBLayer::GetObjectClass() const
{
	return Map::MapDrawLayer::OC_ORUX_DB_LAYER;
}

Bool Map::OruxDBLayer::GetObjectData(Int64 objectId, NN<IO::Stream> stm, OptOut<Int32> tileX, OptOut<Int32> tileY, OptOut<Int64> modTimeTicks)
{
	NN<DB::DBConn> db;
	NN<Map::OruxDBLayer::LayerInfo> lyr;
	if (!this->layerMap.Get(this->currLayer).SetTo(lyr) || !this->db.SetTo(db))
		return false;
	DB::SQLBuilder sql(DB::SQLType::SQLite, false, 0);
	Int32 x;
	Int32 y;
	x = (Int32)((objectId >> 32) & 0xffffffff);
	y = (Int32)(objectId & 0xffffffff);
	sql.AppendCmdC(CSTR("select image from tiles where x = "));
	sql.AppendInt32(x);
	sql.AppendCmdC(CSTR(" and y = "));
	sql.AppendInt32(y);
	sql.AppendCmdC(CSTR(" and z = "));
	sql.AppendInt32((Int32)this->currLayer);
	NN<DB::DBReader> r;
	if (!db->ExecuteReader(sql.ToCString()).SetTo(r))
		return false;
	Bool succ = false;
	if (r->ReadNext())
	{
		UIntOS size = r->GetBinarySize(0);
		UInt8 *buff = MemAlloc(UInt8, size);
		r->GetBinary(0, buff);
		stm->Write(Data::ByteArrayR(buff, size));
		MemFree(buff);
		tileX.Set(x);
		tileY.Set(y);
		modTimeTicks.Set(0);
		succ = true;
	}
	db->CloseReader(r);
	return succ;
}
