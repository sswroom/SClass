#include "Stdafx.h"
#include "DB/TableDef.h"
#include "Map/GeoPackageLayer.h"
#include "Math/CoordinateSystemManager.h"

NN<Map::GeoPackageLayer::StringSession> Map::GeoPackageLayer::StringSessCreate()
{
	NN<StringSession> sess = MemAllocNN(StringSession);
	NN<DB::DBReader> r;
	sess->r = this->gpkg->QueryTableData(nullptr, this->layerContent->tableName->ToCString(), nullptr, 0, 0, nullptr, nullptr);
	if (sess->r.SetTo(r) && r->ReadNext())
	{
		sess->thisId = 0;
	}
	else
	{
		sess->thisId = INVALID_INDEX;
	}
	return sess;
}

Bool Map::GeoPackageLayer::StringSessGoRow(NN<StringSession> sess, UOSInt index)
{
	NN<DB::DBReader> r;
	if (sess->thisId == index)
	{
		return true;
	}
	else if (sess->thisId > index)
	{
		if (sess->r.SetTo(r))
			this->gpkg->CloseReader(r);
		sess->r = this->gpkg->QueryTableData(nullptr, this->layerContent->tableName->ToCString(), nullptr, index, 0, nullptr, nullptr);
		if (sess->r.SetTo(r) && r->ReadNext())
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
		if (sess->r.SetTo(r))
		{
			while (r->ReadNext())
			{
				sess->thisId++;
				if (sess->thisId == index)
					return true;
			}
		}
		return false;
	}
}

Map::GeoPackageLayer::GeoPackageLayer(NN<Map::GeoPackage> gpkg, NN<Map::GeoPackage::ContentInfo> layerContent) : Map::MapDrawLayer(gpkg->GetSourceNameObj(), 0, layerContent->tableName.Ptr(), Math::CoordinateSystemManager::SRCreateCSysOrDef((UInt32)layerContent->srsId))
{
	this->gpkg = gpkg;
	this->layerContent = layerContent;
	this->tabDef = this->gpkg->GetTableDef(nullptr, layerContent->tableName->ToCString());
	this->geomCol = INVALID_INDEX;
	this->mixedData = MixedData::AllData;
	NN<DB::TableDef> tabDef;
	if (this->tabDef.SetTo(tabDef))
	{
		NN<DB::ColDef> col;
		Data::ArrayIterator<NN<DB::ColDef>> it = tabDef->ColIterator();
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
			NN<DB::DBReader> r;
			if (this->gpkg->QueryTableData(nullptr, this->layerContent->tableName->ToCString(), nullptr, 0, 0, nullptr, nullptr).SetTo(r))
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
	this->tabDef.Delete();
	Optional<Math::Geometry::Vector2D> vec;
	UOSInt i = this->vecList.GetCount();
	while (i-- > 0)
	{
		vec = this->vecList.GetItem(i);
		vec.Delete();
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

UOSInt Map::GeoPackageLayer::GetAllObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr)
{
	if (nameArr.IsNotNull()) nameArr.SetNoCheck(NN<NameArray>::ConvertFrom(this->StringSessCreate()));
	UOSInt i;
	UOSInt j;
	UOSInt initCnt;
	NN<Math::Geometry::Vector2D> vec;
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
			if (this->vecList.GetItem(i).SetTo(vec) && Math::Geometry::Vector2D::VectorTypeIsPoint(vec->GetVectorType()))
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
			if (this->vecList.GetItem(i).SetTo(vec) && !Math::Geometry::Vector2D::VectorTypeIsPoint(vec->GetVectorType()))
				outArr->Add((Int64)i);
			i++;
		}
		return outArr->GetCount() - initCnt;
	}
}

UOSInt Map::GeoPackageLayer::GetObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty)
{
	return GetObjectIdsMapXY(outArr, nameArr, rect.ToDouble() * mapRate, keepEmpty);
}

UOSInt Map::GeoPackageLayer::GetObjectIdsMapXY(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr, Math::RectAreaDbl rect, Bool keepEmpty)
{
	if (nameArr.IsNotNull()) nameArr.SetNoCheck(NN<NameArray>::ConvertFrom(this->StringSessCreate()));
	UOSInt i;
	UOSInt j;
	UOSInt initCnt;
	NN<Math::Geometry::Vector2D> vec;
	switch (this->mixedData)
	{
	default:
	case MixedData::AllData:
		initCnt = outArr->GetCount();
		i = 0;
		j = this->vecList.GetCount();
		while (i < j)
		{
			if (this->vecList.GetItem(i).SetTo(vec) && rect.OverlapOrTouch(vec->GetBounds()))
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
			if (this->vecList.GetItem(i).SetTo(vec) && Math::Geometry::Vector2D::VectorTypeIsPoint(vec->GetVectorType()) && rect.OverlapOrTouch(vec->GetBounds()))
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
			if (this->vecList.GetItem(i).SetTo(vec) && !Math::Geometry::Vector2D::VectorTypeIsPoint(vec->GetVectorType()) && rect.OverlapOrTouch(vec->GetBounds()))
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

UOSInt Map::GeoPackageLayer::GetRecordCnt() const
{
	return this->vecList.GetCount();
}

void Map::GeoPackageLayer::ReleaseNameArr(Optional<NameArray> nameArr)
{
	NN<StringSession> sess;
	if (Optional<StringSession>::ConvertFrom(nameArr).SetTo(sess))
	{
		NN<DB::DBReader> r;
		if (sess->r.SetTo(r)) this->gpkg->CloseReader(r);
		MemFreeNN(sess);
	}
}

Bool Map::GeoPackageLayer::GetString(NN<Text::StringBuilderUTF8> sb, Optional<NameArray> nameArr, Int64 id, UOSInt strIndex)
{
	NN<StringSession> sess;
	NN<DB::DBReader> r;
	if (Optional<StringSession>::ConvertFrom(nameArr).SetTo(sess) && sess->r.SetTo(r))
	{
		if (!StringSessGoRow(sess, (UOSInt)id))
			return false;
		return r->GetStr(strIndex, sb);
	}
	return false;
}

UOSInt Map::GeoPackageLayer::GetColumnCnt() const
{
	NN<DB::TableDef> tabDef;
	if (!this->tabDef.SetTo(tabDef))	return 0;
	return tabDef->GetColCnt();
}

UnsafeArrayOpt<UTF8Char> Map::GeoPackageLayer::GetColumnName(UnsafeArray<UTF8Char> buff, UOSInt colIndex) const
{
	NN<DB::TableDef> tabDef;
	if (!this->tabDef.SetTo(tabDef))
		return nullptr;
	NN<DB::ColDef> col;
	if (!tabDef->GetCol(colIndex).SetTo(col))
		return nullptr;
	return col->GetColName()->ConcatTo(buff);
}

DB::DBUtil::ColType Map::GeoPackageLayer::GetColumnType(UOSInt colIndex, OptOut<UOSInt> colSize) const
{
	NN<DB::TableDef> tabDef;
	if (!this->tabDef.SetTo(tabDef))
		return DB::DBUtil::CT_Unknown;
	NN<DB::ColDef> col;
	if (!tabDef->GetCol(colIndex).SetTo(col))
		return DB::DBUtil::CT_Unknown;
	colSize.Set(col->GetColSize());
	return col->GetColType();
}

Bool Map::GeoPackageLayer::GetColumnDef(UOSInt colIndex, NN<DB::ColDef> colDef) const
{
	NN<DB::TableDef> tabDef;
	if (!this->tabDef.SetTo(tabDef))
		return false;
	NN<DB::ColDef> col;
	if (!tabDef->GetCol(colIndex).SetTo(col))
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

NN<Map::GetObjectSess> Map::GeoPackageLayer::BeginGetObject()
{
	return NN<GetObjectSess>::ConvertFrom(NN<GeoPackageLayer>(*this));
}

void Map::GeoPackageLayer::EndGetObject(NN<GetObjectSess> session)
{
}

Optional<Math::Geometry::Vector2D> Map::GeoPackageLayer::GetNewVectorById(NN<GetObjectSess> session, Int64 id)
{
	NN<Math::Geometry::Vector2D> vec;
	if (this->vecList.GetItem((UOSInt)id).SetTo(vec))
		return vec->Clone();
	return nullptr;
}

UOSInt Map::GeoPackageLayer::QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names)
{
	return this->gpkg->QueryTableNames(schemaName, names);
}

Optional<DB::DBReader> Map::GeoPackageLayer::QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition)
{
	return this->gpkg->QueryTableData(schemaName, tableName, columnNames, ofst, maxCnt, ordering, condition);
}

Optional<DB::TableDef> Map::GeoPackageLayer::GetTableDef(Text::CString schemaName, Text::CStringNN tableName)
{
	return this->gpkg->GetTableDef(schemaName, tableName);
}

void Map::GeoPackageLayer::CloseReader(NN<DB::DBReader> r)
{
	this->gpkg->CloseReader(r);
}

void Map::GeoPackageLayer::GetLastErrorMsg(NN<Text::StringBuilderUTF8> str)
{
	this->gpkg->GetLastErrorMsg(str);
}

void Map::GeoPackageLayer::Reconnect()
{
	this->gpkg->Reconnect();
}

UOSInt Map::GeoPackageLayer::GetGeomCol() const
{
	return this->geomCol;
}

Map::MapDrawLayer::ObjectClass Map::GeoPackageLayer::GetObjectClass() const
{
	return Map::MapDrawLayer::ObjectClass::OC_GEOPACKAGE;
}

void Map::GeoPackageLayer::MultiplyCoordinates(Double v)
{
	UOSInt i = this->vecList.GetCount();
	NN<Math::Geometry::Vector2D> vec;
	while (i-- > 0)
	{
		if (this->vecList.GetItem(i).SetTo(vec))
			vec->MultiplyCoordinatesXY(v);
	}
}
