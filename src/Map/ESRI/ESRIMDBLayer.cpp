#include "Stdafx.h"
#include "Map/SHPUtil.h"
#include "Map/ESRI/ESRIMDBLayer.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Math.h"
#include "Math/Geometry/PointZ.h"
#include "Math/Geometry/Polyline.h"

Data::FastMap<Int32, const UTF8Char **> *Map::ESRI::ESRIMDBLayer::ReadNameArr()
{
	UTF8Char sbuff[512];
	Sync::MutexUsage mutUsage;
	this->currDB = this->conn->UseConn(mutUsage).Ptr();
	NotNullPtr<DB::DBReader> r;
	if (r.Set(this->currDB->QueryTableData(CSTR_NULL, this->tableName->ToCString(), 0, 0, 0, CSTR_NULL, 0)))
	{
		Data::FastMap<Int32, const UTF8Char **> *nameArr;
		const UTF8Char **names;
		UOSInt colCnt = this->colNames.GetCount();
		UOSInt i;
		Int32 objId;

		NEW_CLASS(nameArr, Data::Int32FastMap<const UTF8Char **>());
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
					names[i] = Text::StrCopyNew(sbuff).Ptr();
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

void Map::ESRI::ESRIMDBLayer::Init(DB::SharedDBConn *conn, UInt32 srid, Text::CString tableName)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	UInt8 *buff = 0; 
	UOSInt buffSize = 0;
	UOSInt currSize;
	conn->UseObject();
	this->conn = conn;
	this->tableName = Text::String::New(tableName);
	this->currDB = 0;
	this->lastDB = 0;
	this->layerType = Map::DRAW_LAYER_UNKNOWN;
	this->max = Math::Coord2DDbl(0, 0);
	this->min = Math::Coord2DDbl(0, 0);
	this->objIdCol = 0;
	this->shapeCol = 1;
	UOSInt nameCol = 0;

	Sync::MutexUsage mutUsage;
	this->currDB = this->conn->UseConn(mutUsage).Ptr();
	NotNullPtr<DB::DBReader> r;
	if (r.Set(this->currDB->QueryTableData(CSTR_NULL, tableName, 0, 0, 0, CSTR_NULL, 0)))
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
			this->colNames.Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
			i++;
		}
		j = this->colNames.GetCount();
		while (j-- > 0)
		{
			if (this->colNames.GetItem(j)->EndsWithICase(UTF8STRC("NAME")))
			{
				nameCol = j;
			}
		}
		this->SetNameCol(nameCol);
		while (r->ReadNext())
		{
			Int32 objId;
			Math::Geometry::Vector2D *vec;

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
				Math::RectAreaDbl thisBounds;
				this->objects.Put(objId, vec);
				thisBounds = vec->GetBounds();
				if (this->min.x == 0 && this->min.y == 0 && this->max.x == 0 && this->max.y == 0)
				{
					min = thisBounds.tl;
					max = thisBounds.br;
				}
				else
				{
					min = min.Min(thisBounds.tl);
					max = max.Max(thisBounds.br);
				}

				if (this->layerType == Map::DRAW_LAYER_UNKNOWN)
				{
					Math::Geometry::Vector2D::VectorType vt = vec->GetVectorType();
					if (vt == Math::Geometry::Vector2D::VectorType::Point)
					{
						if (vec->HasZ())
						{
							this->layerType = Map::DRAW_LAYER_POINT3D;
						}
						else
						{
							this->layerType = Map::DRAW_LAYER_POINT;
						}
					}
					else if (vt == Math::Geometry::Vector2D::VectorType::Polyline)
					{
						if (vec->HasZ())
						{
							this->layerType = Map::DRAW_LAYER_POLYLINE3D;
						}
						else
						{
							this->layerType = Map::DRAW_LAYER_POLYLINE;
						}
					}
					else if (vt == Math::Geometry::Vector2D::VectorType::Polygon)
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
}

Map::ESRI::ESRIMDBLayer::ESRIMDBLayer(DB::SharedDBConn *conn, UInt32 srid, NotNullPtr<Text::String> sourceName, Text::CString tableName) : Map::MapDrawLayer(sourceName->ToCString(), 0, tableName, Math::CoordinateSystemManager::SRCreateCSysOrDef(srid))
{
	SDEL_STRING(this->layerName);
	this->Init(conn, srid, tableName);
}

Map::ESRI::ESRIMDBLayer::ESRIMDBLayer(DB::SharedDBConn *conn, UInt32 srid, Text::CStringNN sourceName, Text::CString tableName) : Map::MapDrawLayer(sourceName, 0, tableName, Math::CoordinateSystemManager::SRCreateCSysOrDef(srid))
{
	this->Init(conn, srid, tableName);
}

Map::ESRI::ESRIMDBLayer::~ESRIMDBLayer()
{
	UOSInt i;

	this->conn->UnuseObject();
	LIST_FREE_STRING(&this->colNames);
	Math::Geometry::Vector2D *vec;
	i = this->objects.GetCount();
	while (i-- > 0)
	{
		vec = this->objects.GetItem(i);
		DEL_CLASS(vec);
	}
	this->tableName->Release();
}

Map::DrawLayerType Map::ESRI::ESRIMDBLayer::GetLayerType() const
{
	return this->layerType;
}

UOSInt Map::ESRI::ESRIMDBLayer::GetAllObjectIds(NotNullPtr<Data::ArrayListInt64> outArr, NameArray **nameArr)
{
	if (nameArr)
	{
		*nameArr = (NameArray*)ReadNameArr();
	}
	UOSInt i = 0;
	UOSInt j = this->objects.GetCount();
	while (i < j)
	{
		outArr->Add(this->objects.GetKey(i));
		i++;
	}
	return j;
}

UOSInt Map::ESRI::ESRIMDBLayer::GetObjectIds(NotNullPtr<Data::ArrayListInt64> outArr, NameArray **nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty)
{
	return GetObjectIdsMapXY(outArr, nameArr, rect.ToDouble() / mapRate, keepEmpty);
}

UOSInt Map::ESRI::ESRIMDBLayer::GetObjectIdsMapXY(NotNullPtr<Data::ArrayListInt64> outArr, NameArray **nameArr, Math::RectAreaDbl rect, Bool keepEmpty)
{
	if (nameArr)
	{
		*nameArr = (NameArray*)ReadNameArr();
	}
	UOSInt cnt = 0;
	Math::RectAreaDbl minMax;
	Math::Geometry::Vector2D *vec;
	UOSInt i;
	UOSInt j;
	i = 0;
	j = this->objects.GetCount();
	while (i < j)
	{
		vec = this->objects.GetItem(i);
		minMax = vec->GetBounds();
		if (rect.OverlapOrTouch(minMax))
		{
			outArr->Add(this->objects.GetKey(i));
			cnt++;
		}
		i++;
	}
	return cnt;
}

Int64 Map::ESRI::ESRIMDBLayer::GetObjectIdMax() const
{
	return this->objects.GetKey(this->objects.GetCount() - 1);
}

void Map::ESRI::ESRIMDBLayer::ReleaseNameArr(NameArray *nameArr)
{
	Data::FastMap<Int32, const UTF8Char **> *names = (Data::FastMap<Int32, const UTF8Char **> *)nameArr;
	UOSInt i = names->GetCount();
	UOSInt colCnt = this->colNames.GetCount();
	UOSInt j;
	const UTF8Char **nameStrs;
	while (i-- > 0)
	{
		nameStrs = names->GetItem(i);
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

UTF8Char *Map::ESRI::ESRIMDBLayer::GetString(UTF8Char *buff, UOSInt buffSize, NameArray *nameArr, Int64 id, UOSInt strIndex)
{
	Data::FastMap<Int32, const UTF8Char **> *names = (Data::FastMap<Int32, const UTF8Char **> *)nameArr;
	if (names == 0)
		return 0;
	const UTF8Char **nameStrs = names->Get((Int32)id);
	if (nameStrs == 0)
		return 0;
	if (nameStrs[strIndex] == 0)
		return 0;
	return Text::StrConcatS(buff, nameStrs[strIndex], buffSize);
}

UOSInt Map::ESRI::ESRIMDBLayer::GetColumnCnt() const
{
	return this->colNames.GetCount();
}

UTF8Char *Map::ESRI::ESRIMDBLayer::GetColumnName(UTF8Char *buff, UOSInt colIndex)
{
	Text::String *colName = this->colNames.GetItem(colIndex);
	if (colName)
	{
		return colName->ConcatTo(buff);
	}
	return 0;
}

DB::DBUtil::ColType Map::ESRI::ESRIMDBLayer::GetColumnType(UOSInt colIndex, OptOut<UOSInt> colSize)
{
	return DB::DBUtil::CT_Unknown;
}

Bool Map::ESRI::ESRIMDBLayer::GetColumnDef(UOSInt colIndex, NotNullPtr<DB::ColDef> colDef)
{
	return false;
}

UInt32 Map::ESRI::ESRIMDBLayer::GetCodePage() const
{
	return 65001;
}

Bool Map::ESRI::ESRIMDBLayer::GetBounds(OutParam<Math::RectAreaDbl> bounds) const
{
	bounds.Set(Math::RectAreaDbl(this->min, this->max));
	return this->min.x != 0 || this->min.y != 0 || this->max.x != 0 || this->max.y != 0;
}

Map::GetObjectSess *Map::ESRI::ESRIMDBLayer::BeginGetObject()
{
	return (GetObjectSess*)-1;
}

void Map::ESRI::ESRIMDBLayer::EndGetObject(GetObjectSess *session)
{
}

Math::Geometry::Vector2D *Map::ESRI::ESRIMDBLayer::GetNewVectorById(GetObjectSess *session, Int64 id)
{
	Math::Geometry::Vector2D *vec = this->objects.Get((Int32)id);
	if (vec)
		return vec->Clone().Ptr();
	return 0;
}

void Map::ESRI::ESRIMDBLayer::AddUpdatedHandler(UpdatedHandler hdlr, void *obj)
{
}

void Map::ESRI::ESRIMDBLayer::RemoveUpdatedHandler(UpdatedHandler hdlr, void *obj)
{
}

UOSInt Map::ESRI::ESRIMDBLayer::QueryTableNames(Text::CString schemaName, Data::ArrayListNN<Text::String> *names)
{
	if (schemaName.leng != 0)
		return 0;
	names->Add(this->tableName->Clone());
	return 1;
}

DB::DBReader *Map::ESRI::ESRIMDBLayer::QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayListNN<Text::String> *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition)
{
	NotNullPtr<Sync::MutexUsage> mutUsage;
	NEW_CLASSNN(mutUsage, Sync::MutexUsage());
	NotNullPtr<DB::DBConn> currDB = this->conn->UseConn(mutUsage);
	this->currDB = currDB.Ptr();
	this->lastDB = this->currDB;
	NotNullPtr<DB::DBReader> rdr;
	if (rdr.Set(this->currDB->QueryTableData(schemaName, tableName, columnNames, ofst, maxCnt, ordering, condition)))
	{
		Map::ESRI::ESRIMDBReader *r;
		NEW_CLASS(r, Map::ESRI::ESRIMDBReader(currDB, rdr, mutUsage));
		return r;
	}
	mutUsage.Delete();
	this->currDB = 0;
	return 0;
}

DB::TableDef *Map::ESRI::ESRIMDBLayer::GetTableDef(Text::CString schemaName, Text::CString tableName)
{
	Sync::MutexUsage mutUsage;
	this->currDB = this->conn->UseConn(mutUsage).Ptr();
	this->lastDB = this->currDB;
	DB::TableDef *tab = this->currDB->GetTableDef(schemaName, tableName);
	this->currDB = 0;
	return tab;
}

void Map::ESRI::ESRIMDBLayer::CloseReader(DB::DBReader *r)
{
	Map::ESRI::ESRIMDBReader *rdr = (Map::ESRI::ESRIMDBReader*)r;
	DEL_CLASS(rdr);
	this->currDB = 0;
}

void Map::ESRI::ESRIMDBLayer::GetLastErrorMsg(NotNullPtr<Text::StringBuilderUTF8> str)
{
	if (this->lastDB)
	{
		this->lastDB->GetLastErrorMsg(str);
	}
}

void Map::ESRI::ESRIMDBLayer::Reconnect()
{
	this->conn->Reconnect();
}

Map::MapDrawLayer::ObjectClass Map::ESRI::ESRIMDBLayer::GetObjectClass() const
{
	return Map::MapDrawLayer::OC_ESRI_MDB_LAYER;
}

Map::ESRI::ESRIMDBReader::ESRIMDBReader(NotNullPtr<DB::DBConn> conn, NotNullPtr<DB::DBReader> r, NotNullPtr<Sync::MutexUsage> mutUsage)
{
	this->conn = conn;
	this->r = r;
	this->mutUsage = mutUsage;
}

Map::ESRI::ESRIMDBReader::~ESRIMDBReader()
{
	this->conn->CloseReader(this->r);
	this->mutUsage.Delete();
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

Bool Map::ESRI::ESRIMDBReader::GetStr(UOSInt colIndex, NotNullPtr<Text::StringBuilderUTF8> sb)
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

Data::Timestamp Map::ESRI::ESRIMDBReader::GetTimestamp(UOSInt colIndex)
{
	return this->r->GetTimestamp((colIndex > 0)?(colIndex + 1):colIndex);
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

Math::Geometry::Vector2D *Map::ESRI::ESRIMDBReader::GetVector(UOSInt colIndex)
{
	return this->r->GetVector(colIndex);
}

Bool Map::ESRI::ESRIMDBReader::GetUUID(UOSInt colIndex, NotNullPtr<Data::UUID> uuid)
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

DB::DBUtil::ColType Map::ESRI::ESRIMDBReader::GetColType(UOSInt colIndex, OptOut<UOSInt> colSize)
{
	return this->r->GetColType((colIndex > 0)?(colIndex + 1):colIndex, colSize);
}

Bool Map::ESRI::ESRIMDBReader::GetColDef(UOSInt colIndex, NotNullPtr<DB::ColDef> colDef)
{
	return this->r->GetColDef((colIndex > 0)?(colIndex + 1):colIndex, colDef);
}
