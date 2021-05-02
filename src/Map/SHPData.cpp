#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Data/ByteTool.h"
#include "Math/Math.h"
#include "Map/SHPData.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Polyline.h"
#include "Math/Polygon.h"
#include "Math/Point.h"
#include "Math/Point3D.h"

Map::SHPData::SHPData(UInt8 *shpHdr, IO::IStreamData *data, Int32 codePage) : Map::IMapDrawLayer(data->GetFullName(), 0, 0)
{
	UTF8Char u8buff[256];
	UTF8Char *u8ptr;
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
	this->layerType = Map::DRAW_LAYER_UNKNOWN;
	this->mapRate = 10000000.0;

	u8ptr = Text::StrConcat(u8buff, data->GetFullFileName());
	if ((u8ptr - u8buff) < 4)
	{
		return;
	}
	if (u8ptr[-4] == '.')
	{
		Text::StrConcat(&u8ptr[-3], (const UTF8Char*)"prj");
	}
	else
	{
		Text::StrConcat(u8ptr, (const UTF8Char*)".prj");
	}
	this->csys = Math::CoordinateSystemManager::ParsePRJFile(u8buff);

	if (u8ptr[-4] == '.')
	{
		Text::StrConcat(&u8ptr[-3], (const UTF8Char*)"dbf");
	}
	else
	{
		Text::StrConcat(u8ptr, (const UTF8Char*)".dbf");
	}

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
	this->shpData = data->GetPartialData(0, data->GetDataSize());
	this->xMin = ReadDouble(&shpHdr[36]);
	this->yMin = ReadDouble(&shpHdr[44]);
	this->xMax = ReadDouble(&shpHdr[52]);
	this->yMax = ReadDouble(&shpHdr[60]);

	if (xMax > 200000000 || xMin < -200000000 || yMax > 200000000 || yMin < -200000000)
	{
		this->mapRate = 1.0;
	}
	else if (xMax > 20000000 || xMin < -20000000 || yMax > 20000000 || yMin < -20000000)
	{
		this->mapRate = 10.0;
	}
	else if (xMax > 2000000 || xMin < -2000000 || yMax > 2000000 || yMin < -2000000)
	{
		this->mapRate = 100.0;
	}
	else if (xMax > 200000 || xMin < -200000 || yMax > 200000 || yMin < -200000)
	{
		this->mapRate = 1000.0;
	}
	else if (xMax > 20000 || xMin < -20000 || yMax > 20000 || yMin < -20000)
	{
		this->mapRate = 10000.0;
	}
	else if (xMax > 2000 || xMin < -2000 || yMax > 2000 || yMin < -2000)
	{
		this->mapRate = 100000.0;
	}
	else if (xMax > 200 || xMin < -200 || yMax > 200 || yMin < -200)
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
		while (data->GetRealData(currOfst, 8, shpBuff) == 8)
		{
			currOfst += 8;
			fileLen = ReadMUInt32(&shpBuff[4]);

			data->GetRealData(currOfst, fileLen << 1, shpBuff);
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
		NEW_CLASS(this->recs, Data::ArrayList<Map::SHPData::RecHdr*>());
		while (data->GetRealData(currOfst, 8, shpBuff) == 8)
		{
			currOfst += 8;
			fileLen = ReadMUInt32(&shpBuff[4]);

			if (fileLen > 22)
			{
				data->GetRealData(currOfst, 44, shpBuff);
				if (*(Int32*)shpBuff == 3)
				{
					rec = MemAlloc(Map::SHPData::RecHdr, 1);
					rec->x1 = ReadDouble(&shpBuff[4]);
					rec->y1 = ReadDouble(&shpBuff[12]);
					rec->x2 = ReadDouble(&shpBuff[20]);
					rec->y2 = ReadDouble(&shpBuff[28]);
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
		NEW_CLASS(this->recs, Data::ArrayList<Map::SHPData::RecHdr*>());
		while (data->GetRealData(currOfst, 8, shpBuff) == 8)
		{
			currOfst += 8;
			fileLen = ReadMUInt32(&shpBuff[4]);

			if (fileLen > 22)
			{
				data->GetRealData(currOfst, 44, shpBuff);
				if (*(Int32*)shpBuff == 5)
				{
					rec = MemAlloc(Map::SHPData::RecHdr, 1);
					rec->x1 = ReadDouble(&shpBuff[4]);
					rec->y1 = ReadDouble(&shpBuff[12]);
					rec->x2 = ReadDouble(&shpBuff[20]);
					rec->y2 = ReadDouble(&shpBuff[28]);
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
		while (data->GetRealData(currOfst, 8, shpBuff) == 8)
		{
			currOfst += 8;
			fileLen = ReadMUInt32(&shpBuff[4]);

			data->GetRealData(currOfst, fileLen << 1, shpBuff);
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
		NEW_CLASS(this->recs, Data::ArrayList<Map::SHPData::RecHdr*>());
		while (data->GetRealData(currOfst, 8, shpBuff) == 8)
		{
			currOfst += 8;
			fileLen = ReadMUInt32(&shpBuff[4]);

			if (fileLen > 22)
			{
				data->GetRealData(currOfst, 44, shpBuff);
				if (*(Int32*)shpBuff == 13)
				{
					rec = MemAlloc(Map::SHPData::RecHdr, 1);
					rec->x1 = ReadDouble(&shpBuff[4]);
					rec->y1 = ReadDouble(&shpBuff[12]);
					rec->x2 = ReadDouble(&shpBuff[20]);
					rec->y2 = ReadDouble(&shpBuff[28]);
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
		NEW_CLASS(this->recs, Data::ArrayList<Map::SHPData::RecHdr*>());
		while (data->GetRealData(currOfst, 8, shpBuff) == 8)
		{
			currOfst += 8;
			fileLen = ReadMUInt32(&shpBuff[4]);

			if (fileLen > 22)
			{
				data->GetRealData(currOfst, 44, shpBuff);
				if (*(Int32*)shpBuff == 15)
				{
					rec = MemAlloc(Map::SHPData::RecHdr, 1);
					rec->x1 = ReadDouble(&shpBuff[4]);
					rec->y1 = ReadDouble(&shpBuff[12]);
					rec->x2 = ReadDouble(&shpBuff[20]);
					rec->y2 = ReadDouble(&shpBuff[28]);
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

	IO::StmData::FileData *dbfData;
	NEW_CLASS(dbfData, IO::StmData::FileData(u8buff, false));
	if (dbfData->GetDataSize() > 0)
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
				this->dbf->GetColumnName(i, u8buff);
				if (Text::StrEndsWithICase(u8buff, (const UTF8Char*)"NAME"))
				{
					nameCol = i;
				}
			}
			this->SetNameCol(nameCol);
		}
	}
	DEL_CLASS(dbfData);
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
			void *obj;
			if ((obj = this->recs->RemoveAt(i)) != 0)
				MemFree(obj);
		}
		DEL_CLASS(this->recs);
		this->recs = 0;
	}
}

Bool Map::SHPData::IsError()
{
	return dbf == 0 ;
}

void Map::SHPData::LatLon2XY(Double lat, Double lon, Int32 *x, Int32 *y)
{
	*x = (Int32)(lon * 200000);
	*y = (Int32)(lat * 200000);
}

Map::DrawLayerType Map::SHPData::GetLayerType()
{
	return this->layerType;
}

UOSInt Map::SHPData::GetAllObjectIds(Data::ArrayListInt64 *outArr, void **nameArr)
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

UOSInt Map::SHPData::GetObjectIds(Data::ArrayListInt64 *outArr, void **nameArr, Double mapRate, Int32 x1, Int32 y1, Int32 x2, Int32 y2, Bool keepEmpty)
{
	return GetObjectIdsMapXY(outArr, nameArr, x1 / mapRate, y1 / mapRate, x2 / mapRate, y2 / mapRate, keepEmpty);
}

UOSInt Map::SHPData::GetObjectIdsMapXY(Data::ArrayListInt64 *outArr, void **nameArr, Double x1, Double y1, Double x2, Double y2, Bool keepEmpty)
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
			if (x1 <= x && x2 >= x && y1 <= y && y2 >= y)
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
		Map::SHPData::RecHdr *rec;
		j = this->recs->GetCount();
		while (i < j)
		{
			rec = (Map::SHPData::RecHdr*)this->recs->GetItem(i);
			if (rec)
			{
				if (rec->x2 >= x1 && rec->x1 <= x2 && rec->y2 >= y1 && rec->y1 <= y2)
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

Int64 Map::SHPData::GetObjectIdMax()
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

void Map::SHPData::ReleaseNameArr(void *nameArr)
{
}

UTF8Char *Map::SHPData::GetString(UTF8Char *buff, UOSInt buffSize, void *nameArr, Int64 id, UOSInt strIndex)
{
	return this->dbf->GetRecord(buff, (UOSInt)id, strIndex);
}

UOSInt Map::SHPData::GetColumnCnt()
{
	return this->dbf->GetColCount();
}

UTF8Char *Map::SHPData::GetColumnName(UTF8Char *buff, UOSInt colIndex)
{
	return this->dbf->GetColumnName(colIndex, buff);
}

DB::DBUtil::ColType Map::SHPData::GetColumnType(UOSInt colIndex, UOSInt *colSize)
{
	return this->dbf->GetColumnType(colIndex, colSize);
}

Bool Map::SHPData::GetColumnDef(UOSInt colIndex, DB::ColDef *colDef)
{
	return this->dbf->GetColumnDef(colIndex, colDef);
}

Int32 Map::SHPData::GetCodePage()
{
	return this->dbf->GetCodePage();
}

Bool Map::SHPData::GetBoundsDbl(Double *minX, Double *minY, Double *maxX, Double *maxY)
{
	*minX = this->xMin;
	*minY = this->yMin;
	*maxX = this->xMax;
	*maxY = this->yMax;
	return this->xMin != 0 || this->yMin != 0 || this->xMax != 0 || this->yMax != 0;
}

void *Map::SHPData::BeginGetObject()
{
	return (void*)-1;
}

void Map::SHPData::EndGetObject(void *session)
{
}

Map::DrawObjectL *Map::SHPData::GetObjectByIdD(void *session, Int64 id)
{
	Map::DrawObjectL *obj;
	Map::SHPData::RecHdr *rec;

	if (this->layerType == Map::DRAW_LAYER_POINT || this->layerType == Map::DRAW_LAYER_POINT3D)
	{
		if (id < 0 || (UInt64)id >= this->ptX->GetCount())
		{
			return 0;
		}
		obj = MemAlloc(Map::DrawObjectL, 1);
		obj->nPtOfst = 0;
		obj->nPoint = 1;
		obj->objId = id;
		obj->ptOfstArr = 0;
		obj->pointArr = MemAlloc(Double, 2);
		obj->pointArr[0] = (this->ptX->GetItem((UOSInt)id));
		obj->pointArr[1] = (this->ptY->GetItem((UOSInt)id));
		obj->flags = 0;
		obj->lineColor = 0;
		return obj;
	}
	else if (this->layerType == Map::DRAW_LAYER_POLYGON || this->layerType == Map::DRAW_LAYER_POLYLINE)
	{
		rec = (Map::SHPData::RecHdr*)this->recs->GetItem((UOSInt)id);
		if (rec == 0)
			return 0;

		obj = MemAlloc(Map::DrawObjectL, 1);
		obj->nPtOfst = rec->nPtOfst;
		obj->nPoint = rec->nPoint;
		obj->objId = id;
		obj->ptOfstArr = MemAlloc(UInt32, rec->nPtOfst);
		obj->pointArr = MemAlloc(Double, rec->nPoint << 1);
		shpData->GetRealData(rec->ofst, rec->nPtOfst << 2, (UInt8*)obj->ptOfstArr);
		shpData->GetRealData(rec->ofst + (rec->nPtOfst << 2), rec->nPoint << 4, (UInt8*)obj->pointArr);
		obj->flags = 0;
		obj->lineColor = 0;
		return obj;
	}
	else if (this->layerType == Map::DRAW_LAYER_POLYLINE3D)
	{
		rec = (Map::SHPData::RecHdr*)this->recs->GetItem((UOSInt)id);
		if (rec == 0)
			return 0;

		obj = MemAlloc(Map::DrawObjectL, 1);
		obj->nPtOfst = rec->nPtOfst;
		obj->nPoint = rec->nPoint;
		obj->objId = id;
		obj->ptOfstArr = MemAlloc(UInt32, rec->nPtOfst);
		obj->pointArr = MemAlloc(Double, rec->nPoint << 1);
		shpData->GetRealData(rec->ofst, rec->nPtOfst << 2, (UInt8*)obj->ptOfstArr);
		shpData->GetRealData(rec->ofst + (rec->nPtOfst << 2), rec->nPoint << 4, (UInt8*)obj->pointArr);
		obj->flags = 0;
		obj->lineColor = 0;
		return obj;
	}
	else
	{
		return 0;
	}
}

Math::Vector2D *Map::SHPData::GetVectorById(void *session, Int64 id)
{
	Map::SHPData::RecHdr *rec;
	UOSInt nPoint;

	if (this->layerType == Map::DRAW_LAYER_POINT)
	{
		Math::Point *pt;
		if (id < 0 || (UInt64)id >= this->ptX->GetCount())
		{
			return 0;
		}
		NEW_CLASS(pt, Math::Point(this->csys->GetSRID(), this->ptX->GetItem((UOSInt)id), this->ptY->GetItem((UOSInt)id)));
		return pt;
	}
	else if (this->layerType == Map::DRAW_LAYER_POINT3D)
	{
		Math::Point3D *pt;
		if (id < 0 || (UInt64)id >= this->ptX->GetCount())
		{
			return 0;
		}
		NEW_CLASS(pt, Math::Point3D(this->csys->GetSRID(), this->ptX->GetItem((UOSInt)id), this->ptY->GetItem((UOSInt)id), this->ptZ->GetItem((UOSInt)id)));
		return pt;
	}
	else if (this->layerType == Map::DRAW_LAYER_POLYGON)
	{
		Math::Polygon *pg;
		rec = (Map::SHPData::RecHdr*)this->recs->GetItem((UOSInt)id);
		if (rec == 0)
			return 0;
		NEW_CLASS(pg, Math::Polygon(this->csys->GetSRID(), rec->nPtOfst, rec->nPoint));
		shpData->GetRealData(rec->ofst, rec->nPtOfst << 2, (UInt8*)pg->GetPtOfstList(&nPoint));
		shpData->GetRealData(rec->ofst + (rec->nPtOfst << 2), rec->nPoint << 4, (UInt8*)pg->GetPointList(&nPoint));
		return pg;
	}
	else if (this->layerType == Map::DRAW_LAYER_POLYLINE)
	{
		Math::Polyline *pl;
		rec = (Map::SHPData::RecHdr*)this->recs->GetItem((UOSInt)id);
		if (rec == 0)
			return 0;
		NEW_CLASS(pl, Math::Polyline(this->csys->GetSRID(), rec->nPtOfst, rec->nPoint));
		shpData->GetRealData(rec->ofst, rec->nPtOfst << 2, (UInt8*)pl->GetPtOfstList(&nPoint));
		shpData->GetRealData(rec->ofst + (rec->nPtOfst << 2), rec->nPoint << 4, (UInt8*)pl->GetPointList(&nPoint));
		return pl;
	}
	else if (this->layerType == Map::DRAW_LAYER_POLYLINE3D)
	{
		Math::Polyline3D *pl;
		rec = (Map::SHPData::RecHdr*)this->recs->GetItem((UOSInt)id);
		if (rec == 0)
			return 0;
		NEW_CLASS(pl, Math::Polyline3D(this->csys->GetSRID(), rec->nPtOfst, rec->nPoint));
		shpData->GetRealData(rec->ofst, rec->nPtOfst << 2, (UInt8*)pl->GetPtOfstList(&nPoint));
		shpData->GetRealData(rec->ofst + (rec->nPtOfst << 2), rec->nPoint << 4, (UInt8*)pl->GetPointList(&nPoint));
		shpData->GetRealData(rec->ofst + (rec->nPtOfst << 2) + (rec->nPoint << 4) + 16, rec->nPoint << 3, (UInt8*)pl->GetAltitudeList(&nPoint));
		return pl;
	}
	else
	{
		return 0;
	}
}

void Map::SHPData::ReleaseObject(void *session, DrawObjectL *obj)
{
	if (obj->ptOfstArr)
		MemFree(obj->ptOfstArr);
	if (obj->pointArr)
		MemFree(obj->pointArr);
	MemFree(obj);
}

/*void Map::SHPData::ReleaseObject(void *session, DrawObjectDbl *obj)
{
	if (obj->parts)
		MemFree(obj->parts);
	if (obj->points)
		MemFree(obj->points);
	MemFree(obj);
}*/

UOSInt Map::SHPData::GetTableNames(Data::ArrayList<const UTF8Char*> *names)
{
	return this->dbf->GetTableNames(names);
}

DB::DBReader *Map::SHPData::GetTableData(const UTF8Char *name, UOSInt maxCnt, void *ordering, void *condition)
{
	return this->dbf->GetTableData(name, maxCnt, ordering, condition);
}

void Map::SHPData::CloseReader(DB::DBReader *r)
{
	this->dbf->CloseReader(r);
}

void Map::SHPData::GetErrorMsg(Text::StringBuilderUTF *str)
{
	this->dbf->GetErrorMsg(str);

}

void Map::SHPData::Reconnect()
{
	return this->dbf->Reconnect();
}

Map::IMapDrawLayer::ObjectClass Map::SHPData::GetObjectClass()
{
	return Map::IMapDrawLayer::OC_SHP_DATA;
}
