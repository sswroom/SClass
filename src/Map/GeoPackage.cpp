#include "Stdafx.h"
#include "DB/DBReader.h"
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
	Data::ArrayList<Text::String*> colList;
	DB::DBReader *r;
	Text::StringTool::SplitAsNewString(CSTR("table_name,data_type,min_x,min_y,max_x,max_y,srs_id"), ',', &colList);
	r = this->conn->QueryTableData(CSTR_NULL, CSTR("gpkg_contents"), &colList, 0, 0, CSTR_NULL, 0);
	LIST_FREE_STRING(&colList);
	if (r == 0)
	{
		return;
	}
	Text::StringBuilderUTF8 sb;
	ContentInfo *cont;
	while (r->ReadNext())
	{
		sb.ClearStr();
		r->GetStr(1, &sb);
		if (sb.Equals(UTF8STRC("features")))
		{
			cont = MemAlloc(ContentInfo, 1);
			cont->tableName = r->GetNewStr(0);
			cont->bounds.tl.x = r->GetDbl(2);
			cont->bounds.tl.y = r->GetDbl(3);
			cont->bounds.br.x = r->GetDbl(4);
			cont->bounds.br.y = r->GetDbl(5);
			cont->srsId = r->GetInt32(6);
			cont->hasZ = false;
			cont->hasM = false;
			cont = this->tableList.Put(cont->tableName, cont);
			if (cont) FreeContent(cont);
		}
	}
	this->conn->CloseReader(r);

	Text::StringTool::SplitAsNewString(CSTR("table_name,z,m"), ',', &colList);
	r = this->conn->QueryTableData(CSTR_NULL, CSTR("gpkg_geometry_columns"), &colList, 0, 0, CSTR_NULL, 0);
	LIST_FREE_STRING(&colList);
	if (r)
	{
		while (r->ReadNext())
		{
			sb.ClearStr();
			r->GetStr(0, &sb);
			cont = this->tableList.GetC(sb.ToCString());
			if (cont)
			{
				cont->hasZ = r->GetInt32(1) != 0;
				cont->hasM = r->GetInt32(2) != 0;
			}
		}
		this->conn->CloseReader(r);
	}
}

void Map::GeoPackage::Release()
{
	if (Sync::Interlocked::Decrement(&this->useCnt) == 0)
	{
		DEL_CLASS(this);
	}
}

Text::String *Map::GeoPackage::GetSourceNameObj()
{
	return this->conn->GetSourceNameObj();
}

UOSInt Map::GeoPackage::QueryTableNames(Text::CString schemaName, Data::ArrayList<Text::String*> *names)
{
	return this->conn->QueryTableNames(schemaName, names);
}

DB::DBReader *Map::GeoPackage::QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayList<Text::String*> *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition)
{
	return this->conn->QueryTableData(schemaName, tableName, columnNames, ofst, maxCnt, ordering, condition);
}

DB::TableDef *Map::GeoPackage::GetTableDef(Text::CString schemaName, Text::CString tableName)
{
	return this->conn->GetTableDef(schemaName, tableName);
}

void Map::GeoPackage::CloseReader(DB::DBReader *r)
{
	this->conn->CloseReader(r);
}

void Map::GeoPackage::GetLastErrorMsg(Text::StringBuilderUTF8 *str)
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
	Text::String *sourceName = this->conn->GetSourceNameObj();
	UOSInt i = sourceName->LastIndexOf(IO::Path::PATH_SEPERATOR);
	UOSInt j;
	Map::GeoPackageLayer *layer;
	NEW_CLASS(layerColl, Map::MapLayerCollection(sourceName->ToCString(), sourceName->ToCString().Substring(i + 1)));
	i = 0;
	j = this->tableList.GetCount();
	while (i < j)
	{
		NEW_CLASS(layer, Map::GeoPackageLayer(this, this->tableList.GetItem(i)));
		Sync::Interlocked::Increment(&this->useCnt);
		layerColl->Add(layer);
		i++;
	}
	return layerColl;
}