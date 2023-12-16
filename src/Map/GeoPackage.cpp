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
	Text::String *tableName;
	Data::ArrayListStringNN colList;
	NotNullPtr<DB::DBReader> r;
	Text::StringTool::SplitAsNewString(CSTR("table_name,data_type,min_x,min_y,max_x,max_y,srs_id"), ',', colList);
	if (!r.Set(this->conn->QueryTableData(CSTR_NULL, CSTR("gpkg_contents"), &colList, 0, 0, CSTR_NULL, 0)))
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
		tableName = r->GetNewStr(0);
		this->allTables.Add(tableName);
		if (sb.Equals(UTF8STRC("features")))
		{
			cont = MemAlloc(ContentInfo, 1);
			cont->tableName = tableName->Clone();
			cont->bounds.tl.x = r->GetDbl(2);
			cont->bounds.tl.y = r->GetDbl(3);
			cont->bounds.br.x = r->GetDbl(4);
			cont->bounds.br.y = r->GetDbl(5);
			cont->srsId = r->GetInt32(6);
			cont->hasZ = false;
			cont->hasM = false;
			cont = this->tableList.PutNN(cont->tableName, cont);
			if (cont) FreeContent(cont);
		}
	}
	this->conn->CloseReader(r);

	Text::StringTool::SplitAsNewString(CSTR("table_name,z,m"), ',', colList);
	if (r.Set(this->conn->QueryTableData(CSTR_NULL, CSTR("gpkg_geometry_columns"), &colList, 0, 0, CSTR_NULL, 0)))
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

NotNullPtr<Text::String> Map::GeoPackage::GetSourceNameObj()
{
	return this->conn->GetSourceNameObj();
}

UOSInt Map::GeoPackage::QueryTableNames(Text::CString schemaName, NotNullPtr<Data::ArrayListNN<Text::String>> names)
{
	UOSInt i = 0;
	UOSInt j = this->allTables.GetCount();
	while (i < j)
	{
		names->Add(this->allTables.GetItem(i)->Clone());
		i++;
	}
	return j;
}

DB::DBReader *Map::GeoPackage::QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayListStringNN *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition)
{
	return this->conn->QueryTableData(schemaName, tableName, columnNames, ofst, maxCnt, ordering, condition);
}

DB::TableDef *Map::GeoPackage::GetTableDef(Text::CString schemaName, Text::CString tableName)
{
	DB::TableDef *tabDef = this->conn->GetTableDef(schemaName, tableName);
	if (tabDef)
	{
		ContentInfo *cont = this->tableList.GetC(tableName.OrEmpty());
		if (cont)
		{
			Data::ArrayIterator<NotNullPtr<DB::ColDef>> it = tabDef->ColIterator();
			while (it.HasNext())
			{
				NotNullPtr<DB::ColDef> col = it.Next();
				if (col->GetColType() == DB::DBUtil::CT_Vector)
				{
					col->SetColSize((UOSInt)DB::ColDef::GeometryTypeAdjust((DB::ColDef::GeometryType)col->GetColSize(), cont->hasZ, cont->hasM));
					col->SetColDP((UInt32)cont->srsId);
				}
			}
		}
	}
	return tabDef;
}

void Map::GeoPackage::CloseReader(NotNullPtr<DB::DBReader> r)
{
	this->conn->CloseReader(r);
}

void Map::GeoPackage::GetLastErrorMsg(NotNullPtr<Text::StringBuilderUTF8> str)
{
	this->conn->GetLastErrorMsg(str);
}

void Map::GeoPackage::Reconnect()
{
	this->conn->Reconnect();
}

Map::MapLayerCollection *Map::GeoPackage::CreateLayerCollection()
{
	Map::MapLayerCollection *layerColl;
	NotNullPtr<Text::String> sourceName = this->conn->GetSourceNameObj();
	UOSInt i = sourceName->LastIndexOf(IO::Path::PATH_SEPERATOR);
	UOSInt j;
	NotNullPtr<Map::GeoPackageLayer> layer;
	NotNullPtr<Map::GeoPackage::ContentInfo> contentInfo;
	NEW_CLASS(layerColl, Map::MapLayerCollection(sourceName->ToCString(), sourceName->ToCString().Substring(i + 1)));
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
