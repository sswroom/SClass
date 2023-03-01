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
#include "Math/Math.h"
#include "Math/Geometry/VectorImage.h"
#include "Media/SharedImage.h"

Map::OruxDBLayer::OruxDBLayer(Text::CString sourceName, Text::CString layerName, Parser::ParserList *parsers) : Map::MapDrawLayer(sourceName, 0, layerName)
{
	this->parsers = parsers;
	this->currLayer = (UInt32)-1;
	this->tileSize = 1;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = sourceName.ConcatTo(sbuff);
	this->db = 0;
	sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("OruxMapsImages.db"));
	IO::StmData::FileData *fd;
	NEW_CLASS(fd, IO::StmData::FileData(CSTRP(sbuff, sptr), false));
	DB::ReadingDB *db = (DB::ReadingDB*)parsers->ParseFileType(fd, IO::ParserType::ReadingDB);
	if (db)
	{
		if (db->IsFullConn())
		{
			this->db = (DB::DBConn*)db;
		}
		else
		{
			DEL_CLASS(db);
		}
	}
	DEL_CLASS(fd);
	this->SetCoordinateSystem(Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::CoordinateSystemManager::GCST_WGS84));
}

Map::OruxDBLayer::~OruxDBLayer()
{
	Map::OruxDBLayer::LayerInfo *lyr;
	UOSInt i;
	i = this->layerMap.GetCount();
	while (i-- > 0)
	{
		lyr = this->layerMap.GetItem(i);
		MemFreeA(lyr);
	}
	SDEL_CLASS(this->db);
}

Bool Map::OruxDBLayer::IsError()
{
	return this->db == 0;
}

void Map::OruxDBLayer::AddLayer(UInt32 layerId, Double mapXMin, Double mapYMin, Double mapXMax, Double mapYMax, UInt32 maxX, UInt32 maxY, UInt32 tileSize)
{
	Map::OruxDBLayer::LayerInfo *lyr = this->layerMap.Get(layerId);
	if (lyr == 0)
	{
		this->tileSize = tileSize;
		lyr = MemAllocA(Map::OruxDBLayer::LayerInfo, 1);
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

Map::MapView *Map::OruxDBLayer::CreateMapView(Math::Size2D<Double> scnSize)
{
	Map::MapView *view;
	Map::OruxDBLayer::LayerInfo *lyr = this->layerMap.Get(this->currLayer);
	if (lyr)
	{
		NEW_CLASS(view, Map::MercatorMapView(scnSize, (lyr->mapMax + lyr->mapMin) * 0.5, this->layerMap.GetCount(), this->tileSize));
		return view;
	}
	else
	{
		NEW_CLASS(view, Map::MercatorMapView(scnSize, Math::Coord2DDbl(114.2, 22.4), 18, 256));
		return view;
	}
}

Map::DrawLayerType Map::OruxDBLayer::GetLayerType()
{
	return Map::DRAW_LAYER_IMAGE;
}

UOSInt Map::OruxDBLayer::GetAllObjectIds(Data::ArrayListInt64 *outArr, NameArray **nameArr)
{
	Map::OruxDBLayer::LayerInfo *lyr = this->layerMap.Get(this->currLayer);
	if (lyr)
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

UOSInt Map::OruxDBLayer::GetObjectIds(Data::ArrayListInt64 *outArr, NameArray **nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty)
{
	return GetObjectIdsMapXY(outArr, nameArr, rect.ToDouble() / mapRate, keepEmpty);
}

UOSInt Map::OruxDBLayer::GetObjectIdsMapXY(Data::ArrayListInt64 *outArr, NameArray **nameArr, Math::RectAreaDbl rect, Bool keepEmpty)
{
	Int32 minX;
	Int32 minY;
	Int32 maxX;
	Int32 maxY;
	Int32 i;
	Int32 j;
	Math::Coord2DDbl diff;
	Map::OruxDBLayer::LayerInfo *lyr = this->layerMap.Get(this->currLayer);
	if (lyr)
	{
		rect = rect.Reorder();
		diff = (lyr->mapMax - lyr->mapMin) / lyr->max.ToDouble();
		minX = (Int32)((rect.tl.x - lyr->mapMin.x) / diff.x);
		minY = (Int32)((lyr->mapMax.y - rect.br.y) / diff.y);
		maxX = 1 + (Int32)((rect.br.x - lyr->mapMin.x) / diff.x);
		maxY = 1 + (Int32)((lyr->mapMax.y - rect.tl.y) / diff.y);
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
		return (UOSInt)(maxX - minX) * (UOSInt)(maxY - minY);
	}
	else
	{
		return 0;
	}
}

Int64 Map::OruxDBLayer::GetObjectIdMax()
{
	Map::OruxDBLayer::LayerInfo *lyr = this->layerMap.Get(this->currLayer);
	if (lyr)
	{
		return (((Int64)lyr->max.x - 1) << 32) | (UInt32)(lyr->max.y - 1);
	}
	else
	{
		return 0;
	}
}

void Map::OruxDBLayer::ReleaseNameArr(NameArray *nameArr)
{
}

UTF8Char *Map::OruxDBLayer::GetString(UTF8Char *buff, UOSInt buffSize, NameArray *nameArr, Int64 id, UOSInt strIndex)
{
	return 0;
}

UOSInt Map::OruxDBLayer::GetColumnCnt()
{
	return 0;
}

UTF8Char *Map::OruxDBLayer::GetColumnName(UTF8Char *buff, UOSInt colIndex)
{
	return 0;
}

DB::DBUtil::ColType Map::OruxDBLayer::GetColumnType(UOSInt colIndex, UOSInt *colSize)
{
	return DB::DBUtil::CT_Unknown;
}

Bool Map::OruxDBLayer::GetColumnDef(UOSInt colIndex, DB::ColDef *colDef)
{
	return false;
}

UInt32 Map::OruxDBLayer::GetCodePage()
{
	return 0;
}

Bool Map::OruxDBLayer::GetBounds(Math::RectAreaDbl *bounds)
{
	Map::OruxDBLayer::LayerInfo *lyr = this->layerMap.Get(this->currLayer);
	if (lyr)
	{
		*bounds = Math::RectAreaDbl(lyr->mapMin, lyr->mapMax);
		return lyr->mapMin.x != 0 || lyr->mapMin.y != 0 || lyr->mapMax.x != 0 || lyr->mapMax.y != 0;
	}
	else
	{
		*bounds = Math::RectAreaDbl(0, 0, 0, 0);
		return false;
	}
}

Map::GetObjectSess *Map::OruxDBLayer::BeginGetObject()
{
	return (GetObjectSess*)-1;
}

void Map::OruxDBLayer::EndGetObject(GetObjectSess *session)
{
}

Math::Geometry::Vector2D *Map::OruxDBLayer::GetNewVectorById(GetObjectSess *session, Int64 id)
{
	if (this->db == 0)
		return 0;
	Map::OruxDBLayer::LayerInfo *lyr = this->layerMap.Get(this->currLayer);
	if (lyr == 0)
		return 0;
	DB::SQLBuilder sql(DB::DBUtil::SQLType::SQLite, 0);
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
	DB::DBReader *r = this->db->ExecuteReader(sql.ToCString());
	if (r == 0)
		return 0;
	Media::ImageList *imgList = 0;
	if (r->ReadNext())
	{
		UOSInt size = r->GetBinarySize(0);
		UInt8 *buff = MemAlloc(UInt8, size);
		r->GetBinary(0, buff);
		IO::StmData::MemoryDataRef fd(buff, size);
		imgList = (Media::ImageList*)this->parsers->ParseFileType(&fd, IO::ParserType::ImageList);
		MemFree(buff);
	}
	this->db->CloseReader(r);
	if (imgList)
	{
		Media::SharedImage *shImg;
		Math::Geometry::VectorImage *vimg;
		Double x1;
		Double y1;
		Double y2;
		Double n;
		Double projY1;
		Double projY2;
		NEW_CLASS(shImg, Media::SharedImage(imgList, false));
		x1 = lyr->mapMin.x + (lyr->mapMax.x - lyr->mapMin.x) * x / lyr->max.x;
		projY1 = lyr->projYMax - (lyr->projYMax - lyr->projYMin) * y / lyr->max.y;
		projY2 = projY1 - (lyr->projYMax - lyr->projYMin) / lyr->max.y;
		n = Math::PI - 2.0 * Math::PI * projY1;
		y1 = 180.0 / Math::PI * Math_ArcTan(0.5 * (Math_Exp(n) - Math_Exp(-n)));
		n = Math::PI - 2.0 * Math::PI * projY2;
		y2 = 180.0 / Math::PI * Math_ArcTan(0.5 * (Math_Exp(n) - Math_Exp(-n)));
		NEW_CLASS(vimg, Math::Geometry::VectorImage(4326, shImg, Math::Coord2DDbl(x1, y2), Math::Coord2DDbl(x1 + (lyr->mapMax.x - lyr->mapMin.x) / lyr->max.x, y1), false, CSTR_NULL, 0, 0));
		DEL_CLASS(shImg);
		return vimg;
	}
	else
	{
		return 0;
	}
}

UOSInt Map::OruxDBLayer::QueryTableNames(Text::CString schemaName, Data::ArrayList<Text::String*> *names)
{
	return this->db->QueryTableNames(schemaName, names);
}

DB::DBReader *Map::OruxDBLayer::QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayList<Text::String*> *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition)
{
	return this->db->QueryTableData(schemaName, tableName, columnNames, ofst, maxCnt, ordering, condition);
}

DB::TableDef *Map::OruxDBLayer::GetTableDef(Text::CString schemaName, Text::CString tableName)
{
	return this->db->GetTableDef(schemaName, tableName);
}

void Map::OruxDBLayer::CloseReader(DB::DBReader *r)
{
	return this->db->CloseReader(r);
}

void Map::OruxDBLayer::GetErrorMsg(Text::StringBuilderUTF8 *str)
{
	this->db->GetErrorMsg(str);
}

void Map::OruxDBLayer::Reconnect()
{
	this->db->Reconnect();
}

Map::MapDrawLayer::ObjectClass Map::OruxDBLayer::GetObjectClass()
{
	return Map::MapDrawLayer::OC_ORUX_DB_LAYER;
}

Bool Map::OruxDBLayer::GetObjectData(Int64 objectId, IO::Stream *stm, Int32 *tileX, Int32 *tileY, Int64 *modTimeTicks)
{
	Map::OruxDBLayer::LayerInfo *lyr = this->layerMap.Get(this->currLayer);
	if (lyr == 0)
		return false;
	DB::SQLBuilder sql(DB::DBUtil::SQLType::SQLite, 0);
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
	DB::DBReader *r = this->db->ExecuteReader(sql.ToCString());
	if (r == 0)
		return false;
	Bool succ = false;
	if (r->ReadNext())
	{
		UOSInt size = r->GetBinarySize(0);
		UInt8 *buff = MemAlloc(UInt8, size);
		r->GetBinary(0, buff);
		stm->Write(buff, size);
		MemFree(buff);
		if (tileX)
			*tileX = x;
		if (tileY)
			*tileY = y;
		if (modTimeTicks)
			*modTimeTicks = 0;
		succ = true;
	}
	this->db->CloseReader(r);
	return succ;
}
