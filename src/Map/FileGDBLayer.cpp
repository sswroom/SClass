#include "Stdafx.h"
#include "DB/ColDef.h"
#include "Map/FileGDBLayer.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Math.h"
#include "Math/Point3D.h"
#include "Math/Polyline3D.h"

Data::Int32Map<const UTF8Char **> *Map::FileGDBLayer::ReadNameArr()
{
	UTF8Char sbuff[512];
	Sync::MutexUsage mutUsage;
	this->currDB = this->conn->UseDB(&mutUsage);
	DB::DBReader *r = this->currDB->GetTableData(tableName, 0, 0, 0, 0, 0);
	if (r)
	{
		Data::Int32Map<const UTF8Char **> *nameArr;
		const UTF8Char **names;
		UOSInt colCnt = this->colNames->GetCount();
		UOSInt i;
		Int32 objId;

		NEW_CLASS(nameArr, Data::Int32Map<const UTF8Char **>());
		while (r->ReadNext())
		{
			objId = r->GetInt32(this->objIdCol);
			names = MemAlloc(const UTF8Char *, colCnt);
			i = 0;
			while (i < colCnt)
			{
				if (i == this->shapeCol)
				{
					names[i] = 0;
				}
				else if (r->GetStr(i, sbuff, sizeof(sbuff)))
				{
					names[i] = Text::StrCopyNew(sbuff);
				}
				else
				{
					names[i] = 0;
				}
				i++;
			}
			nameArr->Put(objId, names);
		}
		this->currDB->CloseReader(r);
		mutUsage.EndUse();
		this->currDB = 0;
		return nameArr;
	}
	else
	{
		mutUsage.EndUse();
		this->currDB = 0;
		return 0;
	}
}

Map::FileGDBLayer::FileGDBLayer(DB::SharedReadingDB *conn, const UTF8Char *sourceName, const UTF8Char *tableName) : Map::IMapDrawLayer(sourceName, 0, tableName)
{
	UInt8 *buff = 0; 
	conn->UseObject();
	this->conn = conn;
	NEW_CLASS(this->objects, Data::Int32Map<Math::Vector2D*>());
	NEW_CLASS(this->colNames, Data::ArrayList<Text::String*>());
	this->tableName = Text::StrCopyNew(tableName);
	this->currDB = 0;
	this->lastDB = 0;
	this->layerType = Map::DRAW_LAYER_UNKNOWN;
	this->maxX = 0;
	this->minX = 0;
	this->maxY = 0;
	this->minY = 0;
	this->objIdCol = 0;
	this->shapeCol = 1;
	UOSInt nameCol = 0;

	Sync::MutexUsage mutUsage;
	this->currDB = this->conn->UseDB(&mutUsage);
	this->csys = Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::CoordinateSystemManager::GCST_WGS84);
	DB::DBReader *r = this->currDB->GetTableData(tableName, 0, 0, 0, 0, 0);
	if (r)
	{
		UOSInt i;
		UOSInt j;
		DB::ColDef *colDef;
		NEW_CLASS(colDef, DB::ColDef((const UTF8Char*)""));
		i = 0;
		j = r->ColCount();
		while (i < j)
		{
			r->GetColDef(i, colDef);
			if (colDef->GetColType() == DB::DBUtil::CT_Vector)
			{
				this->shapeCol = i;
				Text::String *prj = colDef->GetAttr();
				if (prj && prj->v[0])
				{
					Math::CoordinateSystem *csys2 = 0;
					if (Text::StrStartsWith(prj->v, (const UTF8Char*)"EPSG:"))
					{
						csys2 = Math::CoordinateSystemManager::SRCreateCSys(Text::StrToUInt32(&prj->v[5]));
					}
					else
					{
						csys2 = Math::CoordinateSystemManager::ParsePRJFile(prj->v);
					}
					if (csys2)
					{
						DEL_CLASS(this->csys);
						this->csys = csys2;
					}
				}
			}
			else if (colDef->IsPK() && colDef->GetColType() == DB::DBUtil::CT_Int32)
			{
				this->objIdCol = i;
			}
			this->colNames->Add(colDef->GetColName()->Clone());
			i++;
		}
		j = this->colNames->GetCount();
		while (j-- > 0)
		{
			if (Text::StrEndsWithICase(this->colNames->GetItem(j)->v, (const UTF8Char*)"NAME"))
			{
				nameCol = j;
			}
		}
		this->SetNameCol(nameCol);
		DEL_CLASS(colDef);
		while (r->ReadNext())
		{
			Int32 objId;
			Math::Vector2D *vec;
			Double bounds[4];

			objId = r->GetInt32(this->objIdCol);
			vec = r->GetVector(this->shapeCol);
			if (vec)
			{
				this->objects->Put(objId, vec);

				vec->GetBounds(&bounds[0], &bounds[1], &bounds[2], &bounds[3]);
				if (this->minX == 0 && this->minY == 0 && this->maxX == 0 && this->maxY == 0)
				{
					this->minX = bounds[0];
					this->minY = bounds[1];
					this->maxX = bounds[2];
					this->maxY = bounds[3];
				}
				else
				{
					if (maxX < bounds[2])
						maxX = bounds[2];
					if (minX > bounds[0])
						minX = bounds[0];
					if (maxY < bounds[3])
						maxY = bounds[3];
					if (minY > bounds[1])
						minY = bounds[1];
				}
				if (this->layerType == Map::DRAW_LAYER_UNKNOWN)
				{
					this->layerType = Map::IMapDrawLayer::VectorType2LayerType(vec->GetVectorType());
				}
			}
		}
		if (buff)
		{
			MemFree(buff);
		}
		this->currDB->CloseReader(r);
	}
	mutUsage.EndUse();
	this->currDB = 0;
}

Map::FileGDBLayer::~FileGDBLayer()
{
	UOSInt i;

	this->conn->UnuseObject();
	i = this->colNames->GetCount();
	while (i-- > 0)
	{
		this->colNames->RemoveAt(i)->Release();
	}
	DEL_CLASS(this->colNames);
	Data::ArrayList<Math::Vector2D*> *vecList = this->objects->GetValues();
	Math::Vector2D *vec;
	i = vecList->GetCount();
	while (i-- > 0)
	{
		vec = vecList->GetItem(i);
		DEL_CLASS(vec);
	}
	DEL_CLASS(this->objects);
	Text::StrDelNew(tableName);
}

Map::DrawLayerType Map::FileGDBLayer::GetLayerType()
{
	return this->layerType;
}

UOSInt Map::FileGDBLayer::GetAllObjectIds(Data::ArrayListInt64 *outArr, void **nameArr)
{
	if (nameArr)
	{
		*nameArr = ReadNameArr();
	}
	outArr->AddRangeI32(this->objects->GetKeys());
	return this->objects->GetCount();
}

UOSInt Map::FileGDBLayer::GetObjectIds(Data::ArrayListInt64 *outArr, void **nameArr, Double mapRate, Int32 x1, Int32 y1, Int32 x2, Int32 y2, Bool keepEmpty)
{
	return GetObjectIdsMapXY(outArr, nameArr, x1 / mapRate, y1 / mapRate, x2 / mapRate, y2 / mapRate, keepEmpty);
}

UOSInt Map::FileGDBLayer::GetObjectIdsMapXY(Data::ArrayListInt64 *outArr, void **nameArr, Double x1, Double y1, Double x2, Double y2, Bool keepEmpty)
{
	if (nameArr)
	{
		*nameArr = ReadNameArr();
	}
	UOSInt cnt = 0;
	Data::ArrayList<Math::Vector2D*> *vecList = this->objects->GetValues();
	Data::SortableArrayList<Int32> *vecKeys = this->objects->GetKeys();
	Double minX;
	Double minY;
	Double maxX;
	Double maxY;
	Math::Vector2D *vec;
	UOSInt i;
	UOSInt j;
	i = 0;
	j = vecList->GetCount();
	while (i < j)
	{
		vec = vecList->GetItem(i);
		vec->GetBounds(&minX, &minY, &maxX, &maxY);
		if (x1 <= maxX && x2 >= minX && y1 <= maxY && y2 >= minY)
		{
			outArr->Add(vecKeys->GetItem(i));
			cnt++;
		}
		i++;
	}
	return cnt;
}

Int64 Map::FileGDBLayer::GetObjectIdMax()
{
	Data::ArrayList<Int32> *objInd = this->objects->GetKeys();
	return objInd->GetItem(objInd->GetCount() - 1);
}

void Map::FileGDBLayer::ReleaseNameArr(void *nameArr)
{
	Data::Int32Map<const UTF8Char **> *names = (Data::Int32Map<const UTF8Char **> *)nameArr;
	Data::ArrayList<const UTF8Char **> *nameList = names->GetValues();
	UOSInt i = nameList->GetCount();
	UOSInt colCnt = this->colNames->GetCount();
	UOSInt j;
	const UTF8Char **nameStrs;
	while (i-- > 0)
	{
		nameStrs = nameList->GetItem(i);
		j = colCnt;
		while (j-- > 0)
		{
			if (nameStrs[j])
				Text::StrDelNew(nameStrs[j]);
		}
		MemFree(nameStrs);
	}
	DEL_CLASS(names);
}

UTF8Char *Map::FileGDBLayer::GetString(UTF8Char *buff, UOSInt buffSize, void *nameArr, Int64 id, UOSInt strIndex)
{
	Data::Int32Map<const UTF8Char **> *names = (Data::Int32Map<const UTF8Char **> *)nameArr;
	if (names == 0)
		return 0;
	const UTF8Char **nameStrs = names->Get((Int32)id);
	if (nameStrs == 0)
		return 0;
	if (nameStrs[strIndex] == 0)
		return 0;
	return Text::StrConcatS(buff, nameStrs[strIndex], buffSize);
}

UOSInt Map::FileGDBLayer::GetColumnCnt()
{
	return this->colNames->GetCount();
}

UTF8Char *Map::FileGDBLayer::GetColumnName(UTF8Char *buff, UOSInt colIndex)
{
	Text::String *colName = this->colNames->GetItem(colIndex);
	if (colName)
	{
		return Text::StrConcatC(buff, colName->v, colName->leng);
	}
	return 0;
}

DB::DBUtil::ColType Map::FileGDBLayer::GetColumnType(UOSInt colIndex, UOSInt *colSize)
{
	return DB::DBUtil::CT_Unknown;
}

Bool Map::FileGDBLayer::GetColumnDef(UOSInt colIndex, DB::ColDef *colDef)
{
	return false;
}

UInt32 Map::FileGDBLayer::GetCodePage()
{
	return 65001;
}

Bool Map::FileGDBLayer::GetBoundsDbl(Double *minX, Double *minY, Double *maxX, Double *maxY)
{
	if (minX)
		*minX = this->minX;
	if (minY)
		*minY = this->minY;
	if (maxX)
		*maxX = this->maxX;
	if (maxY)
		*maxY = this->maxY;
	return this->minX != 0 || this->minY != 0 || this->maxX != 0 || this->maxY != 0;
}

void *Map::FileGDBLayer::BeginGetObject()
{
	return (void*)-1;
}

void Map::FileGDBLayer::EndGetObject(void *session)
{
}

Map::DrawObjectL *Map::FileGDBLayer::GetObjectByIdD(void *session, Int64 id)
{
	Math::Vector2D *vec = this->objects->Get((Int32)id);
	return Vector2DrawObject(id, vec, this->layerType);
}

Math::Vector2D *Map::FileGDBLayer::GetVectorById(void *session, Int64 id)
{
	Math::Vector2D *vec = this->objects->Get((Int32)id);
	if (vec)
		return vec->Clone();
	return 0;
}

void Map::FileGDBLayer::ReleaseObject(void *session, Map::DrawObjectL *obj)
{
	MemFree(obj->ptOfstArr);
	MemFree(obj->pointArr);
	MemFree(obj);
}

void Map::FileGDBLayer::AddUpdatedHandler(UpdatedHandler hdlr, void *obj)
{
}

void Map::FileGDBLayer::RemoveUpdatedHandler(UpdatedHandler hdlr, void *obj)
{
}

UOSInt Map::FileGDBLayer::GetTableNames(Data::ArrayList<const UTF8Char*> *names)
{
	names->Add(this->tableName);
	return 1;
}

DB::DBReader *Map::FileGDBLayer::GetTableData(const UTF8Char *tableName, Data::ArrayList<const UTF8Char*> *columnNames, UOSInt ofst, UOSInt maxCnt, const UTF8Char *ordering, Data::QueryConditions *condition)
{
	Sync::MutexUsage *mutUsage;
	NEW_CLASS(mutUsage, Sync::MutexUsage());
	this->currDB = this->conn->UseDB(mutUsage);
	this->lastDB = this->currDB;
	DB::DBReader *rdr = this->currDB->GetTableData(tableName, columnNames, ofst, maxCnt, ordering, condition);
	if (rdr)
	{
		Map::FileGDBLReader *r;
		NEW_CLASS(r, Map::FileGDBLReader(this->currDB, rdr, mutUsage));
		return r;
	}
	DEL_CLASS(mutUsage);
	this->currDB = 0;
	return 0;
}

void Map::FileGDBLayer::CloseReader(DB::DBReader *r)
{
	Map::FileGDBLReader *rdr = (Map::FileGDBLReader*)r;
	DEL_CLASS(rdr);
	this->currDB = 0;
}

void Map::FileGDBLayer::GetErrorMsg(Text::StringBuilderUTF *str)
{
	if (this->lastDB)
	{
		this->lastDB->GetErrorMsg(str);
	}
}

void Map::FileGDBLayer::Reconnect()
{
	this->conn->Reconnect();
}

Map::IMapDrawLayer::ObjectClass Map::FileGDBLayer::GetObjectClass()
{
	return Map::IMapDrawLayer::OC_ESRI_MDB_LAYER;
}

Map::FileGDBLReader::FileGDBLReader(DB::ReadingDB *conn, DB::DBReader *r, Sync::MutexUsage *mutUsage)
{
	this->conn = conn;
	this->r = r;
	this->mutUsage = mutUsage;
}

Map::FileGDBLReader::~FileGDBLReader()
{
	this->conn->CloseReader(r);
	DEL_CLASS(this->mutUsage);
}

Bool Map::FileGDBLReader::ReadNext()
{
	return this->r->ReadNext();
}

UOSInt Map::FileGDBLReader::ColCount()
{
	return this->r->ColCount() - 1;
}

OSInt Map::FileGDBLReader::GetRowChanged()
{
	return this->r->GetRowChanged();
}

Int32 Map::FileGDBLReader::GetInt32(UOSInt colIndex)
{
	return this->r->GetInt32((colIndex > 0)?(colIndex + 1):colIndex);
}

Int64 Map::FileGDBLReader::GetInt64(UOSInt colIndex)
{
	return this->r->GetInt64((colIndex > 0)?(colIndex + 1):colIndex);
}

WChar *Map::FileGDBLReader::GetStr(UOSInt colIndex, WChar *buff)
{
	return this->r->GetStr((colIndex > 0)?(colIndex + 1):colIndex, buff);
}

Bool Map::FileGDBLReader::GetStr(UOSInt colIndex, Text::StringBuilderUTF *sb)
{
	return this->r->GetStr((colIndex > 0)?(colIndex + 1):colIndex, sb);
}

Text::String *Map::FileGDBLReader::GetNewStr(UOSInt colIndex)
{
	return this->r->GetNewStr((colIndex > 0)?(colIndex + 1):colIndex);
}

UTF8Char *Map::FileGDBLReader::GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize)
{
	return this->r->GetStr((colIndex > 0)?(colIndex + 1):colIndex, buff, buffSize);
}

DB::DBReader::DateErrType Map::FileGDBLReader::GetDate(UOSInt colIndex, Data::DateTime *outVal)
{
	return this->r->GetDate((colIndex > 0)?(colIndex + 1):colIndex, outVal);
}

Double Map::FileGDBLReader::GetDbl(UOSInt colIndex)
{
	return this->r->GetDbl((colIndex > 0)?(colIndex + 1):colIndex);
}

Bool Map::FileGDBLReader::GetBool(UOSInt colIndex)
{
	return this->r->GetBool((colIndex > 0)?(colIndex + 1):colIndex);
}

UOSInt Map::FileGDBLReader::GetBinarySize(UOSInt colIndex)
{
	return this->r->GetBinarySize((colIndex > 0)?(colIndex + 1):colIndex);
}

UOSInt Map::FileGDBLReader::GetBinary(UOSInt colIndex, UInt8 *buff)
{
	return this->r->GetBinary((colIndex > 0)?(colIndex + 1):colIndex, buff);
}

Math::Vector2D *Map::FileGDBLReader::GetVector(UOSInt colIndex)
{
	return this->r->GetVector(colIndex);
}

Bool Map::FileGDBLReader::GetUUID(UOSInt colIndex, Data::UUID *uuid)
{
	return this->r->GetUUID(colIndex, uuid);
}

Bool Map::FileGDBLReader::IsNull(UOSInt colIndex)
{
	return this->r->IsNull((colIndex > 0)?(colIndex + 1):colIndex);
}

UTF8Char *Map::FileGDBLReader::GetName(UOSInt colIndex, UTF8Char *buff)
{
	return this->r->GetName((colIndex > 0)?(colIndex + 1):colIndex, buff);
}

DB::DBUtil::ColType Map::FileGDBLReader::GetColType(UOSInt colIndex, UOSInt *colSize)
{
	return this->r->GetColType((colIndex > 0)?(colIndex + 1):colIndex, colSize);
}

Bool Map::FileGDBLReader::GetColDef(UOSInt colIndex, DB::ColDef *colDef)
{
	return this->r->GetColDef((colIndex > 0)?(colIndex + 1):colIndex, colDef);
}
