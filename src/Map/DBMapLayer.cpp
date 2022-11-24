#include "Stdafx.h"
#include "Map/DBMapLayer.h"
#include "Math/CoordinateSystemManager.h"

struct DBMapLayer_NameArr
{
	Int64 currId;
	Text::String **names;
};

void Map::DBMapLayer::ClearDB()
{
	this->db = 0;
	SDEL_STRING(this->schema);
	SDEL_STRING(this->table);
	this->idCol = INVALID_INDEX;
	this->vecCol = INVALID_INDEX;
	SDEL_CLASS(this->tabDef);

	Math::Geometry::Vector2D *vec;
	UOSInt i = this->vecMap.GetCount();
	while (i-- > 0)
	{
		vec = this->vecMap.GetItem(i);
		DEL_CLASS(vec);
	}
	this->vecMap.Clear();
}

void *Map::DBMapLayer::InitNameArr()
{
	if (this->tabDef)
	{
		UOSInt i;
		DBMapLayer_NameArr *nameArr = MemAlloc(DBMapLayer_NameArr, 1);
		nameArr->currId = 0;
		i = this->tabDef->GetColCnt();
		nameArr->names = MemAlloc(Text::String*, i);
		while (i-- > 0)
		{
			nameArr->names[i] = 0;
		}
		return nameArr;
	}
	return 0;
}

Map::DBMapLayer::DBMapLayer(Text::String *layerName) : Map::IMapDrawLayer(layerName, 0, layerName)
{
	this->db = 0;
	this->schema = 0;
	this->table = 0;
	this->idCol = INVALID_INDEX;
	this->vecCol = INVALID_INDEX;
	this->tabDef = 0;
	this->mixedData = MixedData::AllData;
}

Map::DBMapLayer::DBMapLayer(Text::CString layerName) : Map::IMapDrawLayer(layerName, 0, layerName)
{
	this->db = 0;
	this->schema = 0;
	this->table = 0;
	this->idCol = INVALID_INDEX;
	this->vecCol = INVALID_INDEX;
	this->tabDef = 0;
	this->mixedData = MixedData::AllData;
}

Map::DBMapLayer::~DBMapLayer()
{
	this->ClearDB();
}

Map::DrawLayerType Map::DBMapLayer::GetLayerType()
{
	return Map::DRAW_LAYER_MIXED;
}

void Map::DBMapLayer::SetMixedData(MixedData mixedData)
{
	this->mixedData = mixedData;
}

UOSInt Map::DBMapLayer::GetAllObjectIds(Data::ArrayListInt64 *outArr, void **nameArr)
{
	UOSInt initCnt = outArr->GetCount();
	if (this->mixedData != MixedData::AllData)
	{
		Math::Geometry::Vector2D *vec;
		UOSInt i = 0;
		UOSInt j = this->vecMap.GetCount();
		while (i < j)
		{
			vec = this->vecMap.GetItem(i);
			if (Math::Geometry::Vector2D::VectorTypeIsPoint(vec->GetVectorType()) == (this->mixedData == MixedData::PointOnly))
			{
				outArr->Add(this->vecMap.GetKey(i));
			}
			i++;
		}
	}
	else
	{
		this->vecMap.AddKeysTo(outArr);
	}
	if (nameArr)
		*nameArr = InitNameArr();
	return outArr->GetCount() - initCnt;
}

UOSInt Map::DBMapLayer::GetObjectIds(Data::ArrayListInt64 *outArr, void **nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty)
{
	return GetObjectIdsMapXY(outArr, nameArr, rect.ToDouble() / mapRate, keepEmpty);
}

UOSInt Map::DBMapLayer::GetObjectIdsMapXY(Data::ArrayListInt64 *outArr, void **nameArr, Math::RectAreaDbl rect, Bool keepEmpty)
{
	UOSInt initCnt = outArr->GetCount();
	Math::Geometry::Vector2D *vec;
	Math::RectAreaDbl bounds;
	UOSInt i = 0;
	UOSInt j = this->vecMap.GetCount();
	if (this->mixedData != MixedData::AllData)
	{
		while (i < j)
		{
			vec = this->vecMap.GetItem(i);
			if (Math::Geometry::Vector2D::VectorTypeIsPoint(vec->GetVectorType()) == (this->mixedData == MixedData::PointOnly))
			{
				vec->GetBounds(&bounds);
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
			vec = this->vecMap.GetItem(i);
			vec->GetBounds(&bounds);
			if (bounds.OverlapOrTouch(rect))
			{
				outArr->Add(this->vecMap.GetKey(i));
			}
			i++;
		}
	}
	if (nameArr)
		*nameArr = InitNameArr();
	return outArr->GetCount() - initCnt;
}

Int64 Map::DBMapLayer::GetObjectIdMax()
{
	return this->vecMap.GetKey(this->vecMap.GetCount() - 1);
}

void Map::DBMapLayer::ReleaseNameArr(void *nameArr)
{
	if (nameArr)
	{
		DBMapLayer_NameArr *narr = (DBMapLayer_NameArr*)nameArr;
		UOSInt i = this->tabDef->GetColCnt();
		while (i-- > 0)
		{
			SDEL_STRING(narr->names[i]);
		}
		MemFree(narr->names);
		MemFree(nameArr);
	}
}

UTF8Char *Map::DBMapLayer::GetString(UTF8Char *buff, UOSInt buffSize, void *nameArr, Int64 id, UOSInt strIndex)
{
	if (nameArr)
	{
		DBMapLayer_NameArr *narr = (DBMapLayer_NameArr*)nameArr;
		UOSInt colCnt = this->tabDef->GetColCnt();
		if (strIndex >= colCnt)
		{
			return 0;
		}
		if (narr->currId != id)
		{
			DB::DBReader *r;
			if (this->idCol != INVALID_INDEX)
			{
				Data::QueryConditions cond;
				DB::ColDef *idCol = this->tabDef->GetCol(this->idCol);
				cond.Int64Equals(idCol->GetColName()->ToCString(), id);
				r = this->db->QueryTableData(STR_CSTR(this->schema), this->table->ToCString(), 0, 0, 0, 0, &cond);
			}
			else
			{
				r = this->db->QueryTableData(STR_CSTR(this->schema), this->table->ToCString(), 0, (UOSInt)(id - 1), 1, 0, 0);
			}
			if (r)
			{
				if (r->ReadNext())
				{
					UOSInt i = 0;
					while (i < colCnt)
					{
						SDEL_STRING(narr->names[i]);
						narr->names[i] = r->GetNewStr(i);
						i++;
					}
					narr->currId = id;
				}
				db->CloseReader(r);
			}
		}
		if (narr->currId == id)
		{
			if (narr->names[strIndex])
			{
				return narr->names[strIndex]->ConcatToS(buff, buffSize);
			}
		}
	}
	return 0;
}

UOSInt Map::DBMapLayer::GetColumnCnt()
{
	if (this->tabDef)
	{
		return this->tabDef->GetColCnt();
	}
	return 0;
}

UTF8Char *Map::DBMapLayer::GetColumnName(UTF8Char *buff, UOSInt colIndex)
{
	if (this->tabDef)
	{
		DB::ColDef *col = this->tabDef->GetCol(colIndex);
		if (col)
		{
			return col->GetColName()->ConcatTo(buff);
		}
	}
	return 0;
}

DB::DBUtil::ColType Map::DBMapLayer::GetColumnType(UOSInt colIndex, UOSInt *colSize)
{
	if (this->tabDef)
	{
		DB::ColDef *col = this->tabDef->GetCol(colIndex);
		if (col)
		{
			return col->GetColType();
		}
	}
	return DB::DBUtil::CT_Unknown;
}

Bool Map::DBMapLayer::GetColumnDef(UOSInt colIndex, DB::ColDef *colDef)
{
	if (this->tabDef)
	{
		DB::ColDef *col = this->tabDef->GetCol(colIndex);
		if (col)
		{
			colDef->Set(col);
			return true;
		}
	}
	return false;
}

UInt32 Map::DBMapLayer::GetCodePage()
{
	return 65001;
}

Bool Map::DBMapLayer::GetBounds(Math::RectAreaDbl *rect)
{
	rect->tl = this->min;
	rect->br = this->max;
	return true;
}

void *Map::DBMapLayer::BeginGetObject()
{
	return (void*)-1;
}

void Map::DBMapLayer::EndGetObject(void *session)
{
}

Math::Geometry::Vector2D *Map::DBMapLayer::GetNewVectorById(void *session, Int64 id)
{
	Math::Geometry::Vector2D *vec = this->vecMap.Get(id);
	if (vec)
	{
		return vec->Clone();
	}
	return 0;
}

UOSInt Map::DBMapLayer::QueryTableNames(Text::CString schemaName, Data::ArrayList<Text::String*> *names)
{
	if (this->db == 0)
	{
		return 0;
	}
	return this->db->QueryTableNames(schemaName, names);
}

DB::DBReader *Map::DBMapLayer::QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayList<Text::String*> *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition)
{
	if (this->db)
	{
		return this->QueryTableData(schemaName, tableName, columnNames, ofst, maxCnt, ordering, condition);
	}
	return 0;
}

DB::TableDef *Map::DBMapLayer::GetTableDef(Text::CString schemaName, Text::CString tableName)
{
	if (this->db)
	{
		return this->GetTableDef(schemaName, tableName);
	}
	return 0;
}

void Map::DBMapLayer::CloseReader(DB::DBReader *r)
{
	if (this->db)
	{
		this->db->CloseReader(r);
	}
}

void Map::DBMapLayer::GetErrorMsg(Text::StringBuilderUTF8 *str)
{
	if (this->db)
	{
		this->db->GetLastErrorMsg(str);
	}
}

void Map::DBMapLayer::Reconnect()
{

}

Map::IMapDrawLayer::ObjectClass Map::DBMapLayer::GetObjectClass()
{
	return OC_DB_MAP_LAYER;
}

Bool Map::DBMapLayer::SetDatabase(DB::DBTool *db, Text::CString schemaName, Text::CString tableName)
{
	this->ClearDB();
	this->db = db;
	this->schema = Text::String::NewOrNull(schemaName);
	this->table = Text::String::New(tableName);

	this->tabDef = this->db->GetTableDef(schemaName, tableName);
	DB::ColDef *col;
	UOSInt i = 0;
	UOSInt j = this->tabDef->GetColCnt();
	while (i < j)
	{
		col = this->tabDef->GetCol(i);
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
		i++;
	}

	if (this->vecCol == INVALID_INDEX)
	{
		return false;
	}

	DB::DBReader *r = this->db->QueryTableData(schemaName, tableName, 0, 0, 0, CSTR_NULL, 0);
	if (r == 0)
	{
		return false;
	}
	Int64 index = 0;
	Int64 id;
	Math::RectAreaDbl bounds;
	Math::Geometry::Vector2D *vec;
	UInt32 layerSrid = 0;
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
		vec = r->GetVector(this->vecCol);
		if (vec != 0)
		{
			vec->GetBounds(&bounds);
			if (this->vecMap.GetCount() == 0)
			{
				this->min = bounds.tl;
				this->max = bounds.br;
			}
			else
			{
				this->min = this->min.Min(bounds.tl);
				this->max = this->max.Max(bounds.br);
			}
			vecSrid = vec->GetSRID();
			if (vecSrid != 0 && layerSrid != vecSrid)
			{
				layerSrid = vecSrid;
			}
			vec = this->vecMap.Put(id, vec);
			SDEL_CLASS(vec);
		}
	}
	this->db->CloseReader(r);

	if (layerSrid != 0)
	{
		if (this->csys != 0 && this->csys->GetSRID() != layerSrid)
		{
			DEL_CLASS(this->csys);
			this->csys = Math::CoordinateSystemManager::SRCreateCSys(layerSrid);
		}
		else if (this->csys == 0)
		{
			this->csys = Math::CoordinateSystemManager::SRCreateCSys(layerSrid);
		}
	} 
	return this->vecMap.GetCount() > 0;
}
