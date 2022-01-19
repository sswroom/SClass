#include "Stdafx.h"
#include "Map/SHPUtil.h"
#include "Map/ESRI/ESRIMDBLayer.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Math.h"
#include "Math/Point3D.h"
#include "Math/Polyline3D.h"

Data::Int32Map<const UTF8Char **> *Map::ESRI::ESRIMDBLayer::ReadNameArr()
{
	UTF8Char sbuff[512];
	Sync::MutexUsage mutUsage;
	this->currDB = this->conn->UseConn(&mutUsage);
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

void Map::ESRI::ESRIMDBLayer::Init(DB::SharedDBConn *conn, UInt32 srid, const UTF8Char *tableName)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	UInt8 *buff = 0; 
	UOSInt buffSize = 0;
	UOSInt currSize;
	conn->UseObject();
	this->conn = conn;
	NEW_CLASS(this->objects, Data::Int32Map<Math::Vector2D*>());
	NEW_CLASS(this->colNames, Data::ArrayListStrUTF8());
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
	this->currDB = this->conn->UseConn(&mutUsage);
	DB::DBReader *r = this->currDB->GetTableData(tableName, 0, 0, 0, 0, 0);
	if (r)
	{
		UOSInt i;
		UOSInt j;
		i = 0;
		j = r->ColCount();
		while (i < j)
		{
			sbuff[0] = 0;
			sptr = r->GetName(i, sbuff);
			if (Text::StrEqualsICaseC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("SHAPE")))
			{
				this->shapeCol = i;
			}
			else if (Text::StrEqualsICaseC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("OBJECTID")))
			{
				this->objIdCol = i;
			}
			this->colNames->Add(Text::StrCopyNew(sbuff));
			i++;
		}
		j = this->colNames->GetCount();
		while (j-- > 0)
		{
			if (Text::StrEndsWithICase(this->colNames->GetItem(j), (const UTF8Char*)"NAME"))
			{
				nameCol = j;
			}
		}
		this->SetNameCol(nameCol);
		while (r->ReadNext())
		{
			Int32 objId;
			Math::Vector2D *vec;

			currSize = r->GetBinarySize(this->shapeCol);
			if (currSize > buffSize)
			{
				if (buff)
				{
					MemFree(buff);
				}
				buffSize = currSize;
				buff = MemAlloc(UInt8, buffSize);
			}
			r->GetBinary(this->shapeCol, buff);
			objId = r->GetInt32(this->objIdCol);
			vec = Map::SHPUtil::ParseShpRecord(srid, buff, buffSize);
			if (vec)
			{
				Double thisMaxX;
				Double thisMinX;
				Double thisMaxY;
				Double thisMinY;
				this->objects->Put(objId, vec);
				vec->GetBounds(&thisMinX, &thisMinY, &thisMaxX, &thisMaxY);
				if (this->minX == 0 && this->minY == 0 && this->maxX == 0 && this->maxY == 0)
				{
					maxX = thisMaxX;
					minX = thisMinX;
					maxY = thisMaxY;
					minY = thisMinY;
				}
				else
				{
					if (maxX < thisMaxX)
						maxX = thisMaxX;
					if (minX > thisMinX)
						minX = thisMinX;
					if (maxY < thisMaxY)
						maxY = thisMaxY;
					if (minY > thisMinY)
						minY = thisMinY;
				}

				if (this->layerType == Map::DRAW_LAYER_UNKNOWN)
				{
					Math::Vector2D::VectorType vt = vec->GetVectorType();
					if (vt == Math::Vector2D::VectorType::Point)
					{
						if (vec->Support3D())
						{
							this->layerType = Map::DRAW_LAYER_POINT3D;
						}
						else
						{
							this->layerType = Map::DRAW_LAYER_POINT;
						}
					}
					else if (vt == Math::Vector2D::VectorType::Polyline)
					{
						if (vec->Support3D())
						{
							this->layerType = Map::DRAW_LAYER_POLYLINE3D;
						}
						else
						{
							this->layerType = Map::DRAW_LAYER_POLYLINE;
						}
					}
					else if (vt == Math::Vector2D::VectorType::Polygon)
					{
						this->layerType = Map::DRAW_LAYER_POLYGON;
					}
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
	this->csys = Math::CoordinateSystemManager::SRCreateCSys(srid);
}

Map::ESRI::ESRIMDBLayer::ESRIMDBLayer(DB::SharedDBConn *conn, UInt32 srid, Text::String *sourceName, const UTF8Char *tableName) : Map::IMapDrawLayer(sourceName, 0, Text::String::NewOrNull(tableName))
{
	SDEL_STRING(this->layerName);
	this->Init(conn, srid, tableName);
}

Map::ESRI::ESRIMDBLayer::ESRIMDBLayer(DB::SharedDBConn *conn, UInt32 srid, const UTF8Char *sourceName, const UTF8Char *tableName) : Map::IMapDrawLayer(sourceName, 0, tableName)
{
	this->Init(conn, srid, tableName);
}

Map::ESRI::ESRIMDBLayer::~ESRIMDBLayer()
{
	UOSInt i;

	this->conn->UnuseObject();
	i = this->colNames->GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(this->colNames->RemoveAt(i));
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

Map::DrawLayerType Map::ESRI::ESRIMDBLayer::GetLayerType()
{
	return this->layerType;
}

UOSInt Map::ESRI::ESRIMDBLayer::GetAllObjectIds(Data::ArrayListInt64 *outArr, void **nameArr)
{
	if (nameArr)
	{
		*nameArr = ReadNameArr();
	}
	outArr->AddRangeI32(this->objects->GetKeys());
	return this->objects->GetCount();
}

UOSInt Map::ESRI::ESRIMDBLayer::GetObjectIds(Data::ArrayListInt64 *outArr, void **nameArr, Double mapRate, Int32 x1, Int32 y1, Int32 x2, Int32 y2, Bool keepEmpty)
{
	return GetObjectIdsMapXY(outArr, nameArr, x1 / mapRate, y1 / mapRate, x2 / mapRate, y2 / mapRate, keepEmpty);
}

UOSInt Map::ESRI::ESRIMDBLayer::GetObjectIdsMapXY(Data::ArrayListInt64 *outArr, void **nameArr, Double x1, Double y1, Double x2, Double y2, Bool keepEmpty)
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

Int64 Map::ESRI::ESRIMDBLayer::GetObjectIdMax()
{
	Data::ArrayList<Int32> *objInd = this->objects->GetKeys();
	return objInd->GetItem(objInd->GetCount() - 1);
}

void Map::ESRI::ESRIMDBLayer::ReleaseNameArr(void *nameArr)
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

UTF8Char *Map::ESRI::ESRIMDBLayer::GetString(UTF8Char *buff, UOSInt buffSize, void *nameArr, Int64 id, UOSInt strIndex)
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

UOSInt Map::ESRI::ESRIMDBLayer::GetColumnCnt()
{
	return this->colNames->GetCount();
}

UTF8Char *Map::ESRI::ESRIMDBLayer::GetColumnName(UTF8Char *buff, UOSInt colIndex)
{
	const UTF8Char *colName = this->colNames->GetItem(colIndex);
	if (colName)
	{
		return Text::StrConcat(buff, colName);
	}
	return 0;
}

DB::DBUtil::ColType Map::ESRI::ESRIMDBLayer::GetColumnType(UOSInt colIndex, UOSInt *colSize)
{
	return DB::DBUtil::CT_Unknown;
}

Bool Map::ESRI::ESRIMDBLayer::GetColumnDef(UOSInt colIndex, DB::ColDef *colDef)
{
	return false;
}

UInt32 Map::ESRI::ESRIMDBLayer::GetCodePage()
{
	return 65001;
}

Bool Map::ESRI::ESRIMDBLayer::GetBoundsDbl(Double *minX, Double *minY, Double *maxX, Double *maxY)
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

void *Map::ESRI::ESRIMDBLayer::BeginGetObject()
{
	return (void*)-1;
}

void Map::ESRI::ESRIMDBLayer::EndGetObject(void *session)
{
}

Map::DrawObjectL *Map::ESRI::ESRIMDBLayer::GetObjectByIdD(void *session, Int64 id)
{
	Math::Vector2D *vec = this->objects->Get((Int32)id);
	return Vector2DrawObject(id, vec, this->layerType);
}

Math::Vector2D *Map::ESRI::ESRIMDBLayer::GetVectorById(void *session, Int64 id)
{
	Math::Vector2D *vec = this->objects->Get((Int32)id);
	if (vec)
		return vec->Clone();
	return 0;
}

void Map::ESRI::ESRIMDBLayer::ReleaseObject(void *session, Map::DrawObjectL *obj)
{
	MemFree(obj->ptOfstArr);
	MemFree(obj->pointArr);
	MemFree(obj);
}

void Map::ESRI::ESRIMDBLayer::AddUpdatedHandler(UpdatedHandler hdlr, void *obj)
{
}

void Map::ESRI::ESRIMDBLayer::RemoveUpdatedHandler(UpdatedHandler hdlr, void *obj)
{
}

UOSInt Map::ESRI::ESRIMDBLayer::GetTableNames(Data::ArrayList<const UTF8Char*> *names)
{
	names->Add(this->tableName);
	return 1;
}

DB::DBReader *Map::ESRI::ESRIMDBLayer::GetTableData(const UTF8Char *name)
{
	Sync::MutexUsage *mutUsage;
	NEW_CLASS(mutUsage, Sync::MutexUsage());
	this->currDB = this->conn->UseConn(mutUsage);
	this->lastDB = this->currDB;
	DB::DBReader *rdr = this->currDB->GetTableData(name, 0, 0, 0, 0, 0);
	if (rdr)
	{
		Map::ESRI::ESRIMDBReader *r;
		NEW_CLASS(r, Map::ESRI::ESRIMDBReader(this->currDB, rdr, mutUsage));
		return r;
	}
	DEL_CLASS(mutUsage);
	this->currDB = 0;
	return 0;
}

void Map::ESRI::ESRIMDBLayer::CloseReader(DB::DBReader *r)
{
	Map::ESRI::ESRIMDBReader *rdr = (Map::ESRI::ESRIMDBReader*)r;
	DEL_CLASS(rdr);
	this->currDB = 0;
}

void Map::ESRI::ESRIMDBLayer::GetErrorMsg(Text::StringBuilderUTF *str)
{
	if (this->lastDB)
	{
		this->lastDB->GetErrorMsg(str);
	}
}

void Map::ESRI::ESRIMDBLayer::Reconnect()
{
	this->conn->Reconnect();
}

Map::IMapDrawLayer::ObjectClass Map::ESRI::ESRIMDBLayer::GetObjectClass()
{
	return Map::IMapDrawLayer::OC_ESRI_MDB_LAYER;
}

Map::ESRI::ESRIMDBReader::ESRIMDBReader(DB::DBConn *conn, DB::DBReader *r, Sync::MutexUsage *mutUsage)
{
	this->conn = conn;
	this->r = r;
	this->mutUsage = mutUsage;
}

Map::ESRI::ESRIMDBReader::~ESRIMDBReader()
{
	this->conn->CloseReader(this->r);
	DEL_CLASS(this->mutUsage);
}

Bool Map::ESRI::ESRIMDBReader::ReadNext()
{
	return this->r->ReadNext();
}

UOSInt Map::ESRI::ESRIMDBReader::ColCount()
{
	return this->r->ColCount() - 1;
}

OSInt Map::ESRI::ESRIMDBReader::GetRowChanged()
{
	return this->r->GetRowChanged();
}

Int32 Map::ESRI::ESRIMDBReader::GetInt32(UOSInt colIndex)
{
	return this->r->GetInt32((colIndex > 0)?(colIndex + 1):colIndex);
}

Int64 Map::ESRI::ESRIMDBReader::GetInt64(UOSInt colIndex)
{
	return this->r->GetInt64((colIndex > 0)?(colIndex + 1):colIndex);
}

WChar *Map::ESRI::ESRIMDBReader::GetStr(UOSInt colIndex, WChar *buff)
{
	return this->r->GetStr((colIndex > 0)?(colIndex + 1):colIndex, buff);
}

Bool Map::ESRI::ESRIMDBReader::GetStr(UOSInt colIndex, Text::StringBuilderUTF *sb)
{
	return this->r->GetStr((colIndex > 0)?(colIndex + 1):colIndex, sb);
}

Text::String *Map::ESRI::ESRIMDBReader::GetNewStr(UOSInt colIndex)
{
	return this->r->GetNewStr((colIndex > 0)?(colIndex + 1):colIndex);
}

UTF8Char *Map::ESRI::ESRIMDBReader::GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize)
{
	return this->r->GetStr((colIndex > 0)?(colIndex + 1):colIndex, buff, buffSize);
}

DB::DBReader::DateErrType Map::ESRI::ESRIMDBReader::GetDate(UOSInt colIndex, Data::DateTime *outVal)
{
	return this->r->GetDate((colIndex > 0)?(colIndex + 1):colIndex, outVal);
}

Double Map::ESRI::ESRIMDBReader::GetDbl(UOSInt colIndex)
{
	return this->r->GetDbl((colIndex > 0)?(colIndex + 1):colIndex);
}

Bool Map::ESRI::ESRIMDBReader::GetBool(UOSInt colIndex)
{
	return this->r->GetBool((colIndex > 0)?(colIndex + 1):colIndex);
}

UOSInt Map::ESRI::ESRIMDBReader::GetBinarySize(UOSInt colIndex)
{
	return this->r->GetBinarySize((colIndex > 0)?(colIndex + 1):colIndex);
}

UOSInt Map::ESRI::ESRIMDBReader::GetBinary(UOSInt colIndex, UInt8 *buff)
{
	return this->r->GetBinary((colIndex > 0)?(colIndex + 1):colIndex, buff);
}

Math::Vector2D *Map::ESRI::ESRIMDBReader::GetVector(UOSInt colIndex)
{
	return this->r->GetVector(colIndex);
}

Bool Map::ESRI::ESRIMDBReader::GetUUID(UOSInt colIndex, Data::UUID *uuid)
{
	return this->r->GetUUID(colIndex, uuid);
}

Bool Map::ESRI::ESRIMDBReader::IsNull(UOSInt colIndex)
{
	return this->r->IsNull((colIndex > 0)?(colIndex + 1):colIndex);
}

UTF8Char *Map::ESRI::ESRIMDBReader::GetName(UOSInt colIndex, UTF8Char *buff)
{
	return this->r->GetName((colIndex > 0)?(colIndex + 1):colIndex, buff);
}

DB::DBUtil::ColType Map::ESRI::ESRIMDBReader::GetColType(UOSInt colIndex, UOSInt *colSize)
{
	return this->r->GetColType((colIndex > 0)?(colIndex + 1):colIndex, colSize);
}

Bool Map::ESRI::ESRIMDBReader::GetColDef(UOSInt colIndex, DB::ColDef *colDef)
{
	return this->r->GetColDef((colIndex > 0)?(colIndex + 1):colIndex, colDef);
}
