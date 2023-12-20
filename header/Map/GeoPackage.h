#ifndef _SM_MAP_GEOPACKAGE
#define _SM_MAP_GEOPACKAGE
#include "Data/ArrayListStringNN.h"
#include "Data/FastStringMap.h"
#include "DB/DBConn.h"
#include "Map/MapLayerCollection.h"
#include "Math/RectAreaDbl.h"

namespace Map
{
	class GeoPackage
	{
	public:
		struct ContentInfo
		{
			NotNullPtr<Text::String> tableName;
			Math::RectAreaDbl bounds;
			Int32 srsId;
			Bool hasZ;
			Bool hasM;
		};
	private:
		DB::DBConn *conn;
		Data::FastStringMap<ContentInfo*> tableList;
		Data::ArrayListStringNN allTables;
		UInt32 useCnt;
	private:
		~GeoPackage();
		static void FreeContent(ContentInfo *cont);
	public:
		GeoPackage(DB::DBConn *conn);
		void Release();

		NotNullPtr<Text::String> GetSourceNameObj();

		UOSInt QueryTableNames(Text::CString schemaName, NotNullPtr<Data::ArrayListNN<Text::String>> names);
		DB::DBReader *QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayListStringNN *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition);
		DB::TableDef *GetTableDef(Text::CString schemaName, Text::CString tableName);
		void CloseReader(NotNullPtr<DB::DBReader> r);
		void GetLastErrorMsg(NotNullPtr<Text::StringBuilderUTF8> str);
		void Reconnect();

		Map::MapLayerCollection *CreateLayerCollection();
	};
}
#endif
