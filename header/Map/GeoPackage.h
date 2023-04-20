#ifndef _SM_MAP_GEOPACKAGE
#define _SM_MAP_GEOPACKAGE
#include "Data/FastStringMap.h"
#include "DB/DBConn.h"
#include "Math/RectAreaDbl.h"

namespace Map
{
	class GeoPackage
	{
	public:
		struct ContentInfo
		{
			Text::String *tableName;
			Math::RectAreaDbl bounds;
			Int32 srsId;
			Bool hasZ;
			Bool hasM;
		};
	private:
		DB::DBConn *conn;
		Data::FastStringMap<ContentInfo*> tableList;
		UInt32 useCnt;
	private:
		~GeoPackage();
		static void FreeContent(ContentInfo *cont);
	public:
		GeoPackage(DB::DBConn *conn);
		void Release();

		Text::String *GetSourceNameObj();

		UOSInt QueryTableNames(Text::CString schemaName, Data::ArrayList<Text::String*> *names);
		DB::DBReader *QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayList<Text::String*> *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition);
		DB::TableDef *GetTableDef(Text::CString schemaName, Text::CString tableName);
		void CloseReader(DB::DBReader *r);
		void GetLastErrorMsg(Text::StringBuilderUTF8 *str);
		void Reconnect();
	};
}
#endif
