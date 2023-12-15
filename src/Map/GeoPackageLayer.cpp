#include "Stdafx.h"
#include "DB/TableDef.h"
#include "Map/GeoPackageLayer.h"
#include "Math/CoordinateSystemManager.h"

Map::GeoPackageLayer::StringSession *Map::GeoPackageLayer::StringSessCreate()
{
	StringSession *sess = MemAlloc(StringSession, 1);
	sess->r = this->gpkg->QueryTableData(CSTR_NULL, this->layerContent->tableName->ToCString(), 0, 0, 0, CSTR_NULL, 0);
	if (sess->r && sess->r->ReadNext())
	{
		sess->thisId = 0;
	}
	else
	{
		sess->thisId = INVALID_INDEX;
	}
	return sess;
}

Bool Map::GeoPackageLayer::StringSessGoRow(StringSession *sess, UOSInt index)
{
	if (sess->thisId == index)
	{
		return true;
	}
	else if (sess->thisId > index)
	{
		NotNullPtr<DB::DBReader> r;
		if (r.Set(sess->r))
			this->gpkg->CloseReader(r);
		sess->r = this->gpkg->QueryTableData(CSTR_NULL, this->layerContent->tableName->ToCString(), 0, index, 0, CSTR_NULL, 0);
		if (sess->r && sess->r->ReadNext())
		{
			sess->thisId = index;
			return true;
		}
		else
		{
			sess->thisId = INVALID_INDEX;
			return false;
		}
	}
	else
	{
		while (sess->r->ReadNext())
		{
			sess->thisId++;
			if (sess->thisId == index)
				return true;
		}
		return false;
	}
}

Map::GeoPackageLayer::GeoPackageLayer(Map::GeoPackage *gpkg, NotNullPtr<Map::GeoPackage::ContentInfo> layerContent) : Map::MapDrawLayer(gpkg->GetSourceNameObj(), 0, layerContent->tableName.Ptr(), Math::CoordinateSystemManager::SRCreateCSysOrDef((UInt32)layerContent->srsId))
{
	this->gpkg = gpkg;
	this->layerContent = layerContent;
	this->tabDef = this->gpkg->GetTableDef(CSTR_NULL, layerContent->tableName->ToCString());
	this->geomCol = INVALID_INDEX;
	this->mixedData = MixedData::AllData;
	if (this->tabDef)
	{
		NotNullPtr<DB::ColDef> col;
		Data::ArrayIterator<NotNullPtr<DB::ColDef>> it = this->tabDef->ColIterator();
		UOSInt i = 0;
		while (it.HasNext())
		{
			col = it.Next();
			if (col->GetColType() == DB::DBUtil::CT_Vector)
			{
				this->geomCol = i;
				break;
			}
			i++;
		}

		if (this->geomCol != INVALID_INDEX)
		{
			NotNullPtr<DB::DBReader> r;
			if (r.Set(this->gpkg->QueryTableData(CSTR_NULL, this->layerContent->tableName->ToCString(), 0, 0, 0, CSTR_NULL, 0)))
			{
				while (r->ReadNext())
				{
					this->vecList.Add(r->GetVector(this->geomCol));
				}
				this->gpkg->CloseReader(r);
			}
		}
	}
}

Map::GeoPackageLayer::~GeoPackageLayer()
{
	this->gpkg->Release();
	SDEL_CLASS(this->tabDef);
	Math::Geometry::Vector2D *vec;
	UOSInt i = this->vecList.GetCount();
	while (i-- > 0)
	{
		vec = this->vecList.GetItem(i);
		SDEL_CLASS(vec);
	}
}

Map::DrawLayerType Map::GeoPackageLayer::GetLayerType() const
{
	return Map::DRAW_LAYER_MIXED;	
}

void Map::GeoPackageLayer::SetMixedData(MixedData mixedData)
{
	this->mixedData = mixedData;
}

UOSInt Map::GeoPackageLayer::GetAllObjectIds(NotNullPtr<Data::ArrayListInt64> outArr, NameArray **nameArr)
{
	if (nameArr) *nameArr = (NameArray*)this->StringSessCreate();
	UOSInt i;
	UOSInt j;
	UOSInt initCnt;
	Math::Geometry::Vector2D *vec;
	switch (this->mixedData)
	{
	default:
	case MixedData::AllData:
		i = 0;
		j = this->vecList.GetCount();
		while (i < j)
		{
			outArr->Add((Int64)i);
			i++;
		}
		return j;
	case MixedData::PointOnly:
		initCnt = outArr->GetCount();
		i = 0;
		j = this->vecList.GetCount();
		while (i < j)
		{
			vec = this->vecList.GetItem(i);
			if (vec && Math::Geometry::Vector2D::VectorTypeIsPoint(vec->GetVectorType()))
				outArr->Add((Int64)i);
			i++;
		}
		return outArr->GetCount() - initCnt;
	case MixedData::NonPointOnly:
		initCnt = outArr->GetCount();
		i = 0;
		j = this->vecList.GetCount();
		while (i < j)
		{
			vec = this->vecList.GetItem(i);
			if (vec && !Math::Geometry::Vector2D::VectorTypeIsPoint(vec->GetVectorType()))
				outArr->Add((Int64)i);
			i++;
		}
		return outArr->GetCount() - initCnt;
	}
}

UOSInt Map::GeoPackageLayer::GetObjectIds(NotNullPtr<Data::ArrayListInt64> outArr, NameArray **nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty)
{
	return GetObjectIdsMapXY(outArr, nameArr, rect.ToDouble() * mapRate, keepEmpty);
}

UOSInt Map::GeoPackageLayer::GetObjectIdsMapXY(NotNullPtr<Data::ArrayListInt64> outArr, NameArray **nameArr, Math::RectAreaDbl rect, Bool keepEmpty)
{
	if (nameArr) *nameArr = (NameArray*)this->StringSessCreate();
	UOSInt i;
	UOSInt j;
	UOSInt initCnt;
	Math::Geometry::Vector2D *vec;
	switch (this->mixedData)
	{
	default:
	case MixedData::AllData:
		initCnt = outArr->GetCount();
		i = 0;
		j = this->vecList.GetCount();
		while (i < j)
		{
			vec = this->vecList.GetItem(i);
			if (vec && rect.OverlapOrTouch(vec->GetBounds()))
				outArr->Add((Int64)i);
			i++;
		}
		return outArr->GetCount() - initCnt;
	case MixedData::PointOnly:
		initCnt = outArr->GetCount();
		i = 0;
		j = this->vecList.GetCount();
		while (i < j)
		{
			vec = this->vecList.GetItem(i);
			if (vec && Math::Geometry::Vector2D::VectorTypeIsPoint(vec->GetVectorType()) && rect.OverlapOrTouch(vec->GetBounds()))
				outArr->Add((Int64)i);
			i++;
		}
		return outArr->GetCount() - initCnt;
	case MixedData::NonPointOnly:
		initCnt = outArr->GetCount();
		i = 0;
		j = this->vecList.GetCount();
		while (i < j)
		{
			vec = this->vecList.GetItem(i);
			if (vec && !Math::Geometry::Vector2D::VectorTypeIsPoint(vec->GetVectorType()) && rect.OverlapOrTouch(vec->GetBounds()))
				outArr->Add((Int64)i);
			i++;
		}
		return outArr->GetCount() - initCnt;
	}
}

Int64 Map::GeoPackageLayer::GetObjectIdMax() const
{
	return (Int64)(this->vecList.GetCount() - 1);
}

void Map::GeoPackageLayer::ReleaseNameArr(NameArray *nameArr)
{
	StringSession *sess = (StringSession*)nameArr;
	if (sess)
	{
		NotNullPtr<DB::DBReader> r;
		if (r.Set(sess->r)) this->gpkg->CloseReader(r);
		MemFree(sess);
	}
}

Bool Map::GeoPackageLayer::GetString(NotNullPtr<Text::StringBuilderUTF8> sb, NameArray *nameArr, Int64 id, UOSInt strIndex)
{
	StringSession *sess = (StringSession*)nameArr;
	if (sess && sess->r)
	{
		if (!StringSessGoRow(sess, (UOSInt)id))
			return false;
		return sess->r->GetStr(strIndex, sb);
	}
	return false;
}

UOSInt Map::GeoPackageLayer::GetColumnCnt() const
{
	if (this->tabDef == 0)	return 0;
	return this->tabDef->GetColCnt();
}

UTF8Char *Map::GeoPackageLayer::GetColumnName(UTF8Char *buff, UOSInt colIndex)
{
	if (this->tabDef == 0)
		return 0;
	NotNullPtr<DB::ColDef> col;
	if (!this->tabDef->GetCol(colIndex).SetTo(col))
		return 0;
	return col->GetColName()->ConcatTo(buff);
}

DB::DBUtil::ColType Map::GeoPackageLayer::GetColumnType(UOSInt colIndex, OptOut<UOSInt> colSize)
{
	if (this->tabDef == 0)
		return DB::DBUtil::CT_Unknown;
	NotNullPtr<DB::ColDef> col;
	if (!this->tabDef->GetCol(colIndex).SetTo(col))
		return DB::DBUtil::CT_Unknown;
	colSize.Set(col->GetColSize());
	return col->GetColType();
}

Bool Map::GeoPackageLayer::GetColumnDef(UOSInt colIndex, NotNullPtr<DB::ColDef> colDef)
{
	if (this->tabDef == 0)
		return false;
	NotNullPtr<DB::ColDef> col;
	if (!this->tabDef->GetCol(colIndex).SetTo(col))
		return false;
	colDef->Set(col);
	return true;
}

UInt32 Map::GeoPackageLayer::GetCodePage() const
{
	return 65001;
}

Bool Map::GeoPackageLayer::GetBounds(OutParam<Math::RectAreaDbl> rect) const
{
	rect.Set(this->layerContent->bounds);
	return true;
}

Map::GetObjectSess *Map::GeoPackageLayer::BeginGetObject()
{
	return (Map::GetObjectSess*)-1;
}

void Map::GeoPackageLayer::EndGetObject(GetObjectSess *session)
{
}

Math::Geometry::Vector2D *Map::GeoPackageLayer::GetNewVectorById(GetObjectSess *session, Int64 id)
{
	Math::Geometry::Vector2D *vec = this->vecList.GetItem((UOSInt)id);
	if (vec)
		return vec->Clone().Ptr();
	return 0;
}

UOSInt Map::GeoPackageLayer::QueryTableNames(Text::CString schemaName, NotNullPtr<Data::ArrayListNN<Text::String>> names)
{
	return this->gpkg->QueryTableNames(schemaName, names);
}

DB::DBReader *Map::GeoPackageLayer::QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayListNN<Text::String> *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition)
{
	return this->gpkg->QueryTableData(schemaName, tableName, columnNames, ofst, maxCnt, ordering, condition);
}

DB::TableDef *Map::GeoPackageLayer::GetTableDef(Text::CString schemaName, Text::CString tableName)
{
	return this->gpkg->GetTableDef(schemaName, tableName);
}

void Map::GeoPackageLayer::CloseReader(NotNullPtr<DB::DBReader> r)
{
	this->gpkg->CloseReader(r);
}

void Map::GeoPackageLayer::GetLastErrorMsg(NotNullPtr<Text::StringBuilderUTF8> str)
{
	this->gpkg->GetLastErrorMsg(str);
}

void Map::GeoPackageLayer::Reconnect()
{
	this->gpkg->Reconnect();
}

Map::MapDrawLayer::ObjectClass Map::GeoPackageLayer::GetObjectClass() const
{
	return Map::MapDrawLayer::ObjectClass::OC_GEOPACKAGE;
}

void Map::GeoPackageLayer::MultiplyCoordinates(Double v)
{
	UOSInt i = this->vecList.GetCount();
	while (i-- > 0)
	{
		this->vecList.GetItem(i)->MultiplyCoordinatesXY(v);
	}
}
