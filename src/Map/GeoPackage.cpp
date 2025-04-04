#include "Stdafx.h"
#include "Data/ArrayListStringNN.h"
#include "DB/DBReader.h"
#include "DB/TableDef.h"
#include "IO/Path.h"
#include "Map/GeoPackage.h"
#include "Map/GeoPackageLayer.h"
#include "Sync/Interlocked.h"
#include "Text/StringTool.h"

Map::GeoPackage::~GeoPackage()
{
	DEL_CLASS(this->conn);
	LIST_CALL_FUNC(&this->tableList, FreeContent);
}

void Map::GeoPackage::FreeContent(ContentInfo *cont)
{
	cont->tableName->Release();
	MemFree(cont);
}

Map::GeoPackage::GeoPackage(DB::DBConn *conn)
{
	this->conn = conn;
	this->useCnt = 1;
	NN<Text::String> tableName;
	Data::ArrayListStringNN colList;
	NN<DB::DBReader> r;
	Text::StringTool::SplitAsNewString(CSTR("table_name,data_type,min_x,min_y,max_x,max_y,srs_id"), ',', colList);
	if (!this->conn->QueryTableData(CSTR_NULL, CSTR("gpkg_contents"), &colList, 0, 0, CSTR_NULL, 0).SetTo(r))
	{
		colList.FreeAll();
		return;
	}
	colList.FreeAll();
	Text::StringBuilderUTF8 sb;
	ContentInfo *cont;
	while (r->ReadNext())
	{
		sb.ClearStr();
		r->GetStr(1, sb);
		if (r->GetNewStr(0).SetTo(tableName))
		{
			this->allTables.Add(tableName);
			if (sb.Equals(UTF8STRC("features")))
			{
				cont = MemAlloc(ContentInfo, 1);
				cont->tableName = tableName->Clone();
				cont->bounds.min.x = r->GetDblOrNAN(2);
				cont->bounds.min.y = r->GetDblOrNAN(3);
				cont->bounds.max.x = r->GetDblOrNAN(4);
				cont->bounds.max.y = r->GetDblOrNAN(5);
				cont->srsId = r->GetInt32(6);
				cont->hasZ = false;
				cont->hasM = false;
				cont = this->tableList.PutNN(cont->tableName, cont);
				if (cont) FreeContent(cont);
			}
		}
	}
	this->conn->CloseReader(r);

	Text::StringTool::SplitAsNewString(CSTR("table_name,z,m"), ',', colList);
	if (this->conn->QueryTableData(CSTR_NULL, CSTR("gpkg_geometry_columns"), &colList, 0, 0, CSTR_NULL, 0).SetTo(r))
	{
		colList.FreeAll();
		while (r->ReadNext())
		{
			sb.ClearStr();
			r->GetStr(0, sb);
			cont = this->tableList.GetC(sb.ToCString());
			if (cont)
			{
				cont->hasZ = r->GetInt32(1) != 0;
				cont->hasM = r->GetInt32(2) != 0;
			}
		}
		this->conn->CloseReader(r);
	}
	else
	{
		colList.FreeAll();
	}
}

void Map::GeoPackage::Release()
{
	if (Sync::Interlocked::DecrementU32(this->useCnt) == 0)
	{
		DEL_CLASS(this);
	}
}

NN<Text::String> Map::GeoPackage::GetSourceNameObj()
{
	return this->conn->GetSourceNameObj();
}

UOSInt Map::GeoPackage::QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names)
{
	Data::ArrayIterator<NN<Text::String>> it = this->allTables.Iterator();
	while (it.HasNext())
	{
		names->Add(it.Next()->Clone());
	}
	return this->allTables.GetCount();
}

Optional<DB::DBReader> Map::GeoPackage::QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Data::ArrayListStringNN *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition)
{
	return this->conn->QueryTableData(schemaName, tableName, columnNames, ofst, maxCnt, ordering, condition);
}

Optional<DB::TableDef> Map::GeoPackage::GetTableDef(Text::CString schemaName, Text::CStringNN tableName)
{
	NN<DB::TableDef> tabDef;
	if (this->conn->GetTableDef(schemaName, tableName).SetTo(tabDef))
	{
		ContentInfo *cont = this->tableList.GetC(tableName);
		if (cont)
		{
			Data::ArrayIterator<NN<DB::ColDef>> it = tabDef->ColIterator();
			while (it.HasNext())
			{
				NN<DB::ColDef> col = it.Next();
				if (col->GetColType() == DB::DBUtil::CT_Vector)
				{
					col->SetColSize((UOSInt)DB::ColDef::GeometryTypeAdjust((DB::ColDef::GeometryType)col->GetColSize(), cont->hasZ, cont->hasM));
					col->SetColDP((UInt32)cont->srsId);
				}
			}
		}
		return tabDef;
	}
	return 0;
}

void Map::GeoPackage::CloseReader(NN<DB::DBReader> r)
{
	this->conn->CloseReader(r);
}

void Map::GeoPackage::GetLastErrorMsg(NN<Text::StringBuilderUTF8> str)
{
	this->conn->GetLastErrorMsg(str);
}

void Map::GeoPackage::Reconnect()
{
	this->conn->Reconnect();
}

NN<Map::MapLayerCollection> Map::GeoPackage::CreateLayerCollection()
{
	NN<Map::MapLayerCollection> layerColl;
	NN<Text::String> sourceName = this->conn->GetSourceNameObj();
	UOSInt i = sourceName->LastIndexOf(IO::Path::PATH_SEPERATOR);
	UOSInt j;
	NN<Map::GeoPackageLayer> layer;
	NN<Map::GeoPackage::ContentInfo> contentInfo;
	NEW_CLASSNN(layerColl, Map::MapLayerCollection(sourceName->ToCString(), sourceName->ToCString().Substring(i + 1)));
	i = 0;
	j = this->tableList.GetCount();
	while (i < j)
	{
		if (contentInfo.Set(this->tableList.GetItem(i)))
		{
			NEW_CLASSNN(layer, Map::GeoPackageLayer(this, contentInfo));
			Sync::Interlocked::IncrementU32(this->useCnt);
			layerColl->Add(layer);
		}
		i++;
	}
	return layerColl;
}
