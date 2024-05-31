#include "Stdafx.h"
#include "DB/ColDef.h"
#include "Map/FileGDBLayer.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Math.h"

Optional<Data::FastMap<Int32, UnsafeArrayOpt<UnsafeArrayOpt<const UTF8Char>>>> Map::FileGDBLayer::ReadNameArr()
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	Sync::MutexUsage mutUsage;
	this->currDB = this->conn->UseDB(mutUsage).Ptr();
	NN<DB::DBReader> r;
	if (this->currDB->QueryTableData(CSTR_NULL, tableName->ToCString(), 0, 0, 0, 0, 0).SetTo(r))
	{
		Data::FastMap<Int32, UnsafeArrayOpt<UnsafeArrayOpt<const UTF8Char>>> *nameArr;
		UnsafeArray<UnsafeArrayOpt<const UTF8Char>> names;
		UOSInt colCnt = this->colNames.GetCount();
		UOSInt i;
		Int32 objId;

		NEW_CLASS(nameArr, Data::Int32FastMap<UnsafeArrayOpt<UnsafeArrayOpt<const UTF8Char>>>());
		while (r->ReadNext())
		{
			objId = r->GetInt32(this->objIdCol);
			names = MemAllocArr(UnsafeArrayOpt<const UTF8Char>, colCnt);
			i = 0;
			while (i < colCnt)
			{
				if (i == this->shapeCol)
				{
					names[i] = 0;
				}
				else if (r->GetStr(i, sbuff, sizeof(sbuff)).SetTo(sptr))
				{
					names[i] = Text::StrCopyNewC(sbuff, (UOSInt)(sptr - sbuff));
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

Map::FileGDBLayer::FileGDBLayer(DB::SharedReadingDB *conn, Text::CStringNN sourceName, Text::CStringNN tableName, NN<Math::ArcGISPRJParser> prjParser) : Map::MapDrawLayer(sourceName, 0, tableName, Math::CoordinateSystemManager::CreateWGS84Csys())
{
	UInt8 *buff = 0; 
	conn->UseObject();
	this->conn = conn;
	this->tableName = Text::String::New(tableName);
	this->currDB = 0;
	this->lastDB = 0;
	this->layerType = Map::DRAW_LAYER_UNKNOWN;
	this->minPos = Math::Coord2DDbl(0, 0);
	this->maxPos = Math::Coord2DDbl(0, 0);
	this->objIdCol = 0;
	this->shapeCol = 1;
	UOSInt nameCol = 0;

	Sync::MutexUsage mutUsage;
	this->currDB = this->conn->UseDB(mutUsage).Ptr();
	NN<DB::DBReader> r;
	if (this->currDB->QueryTableData(CSTR_NULL, tableName, 0, 0, 0, 0, 0).SetTo(r))
	{
		UOSInt i;
		UOSInt j;
		DB::ColDef colDef(CSTR(""));
		i = 0;
		j = r->ColCount();
		while (i < j)
		{
			r->GetColDef(i, colDef);
			if (colDef.GetColType() == DB::DBUtil::CT_Vector)
			{
				this->shapeCol = i;
				NN<Text::String> prj;
				if (colDef.GetAttr().SetTo(prj) && prj->v[0])
				{
					Optional<Math::CoordinateSystem> csys2 = 0;
					NN<Math::CoordinateSystem> nncsys2;
					if (prj->StartsWith(UTF8STRC("EPSG:")))
					{
						csys2 = Math::CoordinateSystemManager::SRCreateCSys(Text::StrToUInt32(&prj->v[5]));
					}
					else
					{
						UOSInt tmp;
						csys2 = prjParser->ParsePRJBuff(tableName, prj->v, prj->leng, tmp);
					}
					if (csys2.SetTo(nncsys2))
					{
						this->csys.Delete();
						this->csys = nncsys2;
					}
				}
			}
			else if (colDef.IsPK() && colDef.GetColType() == DB::DBUtil::CT_Int32)
			{
				this->objIdCol = i;
			}
			this->colNames.Add(colDef.GetColName()->Clone());
			i++;
		}
		j = this->colNames.GetCount();
		while (j-- > 0)
		{
			NN<Text::String> s;
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
			Math::RectAreaDbl bounds;

			objId = r->GetInt32(this->objIdCol);
			if (r->GetVector(this->shapeCol).SetTo(vec))
			{
				this->objects.Put(objId, vec.Ptr());

				bounds = vec->GetBounds();
				if (this->minPos.IsZero() && this->maxPos.IsZero())
				{
					this->minPos = bounds.min;
					this->maxPos = bounds.max;
				}
				else
				{
					this->minPos = this->minPos.Min(bounds.min);
					this->maxPos = this->maxPos.Max(bounds.max);
				}
				if (this->layerType == Map::DRAW_LAYER_UNKNOWN)
				{
					this->layerType = Map::MapDrawLayer::VectorType2LayerType(vec->GetVectorType());
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
	i = this->colNames.GetCount();
	while (i-- > 0)
	{
		OPTSTR_DEL(this->colNames.RemoveAt(i));
	}
	Math::Geometry::Vector2D *vec;
	i = this->objects.GetCount();
	while (i-- > 0)
	{
		vec = this->objects.GetItem(i);
		DEL_CLASS(vec);
	}
	this->tableName->Release();
}

Map::DrawLayerType Map::FileGDBLayer::GetLayerType() const
{
	return this->layerType;
}

UOSInt Map::FileGDBLayer::GetAllObjectIds(NN<Data::ArrayListInt64> outArr, NameArray **nameArr)
{
	if (nameArr)
	{
		*nameArr = (NameArray*)ReadNameArr().OrNull();
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

UOSInt Map::FileGDBLayer::GetObjectIds(NN<Data::ArrayListInt64> outArr, NameArray **nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty)
{
	return GetObjectIdsMapXY(outArr, nameArr, rect.ToDouble() / mapRate, keepEmpty);
}

UOSInt Map::FileGDBLayer::GetObjectIdsMapXY(NN<Data::ArrayListInt64> outArr, NameArray **nameArr, Math::RectAreaDbl rect, Bool keepEmpty)
{
	if (nameArr)
	{
		*nameArr = (NameArray*)ReadNameArr().OrNull();
	}
	UOSInt cnt = 0;
	Math::Geometry::Vector2D *vec;
	UOSInt i;
	UOSInt j;
	i = 0;
	j = this->objects.GetCount();
	while (i < j)
	{
		vec = this->objects.GetItem(i);
		if (rect.OverlapOrTouch(vec->GetBounds()))
		{
			outArr->Add(this->objects.GetKey(i));
			cnt++;
		}
		i++;
	}
	return cnt;
}

Int64 Map::FileGDBLayer::GetObjectIdMax() const
{
	return this->objects.GetKey(this->objects.GetCount() - 1);
}

void Map::FileGDBLayer::ReleaseNameArr(NameArray *nameArr)
{
	Data::FastMap<Int32, UnsafeArrayOpt<UnsafeArrayOpt<const UTF8Char>>> *names = (Data::FastMap<Int32, UnsafeArrayOpt<UnsafeArrayOpt<const UTF8Char>>> *)nameArr;
	UOSInt i = names->GetCount();
	UOSInt colCnt = this->colNames.GetCount();
	UOSInt j;
	UnsafeArray<UnsafeArrayOpt<const UTF8Char>> nameStrs;
	UnsafeArray<const UTF8Char> nameStr;
	while (i-- > 0)
	{
		if (names->GetItem(i).SetTo(nameStrs))
		{
			j = colCnt;
			while (j-- > 0)
			{
				if (nameStrs[j].SetTo(nameStr))
					Text::StrDelNew(nameStr);
			}
			MemFreeArr(nameStrs);
		}
	}
	DEL_CLASS(names);
}

Bool Map::FileGDBLayer::GetString(NN<Text::StringBuilderUTF8> sb, NameArray *nameArr, Int64 id, UOSInt strIndex)
{
	Data::FastMap<Int32, UnsafeArrayOpt<UnsafeArrayOpt<const UTF8Char>>> *names = (Data::FastMap<Int32, UnsafeArrayOpt<UnsafeArrayOpt<const UTF8Char>>> *)nameArr;
	if (names == 0)
		return false;
	UnsafeArray<UnsafeArrayOpt<const UTF8Char>> nameStrs;
	if (!names->Get((Int32)id).SetTo(nameStrs))
		return false;
	if (nameStrs[strIndex] == 0)
		return false;
	sb->AppendSlow(nameStrs[strIndex]);
	return true;
}

UOSInt Map::FileGDBLayer::GetColumnCnt() const
{
	return this->colNames.GetCount();
}

UnsafeArrayOpt<UTF8Char> Map::FileGDBLayer::GetColumnName(UnsafeArray<UTF8Char> buff, UOSInt colIndex)
{
	NN<Text::String> colName;
	if (this->colNames.GetItem(colIndex).SetTo(colName))
	{
		return Text::StrConcatC(buff, colName->v, colName->leng);
	}
	return 0;
}

DB::DBUtil::ColType Map::FileGDBLayer::GetColumnType(UOSInt colIndex, OptOut<UOSInt> colSize)
{
	return DB::DBUtil::CT_Unknown;
}

Bool Map::FileGDBLayer::GetColumnDef(UOSInt colIndex, NN<DB::ColDef> colDef)
{
	return false;
}

UInt32 Map::FileGDBLayer::GetCodePage() const
{
	return 65001;
}

Bool Map::FileGDBLayer::GetBounds(OutParam<Math::RectAreaDbl> rect) const
{
	rect.Set(Math::RectAreaDbl(this->minPos, this->maxPos));
	return !this->minPos.IsZero() || !this->maxPos.IsZero();
}

Map::GetObjectSess *Map::FileGDBLayer::BeginGetObject()
{
	return (GetObjectSess*)-1;
}

void Map::FileGDBLayer::EndGetObject(GetObjectSess *session)
{
}

Math::Geometry::Vector2D *Map::FileGDBLayer::GetNewVectorById(GetObjectSess *session, Int64 id)
{
	Math::Geometry::Vector2D *vec = this->objects.Get((Int32)id);
	if (vec)
	{
		return vec->Clone().Ptr();
	}
	return 0;
}

void Map::FileGDBLayer::AddUpdatedHandler(UpdatedHandler hdlr, AnyType obj)
{
}

void Map::FileGDBLayer::RemoveUpdatedHandler(UpdatedHandler hdlr, AnyType obj)
{
}

UOSInt Map::FileGDBLayer::QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names)
{
	if (schemaName.leng != 0)
		return 0;
	names->Add(this->tableName->Clone());
	return 1;
}

Optional<DB::DBReader> Map::FileGDBLayer::QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Data::ArrayListStringNN *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition)
{
	NN<Sync::MutexUsage> mutUsage;
	NEW_CLASSNN(mutUsage, Sync::MutexUsage());
	NN<DB::ReadingDB> currDB = this->conn->UseDB(mutUsage);
	this->currDB = currDB.Ptr();
	this->lastDB = this->currDB;
	NN<DB::DBReader> rdr;
	if (this->currDB->QueryTableData(schemaName, tableName, columnNames, ofst, maxCnt, ordering, condition).SetTo(rdr))
	{
		Map::FileGDBLReader *r;
		NEW_CLASS(r, Map::FileGDBLReader(currDB, rdr, mutUsage));
		return r;
	}
	mutUsage.Delete();
	this->currDB = 0;
	return 0;
}

Optional<DB::TableDef> Map::FileGDBLayer::GetTableDef(Text::CString schemaName, Text::CStringNN tableName)
{
	Sync::MutexUsage mutUsage;
	this->currDB = this->conn->UseDB(mutUsage).Ptr();
	this->lastDB = this->currDB;
	Optional<DB::TableDef> tab = this->currDB->GetTableDef(schemaName, tableName);
	this->currDB = 0;
	return tab;
}

void Map::FileGDBLayer::CloseReader(NN<DB::DBReader> r)
{
	NN<Map::FileGDBLReader> rdr = NN<Map::FileGDBLReader>::ConvertFrom(r);
	rdr.Delete();
	this->currDB = 0;
}

void Map::FileGDBLayer::GetLastErrorMsg(NN<Text::StringBuilderUTF8> str)
{
	if (this->lastDB)
	{
		this->lastDB->GetLastErrorMsg(str);
	}
}

void Map::FileGDBLayer::Reconnect()
{
	this->conn->Reconnect();
}

Map::MapDrawLayer::ObjectClass Map::FileGDBLayer::GetObjectClass() const
{
	return Map::MapDrawLayer::OC_ESRI_MDB_LAYER;
}

Map::FileGDBLReader::FileGDBLReader(NN<DB::ReadingDB> conn, NN<DB::DBReader> r, NN<Sync::MutexUsage> mutUsage)
{
	this->conn = conn;
	this->r = r;
	this->mutUsage = mutUsage;
}

Map::FileGDBLReader::~FileGDBLReader()
{
	this->conn->CloseReader(r);
	this->mutUsage.Delete();
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

Bool Map::FileGDBLReader::GetStr(UOSInt colIndex, NN<Text::StringBuilderUTF8> sb)
{
	return this->r->GetStr((colIndex > 0)?(colIndex + 1):colIndex, sb);
}

Optional<Text::String> Map::FileGDBLReader::GetNewStr(UOSInt colIndex)
{
	return this->r->GetNewStr((colIndex > 0)?(colIndex + 1):colIndex);
}

UnsafeArrayOpt<UTF8Char> Map::FileGDBLReader::GetStr(UOSInt colIndex, UnsafeArray<UTF8Char> buff, UOSInt buffSize)
{
	return this->r->GetStr((colIndex > 0)?(colIndex + 1):colIndex, buff, buffSize);
}

Data::Timestamp Map::FileGDBLReader::GetTimestamp(UOSInt colIndex)
{
	return this->r->GetTimestamp((colIndex > 0)?(colIndex + 1):colIndex);
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

Optional<Math::Geometry::Vector2D> Map::FileGDBLReader::GetVector(UOSInt colIndex)
{
	return this->r->GetVector(colIndex);
}

Bool Map::FileGDBLReader::GetUUID(UOSInt colIndex, NN<Data::UUID> uuid)
{
	return this->r->GetUUID(colIndex, uuid);
}

Bool Map::FileGDBLReader::IsNull(UOSInt colIndex)
{
	return this->r->IsNull((colIndex > 0)?(colIndex + 1):colIndex);
}

UnsafeArrayOpt<UTF8Char> Map::FileGDBLReader::GetName(UOSInt colIndex, UnsafeArray<UTF8Char> buff)
{
	return this->r->GetName((colIndex > 0)?(colIndex + 1):colIndex, buff);
}

DB::DBUtil::ColType Map::FileGDBLReader::GetColType(UOSInt colIndex, OptOut<UOSInt> colSize)
{
	return this->r->GetColType((colIndex > 0)?(colIndex + 1):colIndex, colSize);
}

Bool Map::FileGDBLReader::GetColDef(UOSInt colIndex, NN<DB::ColDef> colDef)
{
	return this->r->GetColDef((colIndex > 0)?(colIndex + 1):colIndex, colDef);
}
