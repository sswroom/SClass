#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"
#include "DB/TableDef.h"
#include "IO/Path.h"
#include "IO/StmData/BufferedStreamData.h"
#include "Map/SHPData.h"
#include "Math/ArcGISPRJParser.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Math_C.h"
#include "Math/WKTWriter.h"
#include "Math/Geometry/Polyline.h"
#include "Math/Geometry/Polygon.h"
#include "Math/Geometry/Point.h"
#include "Math/Geometry/PointZ.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"

Map::SHPData::SHPData(UnsafeArray<const UInt8> shpHdr, NN<IO::StreamData> data, UInt32 codePage, NN<Math::ArcGISPRJParser> prjParser) : Map::MapDrawLayer(data->GetFullName(), 0, nullptr, Math::CoordinateSystemManager::CreateWGS84Csys())
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	UInt8 shpBuff[100];
	Int32 valid;
	UInt32 fileLen;
//	Int32 shpType;
	UIntOS i;
	NN<Map::SHPData::RecHdr> rec;
	NN<Data::ArrayListDbl> ptX;
	NN<Data::ArrayListDbl> ptY;
	NN<Data::ArrayListDbl> ptZ;
	NN<Data::ArrayListObj<Optional<Map::SHPData::RecHdr>>> recs;

	this->dbf = nullptr;
	this->shpData = nullptr;
	this->ptX = nullptr;
	this->ptY = nullptr;
	this->ptZ = nullptr;
	this->recs = nullptr;
	this->recsMut = nullptr;
	this->layerType = Map::DRAW_LAYER_UNKNOWN;
	this->mapRate = 10000000.0;

	sptr = data->GetFullFileName()->ConcatTo(sbuff);
	if ((sptr - sbuff) < 4)
	{
		return;
	}
	i = Text::StrLastIndexOfC(sbuff, (UIntOS)(sptr - sbuff), IO::Path::PATH_SEPERATOR);
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
	if (prjParser->ParsePRJFile(CSTRP(sbuff, sptr)).SetTo(csys))
	{
		this->SetCoordinateSystem(csys);	
	}

	Text::StrConcatC(&sptr[-3], UTF8STRC("dbf"));

	if (ReadMInt32(&shpHdr[0]) != 9994 || ReadInt32(&shpHdr[28]) != 1000 || (ReadMUInt32(&shpHdr[24]) << 1) != data->GetDataSize())
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
	//this->shpData = data->GetPartialData(0, data->GetDataSize());
	NEW_CLASSNN(data, IO::StmData::BufferedStreamData(data->GetPartialData(0, data->GetDataSize())));
	this->shpData = data;

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
		NEW_CLASSNN(ptX, Data::ArrayListDbl());
		NEW_CLASSNN(ptY, Data::ArrayListDbl());
		this->ptX = ptX;
		this->ptY = ptY;
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
		NEW_CLASSNN(recs, Data::ArrayListObj<Optional<Map::SHPData::RecHdr>>());
		NEW_CLASSOPT(this->recsMut, Sync::Mutex());
		this->recs = recs;
		while (data->GetRealData(currOfst, 8, BYTEARR(shpBuff)) == 8)
		{
			currOfst += 8;
			fileLen = ReadMUInt32(&shpBuff[4]);

			if (fileLen > 22)
			{
				data->GetRealData(currOfst, 44, BYTEARR(shpBuff));
				if (*(Int32*)shpBuff == 3)
				{
					rec = MemAllocNN(Map::SHPData::RecHdr);
					rec->x1 = ReadDouble(&shpBuff[4]);
					rec->y1 = ReadDouble(&shpBuff[12]);
					rec->x2 = ReadDouble(&shpBuff[20]);
					rec->y2 = ReadDouble(&shpBuff[28]);
					rec->vec = nullptr;
					rec->nPoint = ReadUInt32(&shpBuff[40]);
					rec->nPtOfst = ReadUInt32(&shpBuff[36]);
					rec->ofst = (UInt32)(currOfst + 44);
					rec->endOfst = (UInt32)currOfst + (fileLen << 1);
					recs->Add(rec);
				}
				else
				{
					recs->Add(nullptr);
				}
				currOfst += fileLen << 1;
			}
			else
			{
				currOfst += fileLen << 1;
				recs->Add(nullptr);
			}
		}		
	}
	else if (*(Int32*)&shpHdr[32] == 5)
	{
		this->layerType = Map::DRAW_LAYER_POLYGON;
		NEW_CLASSNN(recs, Data::ArrayListObj<Optional<Map::SHPData::RecHdr>>());
		NEW_CLASSOPT(this->recsMut, Sync::Mutex());
		this->recs = recs;
		while (data->GetRealData(currOfst, 8, BYTEARR(shpBuff)) == 8)
		{
			currOfst += 8;
			fileLen = ReadMUInt32(&shpBuff[4]);

			if (fileLen > 22)
			{
				data->GetRealData(currOfst, 44, BYTEARR(shpBuff));
				if (*(Int32*)shpBuff == 5)
				{
					rec = MemAllocNN(Map::SHPData::RecHdr);
					rec->x1 = ReadDouble(&shpBuff[4]);
					rec->y1 = ReadDouble(&shpBuff[12]);
					rec->x2 = ReadDouble(&shpBuff[20]);
					rec->y2 = ReadDouble(&shpBuff[28]);
					rec->vec = nullptr;
					rec->nPoint = ReadUInt32(&shpBuff[40]);
					rec->nPtOfst = ReadUInt32(&shpBuff[36]);
					rec->ofst = (UInt32)(currOfst + 44);
					rec->endOfst = (UInt32)currOfst + (fileLen << 1);
					recs->Add(rec);
				}
				else
				{
					recs->Add(nullptr);
				}
				currOfst += fileLen << 1;
			}
			else
			{
				currOfst += fileLen << 1;
				recs->Add(nullptr);
			}
		}		
	}
	else if (*(Int32*)&shpHdr[32] == 11)
	{
		this->layerType = Map::DRAW_LAYER_POINT3D;
		NEW_CLASSNN(ptX, Data::ArrayListDbl());
		NEW_CLASSNN(ptY, Data::ArrayListDbl());
		NEW_CLASSNN(ptZ, Data::ArrayListDbl());
		this->ptX = ptX;
		this->ptY = ptY;
		this->ptZ = ptZ;
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
		NEW_CLASSNN(recs, Data::ArrayListObj<Optional<Map::SHPData::RecHdr>>());
		NEW_CLASSOPT(this->recsMut, Sync::Mutex());
		this->recs = recs;
		while (data->GetRealData(currOfst, 8, BYTEARR(shpBuff)) == 8)
		{
			currOfst += 8;
			fileLen = ReadMUInt32(&shpBuff[4]);

			if (fileLen > 22)
			{
				data->GetRealData(currOfst, 44, BYTEARR(shpBuff));
				if (*(Int32*)shpBuff == 13)
				{
					rec = MemAllocNN(Map::SHPData::RecHdr);
					rec->x1 = ReadDouble(&shpBuff[4]);
					rec->y1 = ReadDouble(&shpBuff[12]);
					rec->x2 = ReadDouble(&shpBuff[20]);
					rec->y2 = ReadDouble(&shpBuff[28]);
					rec->vec = nullptr;
					rec->nPoint = ReadUInt32(&shpBuff[40]);
					rec->nPtOfst = ReadUInt32(&shpBuff[36]);
					rec->ofst = (UInt32)(currOfst + 44);
					rec->endOfst = (UInt32)currOfst + (fileLen << 1);
					recs->Add(rec);
				}
				else
				{
					recs->Add(nullptr);
				}
				currOfst += fileLen << 1;
			}
			else
			{
				currOfst += fileLen << 1;
				recs->Add(nullptr);
			}
		}		
	}
	else if (*(Int32*)&shpHdr[32] == 15)
	{
		this->layerType = Map::DRAW_LAYER_POLYGON;
		NEW_CLASSNN(recs, Data::ArrayListObj<Optional<Map::SHPData::RecHdr>>());
		NEW_CLASSOPT(this->recsMut, Sync::Mutex());
		this->recs = recs;
		while (data->GetRealData(currOfst, 8, BYTEARR(shpBuff)) == 8)
		{
			currOfst += 8;
			fileLen = ReadMUInt32(&shpBuff[4]);

			if (fileLen > 22)
			{
				data->GetRealData(currOfst, 44, BYTEARR(shpBuff));
				if (*(Int32*)shpBuff == 15)
				{
					rec = MemAllocNN(Map::SHPData::RecHdr);
					rec->x1 = ReadDouble(&shpBuff[4]);
					rec->y1 = ReadDouble(&shpBuff[12]);
					rec->x2 = ReadDouble(&shpBuff[20]);
					rec->y2 = ReadDouble(&shpBuff[28]);
					rec->vec = nullptr;
					rec->nPoint = ReadUInt32(&shpBuff[40]);
					rec->nPtOfst = ReadUInt32(&shpBuff[36]);
					rec->ofst = (UInt32)(currOfst + 44);
					rec->endOfst = (UInt32)currOfst + (fileLen << 1);
					recs->Add(rec);
				}
				else
				{
					recs->Add(nullptr);
				}
				currOfst += fileLen << 1;
			}
			else
			{
				currOfst += fileLen << 1;
				recs->Add(nullptr);
			}
		}		
	}
	else
	{
		this->shpData.Delete();
		return;
	}

	IO::StmData::FileData dbfData({sbuff, (UIntOS)(sptr - sbuff)}, false);
	if (dbfData.GetDataSize() > 0)
	{
		NN<DB::DBFFile> dbf;
		NEW_CLASSNN(dbf, DB::DBFFile(dbfData, codePage));
		if (dbf->IsError())
		{
			dbf.Delete();
		}
		else
		{
			this->dbf = dbf;
			UIntOS nameCol = 0;
			i = dbf->GetColCount();
			while (i-- > 0)
			{
				if (dbf->GetColumnName(i, sbuff).SetTo(sptr) && Text::StrEndsWithICaseC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC("NAME")))
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
	this->dbf.Delete();
	this->shpData.Delete();
	this->ptX.Delete();
	this->ptY.Delete();
	this->ptZ.Delete();
	NN<Data::ArrayListObj<Optional<Map::SHPData::RecHdr>>> recs;
	if (this->recs.SetTo(recs))
	{
		UIntOS i = recs->GetCount();
		while (i-- > 0)
		{
			NN<RecHdr> obj;
			if (recs->RemoveAt(i).SetTo(obj))
			{
				obj->vec.Delete();
				MemFreeNN(obj);
			}
		}
		this->recs.Delete();
	}
	this->recsMut.Delete();
}

Bool Map::SHPData::IsError() const
{
	return this->dbf.IsNull();
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

UIntOS Map::SHPData::GetAllObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr)
{
	UIntOS i;
	UIntOS j;
	NN<Data::ArrayListDbl> ptX;
	NN<Data::ArrayListObj<Optional<Map::SHPData::RecHdr>>> recs;
	if (this->ptX.SetTo(ptX) && (this->layerType == Map::DRAW_LAYER_POINT || this->layerType == Map::DRAW_LAYER_POINT3D))
	{
		i = 0;
		j = ptX->GetCount();
		while (i < j)
		{
			outArr->Add((Int64)i + 1);
			i++;
		}
		return j;
	}
	else if (this->recs.SetTo(recs) && (this->layerType == Map::DRAW_LAYER_POLYGON || this->layerType == Map::DRAW_LAYER_POLYLINE || this->layerType == Map::DRAW_LAYER_POLYLINE3D))
	{
		j = recs->GetCount();
		i = 0;
		while (i < j)
		{
			outArr->Add((Int64)i + 1);
			i++;
		}
		return j;
	}
	else 
	{
		return 0;
	}
}

UIntOS Map::SHPData::GetObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty)
{
	return GetObjectIdsMapXY(outArr, nameArr, rect.ToDouble() / mapRate, keepEmpty);
}

UIntOS Map::SHPData::GetObjectIdsMapXY(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr, Math::RectAreaDbl rect, Bool keepEmpty)
{
	UIntOS retCnt = 0;
	UIntOS i = 0;
	UIntOS j;
	NN<Data::ArrayListDbl> ptX;
	NN<Data::ArrayListDbl> ptY;
	NN<Data::ArrayListObj<Optional<Map::SHPData::RecHdr>>> recs;
	if (this->ptX.SetTo(ptX) && this->ptY.SetTo(ptY) && (this->layerType == Map::DRAW_LAYER_POINT || this->layerType == Map::DRAW_LAYER_POINT3D))
	{
		Double x;
		Double y;
		j = ptX->GetCount();
		while (i < j)
		{
			x = ptX->GetItem(i);
			y = ptY->GetItem(i);
			if (rect.ContainPt(x, y))
			{
				outArr->Add((Int64)i + 1);
				retCnt++;
			}
			i++;
		}
		return retCnt;
	}
	else if (this->recs.SetTo(recs) && (this->layerType == Map::DRAW_LAYER_POLYGON || this->layerType == Map::DRAW_LAYER_POLYLINE || this->layerType == Map::DRAW_LAYER_POLYLINE3D))
	{
		NN<Map::SHPData::RecHdr> rec;
		j = recs->GetCount();
		while (i < j)
		{
			if (recs->GetItem(i).SetTo(rec))
			{
				if (rec->x2 >= rect.min.x && rec->x1 <= rect.max.x && rec->y2 >= rect.min.y && rec->y1 <= rect.max.y)
				{
					outArr->Add((Int64)i + 1);
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
	NN<Data::ArrayListDbl> ptX;
	NN<Data::ArrayListObj<Optional<Map::SHPData::RecHdr>>> recs;
	if (this->ptX.SetTo(ptX) && (this->layerType == Map::DRAW_LAYER_POINT || this->layerType == Map::DRAW_LAYER_POINT3D))
	{
		return (Int64)ptX->GetCount();
	}
	else if (this->recs.SetTo(recs) && (this->layerType == Map::DRAW_LAYER_POLYGON || this->layerType == Map::DRAW_LAYER_POLYLINE || this->layerType == Map::DRAW_LAYER_POLYLINE3D))
	{
		return (Int64)recs->GetCount();
	}
	else
	{
		return -1;
	}
}

UIntOS Map::SHPData::GetRecordCnt() const
{
	NN<Data::ArrayListDbl> ptX;
	NN<Data::ArrayListObj<Optional<Map::SHPData::RecHdr>>> recs;
	if (this->ptX.SetTo(ptX) && (this->layerType == Map::DRAW_LAYER_POINT || this->layerType == Map::DRAW_LAYER_POINT3D))
	{
		return ptX->GetCount();
	}
	else if (this->recs.SetTo(recs) && (this->layerType == Map::DRAW_LAYER_POLYGON || this->layerType == Map::DRAW_LAYER_POLYLINE || this->layerType == Map::DRAW_LAYER_POLYLINE3D))
	{
		return recs->GetCount();
	}
	else
	{
		return 0;
	}
}

void Map::SHPData::ReleaseNameArr(Optional<NameArray> nameArr)
{
}

Bool Map::SHPData::GetString(NN<Text::StringBuilderUTF8> sb, Optional<NameArray> nameArr, Int64 id, UIntOS colIndex)
{
	if (id <= 0)
		return false;
	if (colIndex == 0)
	{
		NN<Math::Geometry::Vector2D> vec;
		if (this->GetNewVectorById(NN<GetObjectSess>::ConvertFrom(NN<SHPData>(*this)), id).SetTo(vec))
		{
			Bool succ = Math::WKTWriter().ToText(sb, vec);
			vec.Delete();
			return succ;
		}
		return false;
	}
	NN<DB::DBFFile> dbf;
	if (!this->dbf.SetTo(dbf))
	{
		return false;
	}
	Bool ret = dbf->GetRecord(sb, (UIntOS)id - 1, colIndex - 1);
	sb->TrimSp();
	return ret;
}

UIntOS Map::SHPData::GetColumnCnt() const
{
	NN<DB::DBFFile> dbf;
	if (!this->dbf.SetTo(dbf))
	{
		return 1;
	}
	return dbf->GetColCount() + 1;
}

UnsafeArrayOpt<UTF8Char> Map::SHPData::GetColumnName(UnsafeArray<UTF8Char> buff, UIntOS colIndex) const
{
	if (colIndex == 0)
		return Text::StrConcatC(buff, UTF8STRC("Shape"));
	NN<DB::DBFFile> dbf;
	if (!this->dbf.SetTo(dbf))
	{
		return nullptr;
	}
	return dbf->GetColumnName(colIndex - 1, buff);
}

DB::DBUtil::ColType Map::SHPData::GetColumnType(UIntOS colIndex, OptOut<UIntOS> colSize) const
{
	if (colIndex == 0)
	{
		colSize.Set(Map::MapLayerReader::GetShapeColSize(this->layerType));
		return DB::DBUtil::CT_Vector;
	}
	NN<DB::DBFFile> dbf;
	if (!this->dbf.SetTo(dbf))
	{
		return DB::DBUtil::CT_Unknown;
	}
	return dbf->GetColumnType(colIndex - 1, colSize);
}

Bool Map::SHPData::GetColumnDef(UIntOS colIndex, NN<DB::ColDef> colDef) const
{
	if (colIndex == 0)
	{
		Map::MapLayerReader::GetShapeColDef(colDef, *this);
		return true;
	}
	NN<DB::DBFFile> dbf;
	if (!this->dbf.SetTo(dbf))
	{
		return false;
	}
	return dbf->GetColumnDef(colIndex - 1, colDef);
}

UInt32 Map::SHPData::GetCodePage() const
{
	NN<DB::DBFFile> dbf;
	if (!this->dbf.SetTo(dbf))
	{
		return 0;
	}
	return dbf->GetCodePage();
}

Bool Map::SHPData::GetBounds(OutParam<Math::RectAreaDbl> bounds) const
{
	bounds.Set(Math::RectAreaDbl(this->min, this->max));
	return this->min.x != 0 || this->min.y != 0 || this->max.x != 0 || this->max.y != 0;
}

NN<Map::GetObjectSess> Map::SHPData::BeginGetObject()
{
	return NN<GetObjectSess>::ConvertFrom(NN<SHPData>(*this));
}

void Map::SHPData::EndGetObject(NN<GetObjectSess> session)
{
}

Optional<Math::Geometry::Vector2D> Map::SHPData::GetNewVectorById(NN<GetObjectSess> session, Int64 id)
{
	NN<Map::SHPData::RecHdr> rec;
	NN<Sync::Mutex> mut;
	NN<Math::Geometry::Vector2D> vec;
	NN<IO::StreamData> shpData;
	if (!this->shpData.SetTo(shpData))
	{
		return nullptr;
	}

	NN<Data::ArrayListDbl> ptX;
	NN<Data::ArrayListDbl> ptY;
	NN<Data::ArrayListDbl> ptZ;
	NN<Data::ArrayListObj<Optional<Map::SHPData::RecHdr>>> recs;
	UInt32 srid = this->csys->GetSRID();
	if (this->layerType == Map::DRAW_LAYER_POINT && this->ptX.SetTo(ptX) && this->ptY.SetTo(ptY))
	{
		NN<Math::Geometry::Point> pt;
		if (id <= 0 || (UInt64)id > ptX->GetCount())
		{
			return nullptr;
		}
		NEW_CLASSNN(pt, Math::Geometry::Point(srid, ptX->GetItem((UIntOS)id - 1), ptY->GetItem((UIntOS)id - 1)));
		return pt;
	}
	else if (this->layerType == Map::DRAW_LAYER_POINT3D && this->ptX.SetTo(ptX) && this->ptY.SetTo(ptY) && this->ptZ.SetTo(ptZ))
	{
		NN<Math::Geometry::PointZ> pt;
		if (id <= 0 || (UInt64)id > ptX->GetCount())
		{
			return nullptr;
		}
		NEW_CLASSNN(pt, Math::Geometry::PointZ(srid, ptX->GetItem((UIntOS)id - 1), ptY->GetItem((UIntOS)id - 1), ptZ->GetItem((UIntOS)id - 1)));
		return pt;
	}
	else if (this->layerType == Map::DRAW_LAYER_POLYGON && this->recsMut.SetTo(mut) && this->recs.SetTo(recs))
	{
		NN<Math::Geometry::Polygon> pg;
		Sync::MutexUsage mutUsage(mut);
		if (!recs->GetItem((UIntOS)id - 1).SetTo(rec))
			return nullptr;
		if (rec->vec.SetTo(vec)) return vec->Clone();
		NEW_CLASSNN(pg, Math::Geometry::Polygon(srid));
		UnsafeArray<UInt32> ptOfstList = MemAllocArr(UInt32, rec->nPtOfst);
		UnsafeArray<Math::Coord2DDbl> pointList = MemAllocAArr(Math::Coord2DDbl, rec->nPoint);
		shpData->GetRealData(rec->ofst, rec->nPtOfst << 2, Data::ByteArray(UnsafeArray<UInt8>::ConvertFrom(ptOfstList), rec->nPtOfst << 2));
		shpData->GetRealData(rec->ofst + (rec->nPtOfst << 2), rec->nPoint << 4, Data::ByteArray(UnsafeArray<UInt8>::ConvertFrom(pointList), rec->nPoint << 4));
		pg->AddFromPtOfst(ptOfstList, rec->nPtOfst, pointList, rec->nPoint, nullptr, nullptr);
		MemFreeAArr(pointList);
		MemFreeArr(ptOfstList);
		rec->vec = pg->Clone();
		return pg;
	}
	else if (this->layerType == Map::DRAW_LAYER_POLYLINE && this->recsMut.SetTo(mut) && this->recs.SetTo(recs))
	{
		NN<Math::Geometry::Polyline> pl;
		Sync::MutexUsage mutUsage(mut);
		if (!recs->GetItem((UIntOS)id - 1).SetTo(rec))
			return nullptr;
		if (rec->vec.SetTo(vec)) return vec->Clone();
		NEW_CLASSNN(pl, Math::Geometry::Polyline(srid));
		UnsafeArray<UInt32> ptOfstList = MemAllocArr(UInt32, rec->nPtOfst);
		UnsafeArray<Math::Coord2DDbl> pointList = MemAllocAArr(Math::Coord2DDbl, rec->nPoint);
		shpData->GetRealData(rec->ofst, rec->nPtOfst << 2, Data::ByteArray(UnsafeArray<UInt8>::ConvertFrom(ptOfstList), rec->nPtOfst << 2));
		shpData->GetRealData(rec->ofst + (rec->nPtOfst << 2), rec->nPoint << 4, Data::ByteArray(UnsafeArray<UInt8>::ConvertFrom(pointList), rec->nPoint << 4));
		pl->AddFromPtOfst(ptOfstList, rec->nPtOfst, pointList, rec->nPoint, nullptr, nullptr);
		MemFreeAArr(pointList);
		MemFreeArr(ptOfstList);
		rec->vec = pl->Clone();
		return pl;
	}
	else if (this->layerType == Map::DRAW_LAYER_POLYLINE3D && this->recsMut.SetTo(mut) && this->recs.SetTo(recs))
	{
		NN<Math::Geometry::Polyline> pl;
		Sync::MutexUsage mutUsage(mut);
		if (!recs->GetItem((UIntOS)id - 1).SetTo(rec))
			return nullptr;
		if (rec->vec.SetTo(vec)) return vec->Clone();
		NEW_CLASSNN(pl, Math::Geometry::Polyline(srid));
		UnsafeArray<UInt32> ptOfstList = MemAllocArr(UInt32, rec->nPtOfst);
		UnsafeArray<Math::Coord2DDbl> pointList = MemAllocAArr(Math::Coord2DDbl, rec->nPoint);
		UnsafeArray<Double> zList = MemAllocArr(Double, rec->nPoint);
		shpData->GetRealData(rec->ofst, rec->nPtOfst << 2, Data::ByteArray(UnsafeArray<UInt8>::ConvertFrom(ptOfstList), rec->nPtOfst << 2));
		shpData->GetRealData(rec->ofst + (rec->nPtOfst << 2), rec->nPoint << 4, Data::ByteArray(UnsafeArray<UInt8>::ConvertFrom(pointList), rec->nPoint << 4));
		shpData->GetRealData(rec->endOfst - (rec->nPoint << 3), rec->nPoint << 3, Data::ByteArray(UnsafeArray<UInt8>::ConvertFrom(zList), rec->nPoint << 3));
		pl->AddFromPtOfst(ptOfstList, rec->nPtOfst, pointList, rec->nPoint, zList, nullptr);
		MemFreeArr(zList);
		MemFreeAArr(pointList);
		MemFreeArr(ptOfstList);
		rec->vec = pl->Clone();
		return pl;
	}
	else
	{
		return nullptr;
	}
}

Map::MapDrawLayer::FailReason Map::SHPData::GetFailReason() const
{
	return Map::MapDrawLayer::FailReason::IdNotFound;
}

UIntOS Map::SHPData::QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names)
{
	NN<DB::DBFFile> dbf;
	if (!this->dbf.SetTo(dbf))
	{
		return 0;
	}
	return dbf->QueryTableNames(schemaName, names);
}

//Optional<DB::DBReader> Map::SHPData::QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> columNames, UIntOS ofst, UIntOS maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition)
//{
//	return this->dbf->QueryTableData(schemaName, tableName, columNames, ofst, maxCnt, ordering, condition);
//}

Optional<DB::TableDef> Map::SHPData::GetTableDef(Text::CString schemaName, Text::CStringNN tableName)
{
	NN<DB::TableDef> tab;
	NEW_CLASSNN(tab, DB::TableDef(schemaName, tableName));
	this->AddColDefs(tab);
	return tab;
}

void Map::SHPData::CloseReader(NN<DB::DBReader> r)
{
	NN<DB::DBFFile> dbf;
	if (!this->dbf.SetTo(dbf))
	{
		return;
	}
	dbf->CloseReader(r);
}

void Map::SHPData::GetLastErrorMsg(NN<Text::StringBuilderUTF8> str)
{
	NN<DB::DBFFile> dbf;
	if (!this->dbf.SetTo(dbf))
	{
		return;
	}
	dbf->GetLastErrorMsg(str);

}

void Map::SHPData::Reconnect()
{
	NN<DB::DBFFile> dbf;
	if (!this->dbf.SetTo(dbf))
	{
		return;
	}
	dbf->Reconnect();
}

UIntOS Map::SHPData::GetGeomCol() const
{
	return 0;
}

Map::MapDrawLayer::ObjectClass Map::SHPData::GetObjectClass() const
{
	return Map::MapDrawLayer::OC_SHP_DATA;
}
