#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/Path.h"
#include "Map/SHPData.h"
#include "Math/ArcGISPRJParser.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Math.h"
#include "Math/Geometry/Polyline.h"
#include "Math/Geometry/Polygon.h"
#include "Math/Geometry/Point.h"
#include "Math/Geometry/PointZ.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"

Map::SHPData::SHPData(const UInt8 *shpHdr, NN<IO::StreamData> data, UInt32 codePage, NN<Math::ArcGISPRJParser> prjParser) : Map::MapDrawLayer(data->GetFullName(), 0, 0, Math::CoordinateSystemManager::CreateWGS84Csys())
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	UInt8 shpBuff[100];
	Int32 valid;
	UInt32 fileLen;
//	Int32 shpType;
	UOSInt i;
	Map::SHPData::RecHdr *rec;

	this->dbf = 0;
	this->shpData = 0;
	this->ptX = 0;
	this->ptY = 0;
	this->ptZ = 0;
	this->recs = 0;
	this->recsMut = 0;
	this->layerType = Map::DRAW_LAYER_UNKNOWN;
	this->mapRate = 10000000.0;

	sptr = data->GetFullFileName()->ConcatTo(sbuff);
	if ((sptr - sbuff) < 4)
	{
		return;
	}
	i = Text::StrLastIndexOfC(sbuff, (UOSInt)(sptr - sbuff), IO::Path::PATH_SEPERATOR);
	if (sptr[-4] == '.')
	{
		sptr[-4] = 0;
		this->SetLayerName(CSTRP(&sbuff[i + 1], &sptr[-4]));
		sptr[-4] = '.';
		Text::StrConcatC(&sptr[-3], UTF8STRC("prj"));
	}
	else
	{
		this->SetLayerName(CSTRP(&sbuff[i + 1], sptr));
		sptr = Text::StrConcatC(sptr, UTF8STRC(".prj"));
	}
	NN<Math::CoordinateSystem> csys;
	if (csys.Set(prjParser->ParsePRJFile({sbuff, (UOSInt)(sptr - sbuff)})))
	{
		this->SetCoordinateSystem(csys);	
	}

	Text::StrConcatC(&sptr[-3], UTF8STRC("dbf"));

	if (ReadMInt32(shpHdr) != 9994 || ReadInt32(&shpHdr[28]) != 1000 || (ReadMUInt32(&shpHdr[24]) << 1) != data->GetDataSize())
	{
		valid = 0;
	}
	else
	{
		valid = 1;
	}
	if (valid == 0)
	{
		return;
	}
	this->isPoint = false;
	this->shpData = data->GetPartialData(0, data->GetDataSize()).Ptr();
	this->min.x = ReadDouble(&shpHdr[36]);
	this->min.y = ReadDouble(&shpHdr[44]);
	this->max.x = ReadDouble(&shpHdr[52]);
	this->max.y = ReadDouble(&shpHdr[60]);

	if (max.x > 200000000 || min.x < -200000000 || max.y > 200000000 || min.y < -200000000)
	{
		this->mapRate = 1.0;
	}
	else if (max.x > 20000000 || min.x < -20000000 || max.y > 20000000 || min.y < -20000000)
	{
		this->mapRate = 10.0;
	}
	else if (max.x > 2000000 || min.x < -2000000 || max.y > 2000000 || min.y < -2000000)
	{
		this->mapRate = 100.0;
	}
	else if (max.x > 200000 || min.x < -200000 || max.y > 200000 || min.y < -200000)
	{
		this->mapRate = 1000.0;
	}
	else if (max.x > 20000 || min.x < -20000 || max.y > 20000 || min.y < -20000)
	{
		this->mapRate = 10000.0;
	}
	else if (max.x > 2000 || min.x < -2000 || max.y > 2000 || min.y < -2000)
	{
		this->mapRate = 100000.0;
	}
	else if (max.x > 200 || min.x < -200 || max.y > 200 || min.y < -200)
	{
		this->mapRate = 1000000.0;
	}
	else
	{
		this->mapRate = 10000000.0;
	}

	UInt64 currOfst = 100;
	if (*(Int32*)&shpHdr[32] == 1)
	{
		this->layerType = Map::DRAW_LAYER_POINT;
		NEW_CLASS(ptX, Data::ArrayListDbl());
		NEW_CLASS(ptY, Data::ArrayListDbl());
		while (data->GetRealData(currOfst, 8, BYTEARR(shpBuff)) == 8)
		{
			currOfst += 8;
			fileLen = ReadMUInt32(&shpBuff[4]);

			data->GetRealData(currOfst, fileLen << 1, BYTEARR(shpBuff));
			currOfst += fileLen << 1;
			if (*(Int32*)shpBuff == 1)
			{
				ptX->Add(ReadDouble(&shpBuff[4]));
				ptY->Add(ReadDouble(&shpBuff[12]));
			}
			else
			{
				ptX->Add(0);
				ptY->Add(0);
			}
		}
		this->isPoint = true;
	}
	else if (*(Int32*)&shpHdr[32] == 3)
	{
		this->layerType = Map::DRAW_LAYER_POLYLINE;
		NEW_CLASS(this->recs, Data::ArrayList<Optional<Map::SHPData::RecHdr>>());
		NEW_CLASS(this->recsMut, Sync::Mutex());
		while (data->GetRealData(currOfst, 8, BYTEARR(shpBuff)) == 8)
		{
			currOfst += 8;
			fileLen = ReadMUInt32(&shpBuff[4]);

			if (fileLen > 22)
			{
				data->GetRealData(currOfst, 44, BYTEARR(shpBuff));
				if (*(Int32*)shpBuff == 3)
				{
					rec = MemAlloc(Map::SHPData::RecHdr, 1);
					rec->x1 = ReadDouble(&shpBuff[4]);
					rec->y1 = ReadDouble(&shpBuff[12]);
					rec->x2 = ReadDouble(&shpBuff[20]);
					rec->y2 = ReadDouble(&shpBuff[28]);
					rec->vec = 0;
					rec->nPoint = ReadUInt32(&shpBuff[40]);
					rec->nPtOfst = ReadUInt32(&shpBuff[36]);
					rec->ofst = (UInt32)(currOfst + 44);
					this->recs->Add(rec);
				}
				else
				{
					this->recs->Add(0);
				}
				currOfst += fileLen << 1;
			}
			else
			{
				currOfst += fileLen << 1;
				this->recs->Add(0);
			}
		}		
	}
	else if (*(Int32*)&shpHdr[32] == 5)
	{
		this->layerType = Map::DRAW_LAYER_POLYGON;
		NEW_CLASS(this->recs, Data::ArrayList<Optional<Map::SHPData::RecHdr>>());
		NEW_CLASS(this->recsMut, Sync::Mutex());
		while (data->GetRealData(currOfst, 8, BYTEARR(shpBuff)) == 8)
		{
			currOfst += 8;
			fileLen = ReadMUInt32(&shpBuff[4]);

			if (fileLen > 22)
			{
				data->GetRealData(currOfst, 44, BYTEARR(shpBuff));
				if (*(Int32*)shpBuff == 5)
				{
					rec = MemAlloc(Map::SHPData::RecHdr, 1);
					rec->x1 = ReadDouble(&shpBuff[4]);
					rec->y1 = ReadDouble(&shpBuff[12]);
					rec->x2 = ReadDouble(&shpBuff[20]);
					rec->y2 = ReadDouble(&shpBuff[28]);
					rec->vec = 0;
					rec->nPoint = ReadUInt32(&shpBuff[40]);
					rec->nPtOfst = ReadUInt32(&shpBuff[36]);
					rec->ofst = (UInt32)(currOfst + 44);
					this->recs->Add(rec);
				}
				else
				{
					this->recs->Add(0);
				}
				currOfst += fileLen << 1;
			}
			else
			{
				currOfst += fileLen << 1;
				this->recs->Add(0);
			}
		}		
	}
	else if (*(Int32*)&shpHdr[32] == 11)
	{
		this->layerType = Map::DRAW_LAYER_POINT3D;
		NEW_CLASS(ptX, Data::ArrayListDbl());
		NEW_CLASS(ptY, Data::ArrayListDbl());
		NEW_CLASS(ptZ, Data::ArrayListDbl());
		while (data->GetRealData(currOfst, 8, BYTEARR(shpBuff)) == 8)
		{
			currOfst += 8;
			fileLen = ReadMUInt32(&shpBuff[4]);

			data->GetRealData(currOfst, fileLen << 1, BYTEARR(shpBuff));
			currOfst += fileLen << 1;
			if (*(Int32*)shpBuff == 11)
			{
				ptX->Add(ReadDouble(&shpBuff[4]));
				ptY->Add(ReadDouble(&shpBuff[12]));
				ptZ->Add(ReadDouble(&shpBuff[20]));
			}
			else
			{
				ptX->Add(0);
				ptY->Add(0);
				ptZ->Add(0);
			}
		}
		this->isPoint = true;
	}
	else if (*(Int32*)&shpHdr[32] == 13)
	{
		this->layerType = Map::DRAW_LAYER_POLYLINE3D;
		NEW_CLASS(this->recs, Data::ArrayList<Optional<Map::SHPData::RecHdr>>());
		NEW_CLASS(this->recsMut, Sync::Mutex());
		while (data->GetRealData(currOfst, 8, BYTEARR(shpBuff)) == 8)
		{
			currOfst += 8;
			fileLen = ReadMUInt32(&shpBuff[4]);

			if (fileLen > 22)
			{
				data->GetRealData(currOfst, 44, BYTEARR(shpBuff));
				if (*(Int32*)shpBuff == 13)
				{
					rec = MemAlloc(Map::SHPData::RecHdr, 1);
					rec->x1 = ReadDouble(&shpBuff[4]);
					rec->y1 = ReadDouble(&shpBuff[12]);
					rec->x2 = ReadDouble(&shpBuff[20]);
					rec->y2 = ReadDouble(&shpBuff[28]);
					rec->vec = 0;
					rec->nPoint = ReadUInt32(&shpBuff[40]);
					rec->nPtOfst = ReadUInt32(&shpBuff[36]);
					rec->ofst = (UInt32)(currOfst + 44);
					this->recs->Add(rec);
				}
				else
				{
					this->recs->Add(0);
				}
				currOfst += fileLen << 1;
			}
			else
			{
				currOfst += fileLen << 1;
				this->recs->Add(0);
			}
		}		
	}
	else if (*(Int32*)&shpHdr[32] == 15)
	{
		this->layerType = Map::DRAW_LAYER_POLYGON;
		NEW_CLASS(this->recs, Data::ArrayList<Optional<Map::SHPData::RecHdr>>());
		NEW_CLASS(this->recsMut, Sync::Mutex());
		while (data->GetRealData(currOfst, 8, BYTEARR(shpBuff)) == 8)
		{
			currOfst += 8;
			fileLen = ReadMUInt32(&shpBuff[4]);

			if (fileLen > 22)
			{
				data->GetRealData(currOfst, 44, BYTEARR(shpBuff));
				if (*(Int32*)shpBuff == 15)
				{
					rec = MemAlloc(Map::SHPData::RecHdr, 1);
					rec->x1 = ReadDouble(&shpBuff[4]);
					rec->y1 = ReadDouble(&shpBuff[12]);
					rec->x2 = ReadDouble(&shpBuff[20]);
					rec->y2 = ReadDouble(&shpBuff[28]);
					rec->vec = 0;
					rec->nPoint = ReadUInt32(&shpBuff[40]);
					rec->nPtOfst = ReadUInt32(&shpBuff[36]);
					rec->ofst = (UInt32)(currOfst + 44);
					this->recs->Add(rec);
				}
				else
				{
					this->recs->Add(0);
				}
				currOfst += fileLen << 1;
			}
			else
			{
				currOfst += fileLen << 1;
				this->recs->Add(0);
			}
		}		
	}
	else
	{
		DEL_CLASS(this->shpData);
		this->shpData = 0;
		return;
	}

	IO::StmData::FileData dbfData({sbuff, (UOSInt)(sptr - sbuff)}, false);
	if (dbfData.GetDataSize() > 0)
	{
		NEW_CLASS(this->dbf, DB::DBFFile(dbfData, codePage));
		if (this->dbf->IsError())
		{
			DEL_CLASS(this->dbf);
			this->dbf = 0;
		}
		else
		{
			UOSInt nameCol = 0;
			i = this->dbf->GetColCount();
			while (i-- > 0)
			{
				sptr = this->dbf->GetColumnName(i, sbuff);
				if (Text::StrEndsWithICaseC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("NAME")))
				{
					nameCol = i;
				}
			}
			this->SetNameCol(nameCol);
		}
	}
}

Map::SHPData::~SHPData()
{
	SDEL_CLASS(this->dbf);
	SDEL_CLASS(this->shpData);
	SDEL_CLASS(this->ptX);
	SDEL_CLASS(this->ptY);
	SDEL_CLASS(this->ptZ);
	if (this->recs)
	{
		UOSInt i = this->recs->GetCount();
		while (i-- > 0)
		{
			NN<RecHdr> obj;
			if (this->recs->RemoveAt(i).SetTo(obj))
			{
				SDEL_CLASS(obj->vec);
				MemFreeNN(obj);
			}
		}
		DEL_CLASS(this->recs);
		this->recs = 0;
	}
	SDEL_CLASS(this->recsMut);
}

Bool Map::SHPData::IsError() const
{
	return dbf == 0 ;
}

void Map::SHPData::LatLon2XY(Double lat, Double lon, OutParam<Int32> x, OutParam<Int32> y)
{
	x.Set((Int32)(lon * 200000));
	y.Set((Int32)(lat * 200000));
}

Map::DrawLayerType Map::SHPData::GetLayerType() const
{
	return this->layerType;
}

UOSInt Map::SHPData::GetAllObjectIds(NN<Data::ArrayListInt64> outArr, NameArray **nameArr)
{
	UOSInt i;
	UOSInt j;
	if (this->layerType == Map::DRAW_LAYER_POINT || this->layerType == Map::DRAW_LAYER_POINT3D)
	{
		i = 0;
		j = this->ptX->GetCount();
		while (i < j)
		{
			outArr->Add((Int64)i);
			i++;
		}
		return j;
	}
	else if (this->layerType == Map::DRAW_LAYER_POLYGON || this->layerType == Map::DRAW_LAYER_POLYLINE)
	{
		j = this->recs->GetCount();
		i = 0;
		while (i < j)
		{
			outArr->Add((Int64)i);
			i++;
		}
		return j;
	}
	else 
	{
		return 0;
	}
}

UOSInt Map::SHPData::GetObjectIds(NN<Data::ArrayListInt64> outArr, NameArray **nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty)
{
	return GetObjectIdsMapXY(outArr, nameArr, rect.ToDouble() / mapRate, keepEmpty);
}

UOSInt Map::SHPData::GetObjectIdsMapXY(NN<Data::ArrayListInt64> outArr, NameArray **nameArr, Math::RectAreaDbl rect, Bool keepEmpty)
{
	UOSInt retCnt = 0;
	UOSInt i = 0;
	UOSInt j;
	if (this->layerType == Map::DRAW_LAYER_POINT || this->layerType == Map::DRAW_LAYER_POINT3D)
	{
		Double x;
		Double y;
		j = this->ptX->GetCount();
		while (i < j)
		{
			x = this->ptX->GetItem(i);
			y = this->ptY->GetItem(i);
			if (rect.ContainPt(x, y))
			{
				outArr->Add((Int64)i);
				retCnt++;
			}
			i++;
		}
		return retCnt;
	}
	else if (this->layerType == Map::DRAW_LAYER_POLYGON || this->layerType == Map::DRAW_LAYER_POLYLINE || this->layerType == Map::DRAW_LAYER_POLYLINE3D)
	{
		NN<Map::SHPData::RecHdr> rec;
		j = this->recs->GetCount();
		while (i < j)
		{
			if (this->recs->GetItem(i).SetTo(rec))
			{
				if (rec->x2 >= rect.min.x && rec->x1 <= rect.max.x && rec->y2 >= rect.min.y && rec->y1 <= rect.max.y)
				{
					outArr->Add((Int64)i);
					retCnt++;
				}
			}
			i++;
		}
		return retCnt;
	}
	else
	{
		return 0;
	}
}

Int64 Map::SHPData::GetObjectIdMax() const
{
	if (this->layerType == Map::DRAW_LAYER_POINT || this->layerType == Map::DRAW_LAYER_POINT3D)
	{
		return (Int64)this->ptX->GetCount() - 1;
	}
	else if (this->layerType == Map::DRAW_LAYER_POLYGON || this->layerType == Map::DRAW_LAYER_POLYLINE)
	{
		return (Int64)this->recs->GetCount() - 1;
	}
	else
	{
		return -1;
	}
}

void Map::SHPData::ReleaseNameArr(NameArray *nameArr)
{
}

Bool Map::SHPData::GetString(NN<Text::StringBuilderUTF8> sb, NameArray *nameArr, Int64 id, UOSInt strIndex)
{
	Bool ret = this->dbf->GetRecord(sb, (UOSInt)id, strIndex);
	sb->Trim();
	return ret;
}

UOSInt Map::SHPData::GetColumnCnt() const
{
	return this->dbf->GetColCount();
}

UTF8Char *Map::SHPData::GetColumnName(UTF8Char *buff, UOSInt colIndex)
{
	return this->dbf->GetColumnName(colIndex, buff);
}

DB::DBUtil::ColType Map::SHPData::GetColumnType(UOSInt colIndex, OptOut<UOSInt> colSize)
{
	return this->dbf->GetColumnType(colIndex, colSize);
}

Bool Map::SHPData::GetColumnDef(UOSInt colIndex, NN<DB::ColDef> colDef)
{
	return this->dbf->GetColumnDef(colIndex, colDef);
}

UInt32 Map::SHPData::GetCodePage() const
{
	return this->dbf->GetCodePage();
}

Bool Map::SHPData::GetBounds(OutParam<Math::RectAreaDbl> bounds) const
{
	bounds.Set(Math::RectAreaDbl(this->min, this->max));
	return this->min.x != 0 || this->min.y != 0 || this->max.x != 0 || this->max.y != 0;
}

Map::GetObjectSess *Map::SHPData::BeginGetObject()
{
	return (GetObjectSess*)-1;
}

void Map::SHPData::EndGetObject(GetObjectSess *session)
{
}

Math::Geometry::Vector2D *Map::SHPData::GetNewVectorById(GetObjectSess *session, Int64 id)
{
	NN<Map::SHPData::RecHdr> rec;
	NN<Sync::Mutex> mut;

	UInt32 srid = this->csys->GetSRID();
	if (this->layerType == Map::DRAW_LAYER_POINT)
	{
		Math::Geometry::Point *pt;
		if (id < 0 || (UInt64)id >= this->ptX->GetCount())
		{
			return 0;
		}
		NEW_CLASS(pt, Math::Geometry::Point(srid, this->ptX->GetItem((UOSInt)id), this->ptY->GetItem((UOSInt)id)));
		return pt;
	}
	else if (this->layerType == Map::DRAW_LAYER_POINT3D)
	{
		Math::Geometry::PointZ *pt;
		if (id < 0 || (UInt64)id >= this->ptX->GetCount())
		{
			return 0;
		}
		NEW_CLASS(pt, Math::Geometry::PointZ(srid, this->ptX->GetItem((UOSInt)id), this->ptY->GetItem((UOSInt)id), this->ptZ->GetItem((UOSInt)id)));
		return pt;
	}
	else if (this->layerType == Map::DRAW_LAYER_POLYGON && mut.Set(this->recsMut))
	{
		Math::Geometry::Polygon *pg;
		Sync::MutexUsage mutUsage(mut);
		if (!this->recs->GetItem((UOSInt)id).SetTo(rec))
			return 0;
		if (rec->vec) return rec->vec->Clone().Ptr();
		NEW_CLASS(pg, Math::Geometry::Polygon(srid));
		UInt32 *ptOfstList = MemAlloc(UInt32, rec->nPtOfst);
		Math::Coord2DDbl *pointList = MemAllocA(Math::Coord2DDbl, rec->nPoint);
		shpData->GetRealData(rec->ofst, rec->nPtOfst << 2, Data::ByteArray((UInt8*)ptOfstList, rec->nPtOfst << 2));
		shpData->GetRealData(rec->ofst + (rec->nPtOfst << 2), rec->nPoint << 4, Data::ByteArray((UInt8*)pointList, rec->nPoint << 4));
		pg->AddFromPtOfst(ptOfstList, rec->nPtOfst, pointList, rec->nPoint, 0, 0);
		MemFreeA(pointList);
		MemFree(ptOfstList);
		rec->vec = pg->Clone().Ptr();
		return pg;
	}
	else if (this->layerType == Map::DRAW_LAYER_POLYLINE && mut.Set(this->recsMut))
	{
		Math::Geometry::Polyline *pl;
		Sync::MutexUsage mutUsage(mut);
		if (!this->recs->GetItem((UOSInt)id).SetTo(rec))
			return 0;
		if (rec->vec) return rec->vec->Clone().Ptr();
		NEW_CLASS(pl, Math::Geometry::Polyline(srid));
		UInt32 *ptOfstList = MemAlloc(UInt32, rec->nPtOfst);
		Math::Coord2DDbl *pointList = MemAllocA(Math::Coord2DDbl, rec->nPoint);
		shpData->GetRealData(rec->ofst, rec->nPtOfst << 2, Data::ByteArray((UInt8*)ptOfstList, rec->nPtOfst << 2));
		shpData->GetRealData(rec->ofst + (rec->nPtOfst << 2), rec->nPoint << 4, Data::ByteArray((UInt8*)pointList, rec->nPoint << 4));
		pl->AddFromPtOfst(ptOfstList, rec->nPtOfst, pointList, rec->nPoint, 0, 0);
		MemFreeA(pointList);
		MemFree(ptOfstList);
		rec->vec = pl->Clone().Ptr();
		return pl;
	}
	else if (this->layerType == Map::DRAW_LAYER_POLYLINE3D && mut.Set(this->recsMut))
	{
		Math::Geometry::Polyline *pl;
		Sync::MutexUsage mutUsage(mut);
		if (!this->recs->GetItem((UOSInt)id).SetTo(rec))
			return 0;
		if (rec->vec) return rec->vec->Clone().Ptr();
		NEW_CLASS(pl, Math::Geometry::Polyline(srid));
		UInt32 *ptOfstList = MemAlloc(UInt32, rec->nPtOfst);
		Math::Coord2DDbl *pointList = MemAllocA(Math::Coord2DDbl, rec->nPoint);
		Double *zList = MemAlloc(Double, rec->nPoint);
		shpData->GetRealData(rec->ofst, rec->nPtOfst << 2, Data::ByteArray((UInt8*)ptOfstList, rec->nPtOfst << 2));
		shpData->GetRealData(rec->ofst + (rec->nPtOfst << 2), rec->nPoint << 4, Data::ByteArray((UInt8*)pointList, rec->nPoint << 4));
		shpData->GetRealData(rec->ofst + (rec->nPtOfst << 2) + (rec->nPoint << 4) + 16, rec->nPoint << 3, Data::ByteArray((UInt8*)zList, rec->nPoint << 3));
		pl->AddFromPtOfst(ptOfstList, rec->nPtOfst, pointList, rec->nPoint, 0, 0);
		MemFree(zList);
		MemFreeA(pointList);
		MemFree(ptOfstList);
		rec->vec = pl->Clone().Ptr();
		return pl;
	}
	else
	{
		return 0;
	}
}

UOSInt Map::SHPData::QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names)
{
	return this->dbf->QueryTableNames(schemaName, names);
}

Optional<DB::DBReader> Map::SHPData::QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayListStringNN *columNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition)
{
	return this->dbf->QueryTableData(schemaName, tableName, columNames, ofst, maxCnt, ordering, condition);
}

DB::TableDef *Map::SHPData::GetTableDef(Text::CString schemaName, Text::CString tableName)
{
	return this->dbf->GetTableDef(schemaName, tableName);
}

void Map::SHPData::CloseReader(NN<DB::DBReader> r)
{
	this->dbf->CloseReader(r);
}

void Map::SHPData::GetLastErrorMsg(NN<Text::StringBuilderUTF8> str)
{
	this->dbf->GetLastErrorMsg(str);

}

void Map::SHPData::Reconnect()
{
	return this->dbf->Reconnect();
}

Map::MapDrawLayer::ObjectClass Map::SHPData::GetObjectClass() const
{
	return Map::MapDrawLayer::OC_SHP_DATA;
}
