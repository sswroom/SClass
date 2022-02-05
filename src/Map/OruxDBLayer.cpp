#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/SQLBuilder.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "IO/StmData/MemoryData.h"
#include "Map/MercatorMapView.h"
#include "Map/OruxDBLayer.h"
#include "Map/OSM/OSMTileMap.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/GeographicCoordinateSystem.h"
#include "Math/Math.h"
#include "Math/VectorImage.h"
#include "Media/SharedImage.h"

Map::OruxDBLayer::OruxDBLayer(Text::CString sourceName, Text::CString layerName, Parser::ParserList *parsers) : Map::IMapDrawLayer(sourceName, 0, layerName)
{
	this->parsers = parsers;
	NEW_CLASS(this->layerMap, Data::UInt32Map<Map::OruxDBLayer::LayerInfo*>());
	this->currLayer = (UInt32)-1;
	this->tileSize = 1;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = sourceName.ConcatTo(sbuff);
	this->db = 0;
	sptr = IO::Path::AppendPathC(sbuff, sptr, UTF8STRC("OruxMapsImages.db"));
	IO::StmData::FileData *fd;
	NEW_CLASS(fd, IO::StmData::FileData({sbuff, (UOSInt)(sptr - sbuff)}, false));
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
	Data::ArrayList<Map::OruxDBLayer::LayerInfo*> *layerList;
	Map::OruxDBLayer::LayerInfo *lyr;
	layerList = this->layerMap->GetValues();
	UOSInt i;
	i = layerList->GetCount();
	while (i-- > 0)
	{
		lyr = layerList->GetItem(i);
		MemFree(lyr);
	}
	DEL_CLASS(this->layerMap);
	SDEL_CLASS(this->db);
}

Bool Map::OruxDBLayer::IsError()
{
	return this->db == 0;
}

void Map::OruxDBLayer::AddLayer(UInt32 layerId, Double mapXMin, Double mapYMin, Double mapXMax, Double mapYMax, UInt32 maxX, UInt32 maxY, UInt32 tileSize)
{
	Map::OruxDBLayer::LayerInfo *lyr = this->layerMap->Get(layerId);
	if (lyr == 0)
	{
		this->tileSize = tileSize;
		lyr = MemAlloc(Map::OruxDBLayer::LayerInfo, 1);
		lyr->layerId = layerId;
		lyr->mapXMin = mapXMin;
		lyr->mapYMin = mapYMin;
		lyr->mapXMax = mapXMax;
		lyr->mapYMax = mapYMax;
		lyr->projYMin = (1.0 - Math_Ln( Math_Tan(mapYMin * Math::PI / 180.0) + 1.0 / Math_Cos(mapYMin * Math::PI / 180.0)) / Math::PI) / 2.0;
		lyr->projYMax = (1.0 - Math_Ln( Math_Tan(mapYMax * Math::PI / 180.0) + 1.0 / Math_Cos(mapYMax * Math::PI / 180.0)) / Math::PI) / 2.0;
		lyr->maxX = maxX;
		lyr->maxY = maxY;
		this->layerMap->Put(layerId, lyr);
		if (this->currLayer == (UInt32)-1 || this->currLayer < layerId)
		{
			this->currLayer = layerId;
		}
	}
}

void Map::OruxDBLayer::SetCurrLayer(UInt32 level)
{
	if (level >= this->layerMap->GetCount())
		level = (UInt32)this->layerMap->GetCount() - 1;
	this->currLayer = level;
}

void Map::OruxDBLayer::SetCurrScale(Double scale)
{
	Int32 level = Double2Int32(Math_Log10(204094080000.0 / scale / this->tileSize) / Math_Log10(2));
	if (level < 0)
		level = 0;
	else if ((UInt32)level >= this->layerMap->GetCount())
		level = (Int32)this->layerMap->GetCount() - 1;
	this->currLayer = (UInt32)level;
}

Map::MapView *Map::OruxDBLayer::CreateMapView(UOSInt width, UOSInt height)
{
	Map::MapView *view;
	Map::OruxDBLayer::LayerInfo *lyr = this->layerMap->Get(this->currLayer);
	if (lyr)
	{
		NEW_CLASS(view, Map::MercatorMapView(width, height, (lyr->mapYMax + lyr->mapYMin) * 0.5, (lyr->mapXMax + lyr->mapXMin) * 0.5, this->layerMap->GetCount(), this->tileSize));
		return view;
	}
	else
	{
		NEW_CLASS(view, Map::MercatorMapView(width, height, 22.4, 114.2, 18, 256));
		return view;
	}
}

Map::DrawLayerType Map::OruxDBLayer::GetLayerType()
{
	return Map::DRAW_LAYER_IMAGE;
}

UOSInt Map::OruxDBLayer::GetAllObjectIds(Data::ArrayListInt64 *outArr, void **nameArr)
{
	Map::OruxDBLayer::LayerInfo *lyr = this->layerMap->Get(this->currLayer);
	if (lyr)
	{
		UInt32 i;
		UInt32 j;
		i = 0;
		while (i < lyr->maxX)
		{
			j = 0;
			while (j < lyr->maxY)
			{
				outArr->Add((Int64)((((UInt64)i) << 32) | j));
				j++;
			}
			i++;
		}
		return lyr->maxX * lyr->maxY;
	}
	else
	{
		return 0;
	}
}

UOSInt Map::OruxDBLayer::GetObjectIds(Data::ArrayListInt64 *outArr, void **nameArr, Double mapRate, Int32 x1, Int32 y1, Int32 x2, Int32 y2, Bool keepEmpty)
{
	return GetObjectIdsMapXY(outArr, nameArr, x1 / mapRate, y1 / mapRate, x2 / mapRate, y2 / mapRate, keepEmpty);
}

UOSInt Map::OruxDBLayer::GetObjectIdsMapXY(Data::ArrayListInt64 *outArr, void **nameArr, Double x1, Double y1, Double x2, Double y2, Bool keepEmpty)
{
	Int32 minX;
	Int32 minY;
	Int32 maxX;
	Int32 maxY;
	Int32 i;
	Int32 j;
	Double tmpV;
	Double xDiff;
	Double yDiff;
	Map::OruxDBLayer::LayerInfo *lyr = this->layerMap->Get(this->currLayer);
	if (lyr)
	{
		if (x1 > x2)
		{
			tmpV = x1;
			x1 = x2;
			x2 = tmpV;
		}
		if (y1 > y2)
		{
			tmpV = y1;
			y1 = y2;
			y2 = tmpV;
		}
		xDiff = (lyr->mapXMax - lyr->mapXMin) / lyr->maxX;
		yDiff = (lyr->mapYMax - lyr->mapYMin) / lyr->maxY;
		minX = (Int32)((x1 - lyr->mapXMin) / xDiff);
		minY = (Int32)((lyr->mapYMax - y2) / yDiff);
		maxX = 1 + (Int32)((x2 - lyr->mapXMin) / xDiff);
		maxY = 1 + (Int32)((lyr->mapYMax - y1) / yDiff);
		if (maxX < 0)
			return 0;
		if (maxY < 0)
			return 0;
		if (minX >= (Int32)lyr->maxX)
			return 0;
		if (minY >= (Int32)lyr->maxY)
			return 0;
		if (minX < 0)
			minX = 0;
		if (minY < 0)
			minY = 0;
		if (maxX > (Int32)lyr->maxX)
			maxX = (Int32)lyr->maxX;
		if (maxY > (Int32)lyr->maxY)
			maxY = (Int32)lyr->maxY;
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
	Map::OruxDBLayer::LayerInfo *lyr = this->layerMap->Get(this->currLayer);
	if (lyr)
	{
		return (((Int64)lyr->maxX - 1) << 32) | (UInt32)(lyr->maxY - 1);
	}
	else
	{
		return 0;
	}
}

void Map::OruxDBLayer::ReleaseNameArr(void *nameArr)
{
}

UTF8Char *Map::OruxDBLayer::GetString(UTF8Char *buff, UOSInt buffSize, void *nameArr, Int64 id, UOSInt strIndex)
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

Bool Map::OruxDBLayer::GetBoundsDbl(Double *minX, Double *minY, Double *maxX, Double *maxY)
{
	Map::OruxDBLayer::LayerInfo *lyr = this->layerMap->Get(this->currLayer);
	if (lyr)
	{
		*minX = lyr->mapXMin;
		*maxX = lyr->mapXMax;
		*minY = lyr->mapYMin;
		*maxY = lyr->mapYMax;
		return lyr->mapXMin != 0 || lyr->mapYMin != 0 || lyr->mapXMax != 0 || lyr->mapYMax != 0;
	}
	else
	{
		*minX = 0;
		*maxX = 0;
		*minY = 0;
		*maxY = 0;
		return false;
	}
}

void *Map::OruxDBLayer::BeginGetObject()
{
	return (void*)-1;
}

void Map::OruxDBLayer::EndGetObject(void *session)
{
}

Map::DrawObjectL *Map::OruxDBLayer::GetObjectByIdD(void *session, Int64 id)
{
	return 0;
}

Math::Vector2D *Map::OruxDBLayer::GetVectorById(void *session, Int64 id)
{
	if (this->db == 0)
		return 0;
	Map::OruxDBLayer::LayerInfo *lyr = this->layerMap->Get(this->currLayer);
	if (lyr == 0)
		return 0;
	DB::SQLBuilder sql(DB::DBUtil::ServerType::SQLite, 0);
	Int32 x;
	Int32 y;
	x = (Int32)((id >> 32) & 0xffffffff);
	y = (Int32)(id & 0xffffffff);
	sql.AppendCmdC(UTF8STRC("select image from tiles where x = "));
	sql.AppendInt32(x);
	sql.AppendCmdC(UTF8STRC(" and y = "));
	sql.AppendInt32(y);
	sql.AppendCmdC(UTF8STRC(" and z = "));
	sql.AppendInt32((Int32)this->currLayer);
	DB::DBReader *r = this->db->ExecuteReaderC(sql.ToString(), sql.GetLength());
	if (r == 0)
		return 0;
	Media::ImageList *imgList = 0;
	if (r->ReadNext())
	{
		UOSInt size = r->GetBinarySize(0);
		UInt8 *buff = MemAlloc(UInt8, size);
		r->GetBinary(0, buff);
		IO::StmData::MemoryData *fd;
		NEW_CLASS(fd, IO::StmData::MemoryData(buff, size));
		imgList = (Media::ImageList*)this->parsers->ParseFileType(fd, IO::ParserType::ImageList);
		DEL_CLASS(fd);
		MemFree(buff);
	}
	this->db->CloseReader(r);
	if (imgList)
	{
		Media::SharedImage *shImg;
		Math::VectorImage *vimg;
		Double x1;
		Double y1;
		Double y2;
		Double n;
		Double projY1;
		Double projY2;
		NEW_CLASS(shImg, Media::SharedImage(imgList, false));
		x1 = lyr->mapXMin + (lyr->mapXMax - lyr->mapXMin) * x / lyr->maxX;
		projY1 = lyr->projYMax - (lyr->projYMax - lyr->projYMin) * y / lyr->maxY;
		projY2 = projY1 - (lyr->projYMax - lyr->projYMin) / lyr->maxY;
		n = Math::PI - 2.0 * Math::PI * projY1;
		y1 = 180.0 / Math::PI * Math_ArcTan(0.5 * (Math_Exp(n) - Math_Exp(-n)));
		n = Math::PI - 2.0 * Math::PI * projY2;
		y2 = 180.0 / Math::PI * Math_ArcTan(0.5 * (Math_Exp(n) - Math_Exp(-n)));
		NEW_CLASS(vimg, Math::VectorImage(4326, shImg, x1, y2, x1 + (lyr->mapXMax - lyr->mapXMin) / lyr->maxX, y1, false, CSTR_NULL, 0, 0));
		DEL_CLASS(shImg);
		return vimg;
	}
	else
	{
		return 0;
	}
}

void Map::OruxDBLayer::ReleaseObject(void *session, DrawObjectL *obj)
{
}

UOSInt Map::OruxDBLayer::GetTableNames(Data::ArrayList<const UTF8Char*> *names)
{
	return this->db->GetTableNames(names);
}

DB::DBReader *Map::OruxDBLayer::GetTableData(const UTF8Char *tableName, Data::ArrayList<Text::String*> *columnNames, UOSInt ofst, UOSInt maxCnt, const UTF8Char *ordering, Data::QueryConditions *condition)
{
	return this->db->GetTableData(tableName, columnNames, ofst, maxCnt, ordering, condition);
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

Map::IMapDrawLayer::ObjectClass Map::OruxDBLayer::GetObjectClass()
{
	return Map::IMapDrawLayer::OC_ORUX_DB_LAYER;
}

Bool Map::OruxDBLayer::GetObjectData(Int64 objectId, IO::Stream *stm, Int32 *tileX, Int32 *tileY, Int64 *modTimeTicks)
{
	Map::OruxDBLayer::LayerInfo *lyr = this->layerMap->Get(this->currLayer);
	if (lyr == 0)
		return false;
	DB::SQLBuilder sql(DB::DBUtil::ServerType::SQLite, 0);
	Int32 x;
	Int32 y;
	x = (Int32)((objectId >> 32) & 0xffffffff);
	y = (Int32)(objectId & 0xffffffff);
	sql.AppendCmdC(UTF8STRC("select image from tiles where x = "));
	sql.AppendInt32(x);
	sql.AppendCmdC(UTF8STRC(" and y = "));
	sql.AppendInt32(y);
	sql.AppendCmdC(UTF8STRC(" and z = "));
	sql.AppendInt32((Int32)this->currLayer);
	DB::DBReader *r = this->db->ExecuteReaderC(sql.ToString(), sql.GetLength());
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
