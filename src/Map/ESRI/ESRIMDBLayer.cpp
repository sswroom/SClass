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
	NN<DB::DBReader> r;
	if (this->currDB->QueryTableData(nullptr, this->tableName->ToCString(), 0, 0, 0, nullptr, 0).SetTo(r))
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
				else if (r->GetStr(i, sbuff, sizeof(sbuff)).NotNull())
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

void Map::ESRI::ESRIMDBLayer::Init(DB::SharedDBConn *conn, UInt32 srid, Text::CStringNN tableName)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
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
	NN<DB::DBReader> r;
	if (this->currDB->QueryTableData(nullptr, tableName, 0, 0, 0, nullptr, 0).SetTo(r))
	{
		UOSInt i;
		UOSInt j;
		NN<Text::String> s;
		i = 0;
		j = r->ColCount();
		while (i < j)
		{
			sbuff[0] = 0;
			sptr = r->GetName(i, sbuff).Or(sbuff);
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
			if (this->colNames.GetItem(j).SetTo(s) && s->EndsWithICase(UTF8STRC("NAME")))
			{
				nameCol = j;
			}
		}
		this->SetNameCol(nameCol);
		while (r->ReadNext())
		{
			Int32 objId;
			NN<Math::Geometry::Vector2D> vec;

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
			if (Map::SHPUtil::ParseShpRecord(srid, buff, buffSize).SetTo(vec))
			{
				Math::RectAreaDbl thisBounds;
				this->objects.Put(objId, vec);
				thisBounds = vec->GetBounds();
				if (this->min.x == 0 && this->min.y == 0 && this->max.x == 0 && this->max.y == 0)
				{
					min = thisBounds.min;
					max = thisBounds.max;
				}
				else
				{
					min = min.Min(thisBounds.min);
					max = max.Max(thisBounds.max);
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

Map::ESRI::ESRIMDBLayer::ESRIMDBLayer(DB::SharedDBConn *conn, UInt32 srid, NN<Text::String> sourceName, Text::CStringNN tableName) : Map::MapDrawLayer(sourceName->ToCString(), 0, tableName, Math::CoordinateSystemManager::SRCreateCSysOrDef(srid))
{
	OPTSTR_DEL(this->layerName);
	this->Init(conn, srid, tableName);
}

Map::ESRI::ESRIMDBLayer::ESRIMDBLayer(DB::SharedDBConn *conn, UInt32 srid, Text::CStringNN sourceName, Text::CStringNN tableName) : Map::MapDrawLayer(sourceName, 0, tableName, Math::CoordinateSystemManager::SRCreateCSysOrDef(srid))
{
	this->Init(conn, srid, tableName);
}

Map::ESRI::ESRIMDBLayer::~ESRIMDBLayer()
{
	this->conn->UnuseObject();
	this->colNames.FreeAll();
	this->objects.DeleteAll();
	this->tableName->Release();
}

Map::DrawLayerType Map::ESRI::ESRIMDBLayer::GetLayerType() const
{
	return this->layerType;
}

UOSInt Map::ESRI::ESRIMDBLayer::GetAllObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr)
{
	if (nameArr.IsNotNull())
	{
		nameArr.SetNoCheck((NameArray*)ReadNameArr());
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

UOSInt Map::ESRI::ESRIMDBLayer::GetObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty)
{
	return GetObjectIdsMapXY(outArr, nameArr, rect.ToDouble() / mapRate, keepEmpty);
}

UOSInt Map::ESRI::ESRIMDBLayer::GetObjectIdsMapXY(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr, Math::RectAreaDbl rect, Bool keepEmpty)
{
	if (nameArr.IsNotNull())
	{
		nameArr.SetNoCheck((NameArray*)ReadNameArr());
	}
	UOSInt cnt = 0;
	Math::RectAreaDbl minMax;
	NN<Math::Geometry::Vector2D> vec;
	UOSInt i;
	UOSInt j;
	i = 0;
	j = this->objects.GetCount();
	while (i < j)
	{
		vec = this->objects.GetItemNoCheck(i);
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

void Map::ESRI::ESRIMDBLayer::ReleaseNameArr(Optional<NameArray> nameArr)
{
	NN<NameArray> nnnameArr;
	if (nameArr.SetTo(nnnameArr))
	{
		NN<Data::FastMap<Int32, const UTF8Char **>> names = NN<Data::FastMap<Int32, const UTF8Char **>>::ConvertFrom(nnnameArr);
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
		names.Delete();
	}
}

Bool Map::ESRI::ESRIMDBLayer::GetString(NN<Text::StringBuilderUTF8> sb, Optional<NameArray> nameArr, Int64 id, UOSInt strIndex)
{
	NN<Data::FastMap<Int32, const UTF8Char **>> names;
	if (!Optional<Data::FastMap<Int32, const UTF8Char **>>::ConvertFrom(nameArr).SetTo(names))
		return false;
	const UTF8Char **nameStrs = names->Get((Int32)id);
	if (nameStrs == 0)
		return false;
	if (nameStrs[strIndex] == 0)
		return false;
	sb->AppendSlow(nameStrs[strIndex]);
	return true;
}

UOSInt Map::ESRI::ESRIMDBLayer::GetColumnCnt() const
{
	return this->colNames.GetCount();
}

UnsafeArrayOpt<UTF8Char> Map::ESRI::ESRIMDBLayer::GetColumnName(UnsafeArray<UTF8Char> buff, UOSInt colIndex)
{
	NN<Text::String> colName;
	if (this->colNames.GetItem(colIndex).SetTo(colName))
	{
		return colName->ConcatTo(buff);
	}
	return 0;
}

DB::DBUtil::ColType Map::ESRI::ESRIMDBLayer::GetColumnType(UOSInt colIndex, OptOut<UOSInt> colSize)
{
	return DB::DBUtil::CT_Unknown;
}

Bool Map::ESRI::ESRIMDBLayer::GetColumnDef(UOSInt colIndex, NN<DB::ColDef> colDef)
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

NN<Map::GetObjectSess> Map::ESRI::ESRIMDBLayer::BeginGetObject()
{
	return NN<GetObjectSess>::ConvertFrom(NN<ESRIMDBLayer>(*this));
}

void Map::ESRI::ESRIMDBLayer::EndGetObject(NN<GetObjectSess> session)
{
}

Optional<Math::Geometry::Vector2D> Map::ESRI::ESRIMDBLayer::GetNewVectorById(NN<GetObjectSess> session, Int64 id)
{
	NN<Math::Geometry::Vector2D> vec;
	if (this->objects.Get((Int32)id).SetTo(vec))
		return vec->Clone();
	return 0;
}

void Map::ESRI::ESRIMDBLayer::AddUpdatedHandler(UpdatedHandler hdlr, AnyType obj)
{
}

void Map::ESRI::ESRIMDBLayer::RemoveUpdatedHandler(UpdatedHandler hdlr, AnyType obj)
{
}

UOSInt Map::ESRI::ESRIMDBLayer::QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names)
{
	if (schemaName.leng != 0)
		return 0;
	names->Add(this->tableName->Clone());
	return 1;
}

Optional<DB::DBReader> Map::ESRI::ESRIMDBLayer::QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Data::ArrayListStringNN *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition)
{
	NN<Sync::MutexUsage> mutUsage;
	NEW_CLASSNN(mutUsage, Sync::MutexUsage());
	NN<DB::DBConn> currDB = this->conn->UseConn(mutUsage);
	this->currDB = currDB.Ptr();
	this->lastDB = this->currDB;
	NN<DB::DBReader> rdr;
	if (this->currDB->QueryTableData(schemaName, tableName, columnNames, ofst, maxCnt, ordering, condition).SetTo(rdr))
	{
		NN<Map::ESRI::ESRIMDBReader> r;
		NEW_CLASSNN(r, Map::ESRI::ESRIMDBReader(currDB, rdr, mutUsage));
		return r;
	}
	mutUsage.Delete();
	this->currDB = 0;
	return 0;
}

Optional<DB::TableDef> Map::ESRI::ESRIMDBLayer::GetTableDef(Text::CString schemaName, Text::CStringNN tableName)
{
	Sync::MutexUsage mutUsage;
	this->currDB = this->conn->UseConn(mutUsage).Ptr();
	this->lastDB = this->currDB;
	Optional<DB::TableDef> tab = this->currDB->GetTableDef(schemaName, tableName);
	this->currDB = 0;
	return tab;
}

void Map::ESRI::ESRIMDBLayer::CloseReader(DB::DBReader *r)
{
	Map::ESRI::ESRIMDBReader *rdr = (Map::ESRI::ESRIMDBReader*)r;
	DEL_CLASS(rdr);
	this->currDB = 0;
}

void Map::ESRI::ESRIMDBLayer::GetLastErrorMsg(NN<Text::StringBuilderUTF8> str)
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

UOSInt Map::ESRI::ESRIMDBLayer::GetGeomCol() const
{
	return INVALID_INDEX;
}

Map::MapDrawLayer::ObjectClass Map::ESRI::ESRIMDBLayer::GetObjectClass() const
{
	return Map::MapDrawLayer::OC_ESRI_MDB_LAYER;
}

Map::ESRI::ESRIMDBReader::ESRIMDBReader(NN<DB::DBConn> conn, NN<DB::DBReader> r, NN<Sync::MutexUsage> mutUsage)
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

UnsafeArrayOpt<WChar> Map::ESRI::ESRIMDBReader::GetStr(UOSInt colIndex, UnsafeArray<WChar> buff)
{
	return this->r->GetStr((colIndex > 0)?(colIndex + 1):colIndex, buff);
}

Bool Map::ESRI::ESRIMDBReader::GetStr(UOSInt colIndex, NN<Text::StringBuilderUTF8> sb)
{
	return this->r->GetStr((colIndex > 0)?(colIndex + 1):colIndex, sb);
}

Optional<Text::String> Map::ESRI::ESRIMDBReader::GetNewStr(UOSInt colIndex)
{
	return this->r->GetNewStr((colIndex > 0)?(colIndex + 1):colIndex);
}

UnsafeArrayOpt<UTF8Char> Map::ESRI::ESRIMDBReader::GetStr(UOSInt colIndex, UnsafeArray<UTF8Char> buff, UOSInt buffSize)
{
	return this->r->GetStr((colIndex > 0)?(colIndex + 1):colIndex, buff, buffSize);
}

Data::Timestamp Map::ESRI::ESRIMDBReader::GetTimestamp(UOSInt colIndex)
{
	return this->r->GetTimestamp((colIndex > 0)?(colIndex + 1):colIndex);
}

Double Map::ESRI::ESRIMDBReader::GetDblOrNAN(UOSInt colIndex)
{
	return this->r->GetDblOrNAN((colIndex > 0)?(colIndex + 1):colIndex);
}

Bool Map::ESRI::ESRIMDBReader::GetBool(UOSInt colIndex)
{
	return this->r->GetBool((colIndex > 0)?(colIndex + 1):colIndex);
}

UOSInt Map::ESRI::ESRIMDBReader::GetBinarySize(UOSInt colIndex)
{
	return this->r->GetBinarySize((colIndex > 0)?(colIndex + 1):colIndex);
}

UOSInt Map::ESRI::ESRIMDBReader::GetBinary(UOSInt colIndex, UnsafeArray<UInt8> buff)
{
	return this->r->GetBinary((colIndex > 0)?(colIndex + 1):colIndex, buff);
}

Optional<Math::Geometry::Vector2D> Map::ESRI::ESRIMDBReader::GetVector(UOSInt colIndex)
{
	return this->r->GetVector(colIndex);
}

Bool Map::ESRI::ESRIMDBReader::GetUUID(UOSInt colIndex, NN<Data::UUID> uuid)
{
	return this->r->GetUUID(colIndex, uuid);
}

Bool Map::ESRI::ESRIMDBReader::IsNull(UOSInt colIndex)
{
	return this->r->IsNull((colIndex > 0)?(colIndex + 1):colIndex);
}

UnsafeArrayOpt<UTF8Char> Map::ESRI::ESRIMDBReader::GetName(UOSInt colIndex, UnsafeArray<UTF8Char> buff)
{
	return this->r->GetName((colIndex > 0)?(colIndex + 1):colIndex, buff);
}

DB::DBUtil::ColType Map::ESRI::ESRIMDBReader::GetColType(UOSInt colIndex, OptOut<UOSInt> colSize)
{
	return this->r->GetColType((colIndex > 0)?(colIndex + 1):colIndex, colSize);
}

Bool Map::ESRI::ESRIMDBReader::GetColDef(UOSInt colIndex, NN<DB::ColDef> colDef)
{
	return this->r->GetColDef((colIndex > 0)?(colIndex + 1):colIndex, colDef);
}
