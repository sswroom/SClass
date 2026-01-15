#include "Stdafx.h"
#include "Map/DBMapLayer.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Geometry/Point.h"
#include "Math/Geometry/PointZ.h"

struct DBMapLayer_NameArr
{
	Int64 currId;
	Text::String **names;
};

void Map::DBMapLayer::ClearDB()
{
	if (this->releaseDB)
	{
		this->db.Delete();
	}
	else
	{
		this->db = nullptr;
	}
	OPTSTR_DEL(this->schema);
	this->table->Release();
	this->idCol = INVALID_INDEX;
	this->vecCol = INVALID_INDEX;
	this->xCol = INVALID_INDEX;
	this->yCol = INVALID_INDEX;
	this->zCol = INVALID_INDEX;
	this->tabDef.Delete();

	this->vecMap.DeleteAll();
	this->objCondition.Delete();
}

Optional<Map::NameArray> Map::DBMapLayer::InitNameArr()
{
	NN<DB::TableDef> tabDef;
	if (this->tabDef.SetTo(tabDef))
	{
		UOSInt i;
		NN<DBMapLayer_NameArr> nameArr = MemAllocNN(DBMapLayer_NameArr);
		nameArr->currId = 0;
		i = tabDef->GetColCnt();
		nameArr->names = MemAlloc(Text::String*, i);
		while (i-- > 0)
		{
			nameArr->names[i] = 0;
		}
		return NN<Map::NameArray>::ConvertFrom(nameArr);
	}
	return nullptr;
}

Map::DBMapLayer::DBMapLayer(NN<Text::String> layerName) : Map::MapDrawLayer(layerName, 0, layerName.Ptr(), Math::CoordinateSystemManager::CreateWGS84Csys())
{
	this->releaseDB = false;
	this->db = nullptr;
	this->schema = nullptr;
	this->table = Text::String::New(CSTR("Default"));
	this->idCol = INVALID_INDEX;
	this->vecCol = INVALID_INDEX;
	this->xCol = INVALID_INDEX;
	this->yCol = INVALID_INDEX;
	this->zCol = INVALID_INDEX;
	this->tabDef = nullptr;
	this->mixedData = MixedData::AllData;
	this->objCondition = nullptr;
}

Map::DBMapLayer::DBMapLayer(Text::CStringNN layerName) : Map::MapDrawLayer(layerName, 0, layerName, Math::CoordinateSystemManager::CreateWGS84Csys())
{
	this->releaseDB = false;
	this->db = nullptr;
	this->schema = nullptr;
	this->table = Text::String::New(CSTR("Default"));
	this->idCol = INVALID_INDEX;
	this->vecCol = INVALID_INDEX;
	this->xCol = INVALID_INDEX;
	this->yCol = INVALID_INDEX;
	this->zCol = INVALID_INDEX;
	this->tabDef = nullptr;
	this->mixedData = MixedData::AllData;
	this->objCondition = nullptr;
}

Map::DBMapLayer::~DBMapLayer()
{
	this->ClearDB();
}

Map::DrawLayerType Map::DBMapLayer::GetLayerType() const
{
	if (this->vecCol == INVALID_INDEX && this->xCol != INVALID_INDEX && this->yCol != INVALID_INDEX)
	{
		if (this->zCol != INVALID_INDEX)
		{
			return Map::DRAW_LAYER_POINT3D;
		}
		else
		{
			return Map::DRAW_LAYER_POINT;
		}
	}
	else
	{
		return Map::DRAW_LAYER_MIXED;
	}
}

void Map::DBMapLayer::SetMixedData(MixedData mixedData)
{
	if (this->vecCol == INVALID_INDEX && this->xCol != INVALID_INDEX && this->yCol != INVALID_INDEX)
	{
		this->mixedData = MixedData::AllData;
	}
	else
	{
		this->mixedData = mixedData;
	}
}

UOSInt Map::DBMapLayer::GetAllObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr)
{
	NN<Data::QueryConditions> cond;
	NN<DB::ReadingDB> db;
	NN<DB::DBReader> r;
	UOSInt initCnt = outArr->GetCount();
	if (this->mixedData != MixedData::AllData)
	{
		NN<Math::Geometry::Vector2D> vec;
		if (this->db.SetTo(db) && this->objCondition.SetTo(cond))
		{
			if (db->QueryTableData(OPTSTR_CSTR(this->schema), this->table->ToCString(), nullptr, 0, 0, nullptr, cond).SetTo(r))
			{
				while (r->ReadNext())
				{
					Int64 id = r->GetInt64(this->idCol);
					if (this->vecMap.Get(id).SetTo(vec) && (Math::Geometry::Vector2D::VectorTypeIsPoint(vec->GetVectorType()) == (this->mixedData == MixedData::PointOnly)))
					{
						outArr->Add(id);
					}
				}
				db->CloseReader(r);
			}
		}
		else
		{
			UOSInt i = 0;
			UOSInt j = this->vecMap.GetCount();
			while (i < j)
			{
				vec = this->vecMap.GetItemNoCheck(i);
				if (Math::Geometry::Vector2D::VectorTypeIsPoint(vec->GetVectorType()) == (this->mixedData == MixedData::PointOnly))
				{
					outArr->Add(this->vecMap.GetKey(i));
				}
				i++;
			}
		}
	}
	else
	{
		if (this->db.SetTo(db) && this->objCondition.SetTo(cond))
		{
			if (db->QueryTableData(OPTSTR_CSTR(this->schema), this->table->ToCString(), nullptr, 0, 0, nullptr, cond).SetTo(r))
			{
				while (r->ReadNext())
				{
					outArr->Add(r->GetInt64(this->idCol));
				}
				db->CloseReader(r);
			}
		}
		else
		{
			this->vecMap.AddKeysTo(outArr);
		}
	}
	if (nameArr.IsNotNull())
		nameArr.SetNoCheck(InitNameArr());
	return outArr->GetCount() - initCnt;
}

UOSInt Map::DBMapLayer::GetObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty)
{
	return GetObjectIdsMapXY(outArr, nameArr, rect.ToDouble() / mapRate, keepEmpty);
}

UOSInt Map::DBMapLayer::GetObjectIdsMapXY(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr, Math::RectAreaDbl rect, Bool keepEmpty)
{
	NN<Data::QueryConditions> cond;
	NN<DB::ReadingDB> db;
	NN<DB::DBReader> r;
	UOSInt initCnt = outArr->GetCount();
	NN<Math::Geometry::Vector2D> vec;
	Math::RectAreaDbl bounds;
	if (this->db.SetTo(db) && this->objCondition.SetTo(cond))
	{
		Int64 id;
		if (this->mixedData != MixedData::AllData)
		{
			if (db->QueryTableData(OPTSTR_CSTR(this->schema), this->table->ToCString(), nullptr, 0, 0, nullptr, cond).SetTo(r))
			{
				while (r->ReadNext())
				{
					id = r->GetInt64(this->idCol);
					if (this->vecMap.Get(id).SetTo(vec) && (Math::Geometry::Vector2D::VectorTypeIsPoint(vec->GetVectorType()) == (this->mixedData == MixedData::PointOnly)))
					{
						bounds = vec->GetBounds();
						if (bounds.OverlapOrTouch(rect))
						{
							outArr->Add(id);
						}
					}
				}
				db->CloseReader(r);
			}
		}
		else
		{
			if (db->QueryTableData(OPTSTR_CSTR(this->schema), this->table->ToCString(), nullptr, 0, 0, nullptr, cond).SetTo(r))
			{
				while (r->ReadNext())
				{
					id = r->GetInt64(this->idCol);
					if (this->vecMap.Get(id).SetTo(vec))
					{
						bounds = vec->GetBounds();
						if (bounds.OverlapOrTouch(rect))
						{
							outArr->Add(id);
						}
					}
				}
				db->CloseReader(r);
			}
		}
	}
	else
	{
		UOSInt i = 0;
		UOSInt j = this->vecMap.GetCount();
		if (this->mixedData != MixedData::AllData)
		{
			while (i < j)
			{
				vec = this->vecMap.GetItemNoCheck(i);
				if (Math::Geometry::Vector2D::VectorTypeIsPoint(vec->GetVectorType()) == (this->mixedData == MixedData::PointOnly))
				{
					bounds = vec->GetBounds();
					if (bounds.OverlapOrTouch(rect))
					{
						outArr->Add(this->vecMap.GetKey(i));
					}
				}
				i++;
			}
		}
		else
		{
			while (i < j)
			{
				vec = this->vecMap.GetItemNoCheck(i);
				bounds = vec->GetBounds();
				if (bounds.OverlapOrTouch(rect))
				{
					outArr->Add(this->vecMap.GetKey(i));
				}
				i++;
			}
		}
	}
	if (nameArr.IsNotNull())
		nameArr.SetNoCheck(InitNameArr());
	return outArr->GetCount() - initCnt;
}

Int64 Map::DBMapLayer::GetObjectIdMax() const
{
	return this->vecMap.GetKey(this->vecMap.GetCount() - 1);
}

UOSInt Map::DBMapLayer::GetRecordCnt() const
{
	return this->vecMap.GetCount();
}

void Map::DBMapLayer::ReleaseNameArr(Optional<NameArray> nameArr)
{
	NN<DB::TableDef> tabDef;
	NN<DBMapLayer_NameArr> narr;
	if (Optional<DBMapLayer_NameArr>::ConvertFrom(nameArr).SetTo(narr) && this->tabDef.SetTo(tabDef))
	{
		UOSInt i = tabDef->GetColCnt();
		while (i-- > 0)
		{
			SDEL_STRING(narr->names[i]);
		}
		MemFree(narr->names);
		MemFreeNN(narr);
	}
}

Bool Map::DBMapLayer::GetString(NN<Text::StringBuilderUTF8> sb, Optional<NameArray> nameArr, Int64 id, UOSInt strIndex)
{
	NN<DB::TableDef> tabDef;
	NN<DB::ReadingDB> db;
	NN<DBMapLayer_NameArr> narr;
	if (Optional<DBMapLayer_NameArr>::ConvertFrom(nameArr).SetTo(narr) && this->tabDef.SetTo(tabDef) && this->db.SetTo(db))
	{
		UOSInt colCnt = tabDef->GetColCnt();
		if (strIndex >= colCnt)
		{
			return false;
		}
		if (narr->currId != id)
		{
			Optional<DB::DBReader> r;
			NN<DB::DBReader> nnr;
			if (this->idCol != INVALID_INDEX)
			{
				Data::QueryConditions cond;
				NN<DB::ColDef> idCol;
				if (tabDef->GetCol(this->idCol).SetTo(idCol))
				{
					cond.Int64Equals(idCol->GetColName()->ToCString(), id);
					r = db->QueryTableData(OPTSTR_CSTR(this->schema), this->table->ToCString(), nullptr, 0, 0, nullptr, &cond);
				}
				else
				{
					r = nullptr;
				}
			}
			else
			{
				r = db->QueryTableData(OPTSTR_CSTR(this->schema), this->table->ToCString(), nullptr, (UOSInt)(id - 1), 1, nullptr, nullptr);
			}
			if (r.SetTo(nnr))
			{
				if (nnr->ReadNext())
				{
					UOSInt i = 0;
					while (i < colCnt)
					{
						SDEL_STRING(narr->names[i]);
						narr->names[i] = nnr->GetNewStr(i).OrNull();
						i++;
					}
					narr->currId = id;
				}
				db->CloseReader(nnr);
			}
		}
		if (narr->currId == id)
		{
			if (narr->names[strIndex])
			{
				sb->Append(narr->names[strIndex]);
				return true;
			}
		}
	}
	return false;
}

UOSInt Map::DBMapLayer::GetColumnCnt() const
{
	NN<DB::TableDef> tabDef;
	if (this->tabDef.SetTo(tabDef))
	{
		return tabDef->GetColCnt();
	}
	return 0;
}

UnsafeArrayOpt<UTF8Char> Map::DBMapLayer::GetColumnName(UnsafeArray<UTF8Char> buff, UOSInt colIndex) const
{
	NN<DB::TableDef> tabDef;
	if (this->tabDef.SetTo(tabDef))
	{
		NN<DB::ColDef> col;
		if (tabDef->GetCol(colIndex).SetTo(col))
		{
			return col->GetColName()->ConcatTo(buff);
		}
	}
	return nullptr;
}

DB::DBUtil::ColType Map::DBMapLayer::GetColumnType(UOSInt colIndex, OptOut<UOSInt> colSize) const
{
	NN<DB::TableDef> tabDef;
	if (this->tabDef.SetTo(tabDef))
	{
		NN<DB::ColDef> col;
		if (tabDef->GetCol(colIndex).SetTo(col))
		{
			colSize.Set(col->GetColSize());
			return col->GetColType();
		}
	}
	return DB::DBUtil::CT_Unknown;
}

Bool Map::DBMapLayer::GetColumnDef(UOSInt colIndex, NN<DB::ColDef> colDef) const
{
	NN<DB::TableDef> tabDef;
	if (this->tabDef.SetTo(tabDef))
	{
		NN<DB::ColDef> col;
		if (tabDef->GetCol(colIndex).SetTo(col))
		{
			colDef->Set(col);
			return true;
		}
	}
	return false;
}

UInt32 Map::DBMapLayer::GetCodePage() const
{
	return 65001;
}

Bool Map::DBMapLayer::GetBounds(OutParam<Math::RectAreaDbl> rect) const
{
	rect.Set(Math::RectAreaDbl(this->min, this->max));
	return true;
}

NN<Map::GetObjectSess> Map::DBMapLayer::BeginGetObject()
{
	return NN<GetObjectSess>::ConvertFrom(NN<DBMapLayer>(*this));
}

void Map::DBMapLayer::EndGetObject(NN<GetObjectSess> session)
{
}

Optional<Math::Geometry::Vector2D> Map::DBMapLayer::GetNewVectorById(NN<GetObjectSess> session, Int64 id)
{
	NN<Math::Geometry::Vector2D> vec;
	if (this->vecMap.Get(id).SetTo(vec))
	{
		return vec->Clone();
	}
	return nullptr;
}

UOSInt Map::DBMapLayer::QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names)
{
	NN<DB::ReadingDB> db;
	if (!this->db.SetTo(db))
	{
		return 0;
	}
	return db->QueryTableNames(schemaName, names);
}

Optional<DB::DBReader> Map::DBMapLayer::QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition)
{
	NN<DB::ReadingDB> db;
	if (this->db.SetTo(db))
	{
		return db->QueryTableData(schemaName, tableName, columnNames, ofst, maxCnt, ordering, condition);
	}
	return nullptr;
}

Optional<DB::TableDef> Map::DBMapLayer::GetTableDef(Text::CString schemaName, Text::CStringNN tableName)
{
	NN<DB::ReadingDB> db;
	if (this->db.SetTo(db))
	{
		return db->GetTableDef(schemaName, tableName);
	}
	return nullptr;
}

void Map::DBMapLayer::CloseReader(NN<DB::DBReader> r)
{
	NN<DB::ReadingDB> db;
	if (this->db.SetTo(db))
	{
		db->CloseReader(r);
	}
}

void Map::DBMapLayer::GetLastErrorMsg(NN<Text::StringBuilderUTF8> str)
{
	NN<DB::ReadingDB> db;
	if (this->db.SetTo(db))
	{
		db->GetLastErrorMsg(str);
	}
}

void Map::DBMapLayer::Reconnect()
{

}

UOSInt Map::DBMapLayer::GetGeomCol() const
{
	NN<DB::TableDef> tabDef;
	NN<DB::ColDef> colDef;
	if (this->tabDef.SetTo(tabDef))
	{
		UOSInt i = tabDef->GetColCnt();
		while (i-- > 0)
		{
			if (tabDef->GetCol(i).SetTo(colDef))
			{
				if (colDef->GetColType() == DB::DBUtil::CT_Vector)
				{
					return i;
				}
			}
		}
	}
	return INVALID_INDEX;
}

Map::MapDrawLayer::ObjectClass Map::DBMapLayer::GetObjectClass() const
{
	return OC_DB_MAP_LAYER;
}

Bool Map::DBMapLayer::SetDatabase(NN<DB::ReadingDB> db, Text::CString schemaName, Text::CStringNN tableName, Bool releaseDB)
{
	this->ClearDB();
	this->releaseDB = false;
	this->db = db;
	this->schema = Text::String::NewOrNull(schemaName);
	this->table = Text::String::New(tableName);

	this->tabDef = db->GetTableDef(schemaName, tableName);
	UOSInt xCol = INVALID_INDEX;
	UOSInt yCol = INVALID_INDEX;
	UOSInt zCol = INVALID_INDEX;
	UInt32 layerSrid = 0;
	NN<DB::ColDef> col;
	NN<DB::TableDef> tabDef;
	if (this->tabDef.SetTo(tabDef))
	{
		UOSInt i = 0;
		UOSInt j = tabDef->GetColCnt();
		while (i < j)
		{
			if (tabDef->GetCol(i).SetTo(col))
			{
				DB::DBUtil::ColType colType = col->GetColType();
				if (col->IsPK())
				{
					if (this->idCol != INVALID_INDEX)
					{
						this->idCol = INVALID_INDEX;
						return false;
					}
					if (colType != DB::DBUtil::CT_Int32 && colType != DB::DBUtil::CT_Int64)
					{
						return false;
					}
					this->idCol = i;
				}
				if (colType == DB::DBUtil::CT_Vector)
				{
					if (this->vecCol != INVALID_INDEX)
					{
						this->vecCol = INVALID_INDEX;
						return false;
					}
					this->vecCol = i;
				}
				else
				{
					NN<Text::String> colName = col->GetColName();
					if (colName->EqualsICase(UTF8STRC("LATITUDE")))
					{
						yCol = i;
						layerSrid = 4326;
					}
					else if (colName->EqualsICase(UTF8STRC("LONGITUDE")))
					{
						xCol = i;
						layerSrid = 4326;
					}
					else if (colName->EqualsICase(UTF8STRC("HEIGHT")))
					{
						zCol = i;
					}
				}
			}
			i++;
		}
	}

	if (this->vecCol != INVALID_INDEX)
	{
		layerSrid = 0;
	}
	else if (xCol != INVALID_INDEX && yCol != INVALID_INDEX)
	{
		this->xCol = xCol;
		this->yCol = yCol;
		this->zCol = zCol;
	}
	else
	{
		return false;
	}

	NN<DB::DBReader> r;
	if (!db->QueryTableData(schemaName, tableName, nullptr, 0, 0, nullptr, nullptr).SetTo(r))
	{
		return false;
	}
	Int64 index = 0;
	Int64 id;
	Math::RectAreaDbl bounds;
	NN<Math::Geometry::Vector2D> vec;
	UInt32 vecSrid;
	while (r->ReadNext())
	{
		index++;
		if (this->idCol == INVALID_INDEX)
		{
			id = index;
		}
		else
		{
			id = r->GetInt64(this->idCol);
		}
		if (this->vecCol != INVALID_INDEX)
		{
			if (r->GetVector(this->vecCol).SetTo(vec))
			{
				bounds = vec->GetBounds();
				if (this->vecMap.GetCount() == 0)
				{
					this->min = bounds.min;
					this->max = bounds.max;
				}
				else
				{
					this->min = this->min.Min(bounds.min);
					this->max = this->max.Max(bounds.max);
				}
				vecSrid = vec->GetSRID();
				if (vecSrid != 0 && layerSrid != vecSrid)
				{
					layerSrid = vecSrid;
				}
				if (this->vecMap.Put(id, vec).SetTo(vec))
				{
					vec.Delete();
				}
			}
		}
		else
		{
			Math::Coord2DDbl pos = Math::Coord2DDbl(r->GetDblOrNAN(xCol), r->GetDblOrNAN(yCol));
			if (this->vecMap.GetCount() == 0)
			{
				this->min = pos;
				this->max = pos;
			}
			else
			{
				this->min = this->min.Min(pos);
				this->max = this->max.Max(pos);
			}
			if (zCol == INVALID_INDEX)
			{
				NEW_CLASSNN(vec, Math::Geometry::Point(layerSrid, pos));
			}
			else
			{
				NEW_CLASSNN(vec, Math::Geometry::PointZ(layerSrid, pos.x, pos.y, r->GetDblOrNAN(zCol)));
			}
			if (this->vecMap.Put(id, vec).SetTo(vec))
			{
				vec.Delete();
			}
		}
	}
	db->CloseReader(r);

	if (layerSrid != 0)
	{
		if (this->csys->GetSRID() != layerSrid)
		{
			NN<Math::CoordinateSystem> csys;
			if (Math::CoordinateSystemManager::SRCreateCSys(layerSrid).SetTo(csys))
			{
				this->SetCoordinateSystem(csys);
			}
		}
	}
	if (this->vecMap.GetCount() > 0)
	{
		this->releaseDB = releaseDB;
		return true;
	}
	else
	{
		this->db = nullptr;
		this->releaseDB = false;
		return false;
	}
}

void Map::DBMapLayer::SetObjCondition(Optional<Data::QueryConditions> condition)
{
	this->objCondition.Delete();
	this->objCondition = condition;
}
