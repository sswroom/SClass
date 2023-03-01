#ifndef _SM_DB_JAVADBUTIL
#define _SM_DB_JAVADBUTIL
#include "Data/StringMap.h"
#include "DB/DBTool.h"
#include "Net/SocketFactory.h"

namespace DB
{
	class JavaDBUtil
	{
	private:
		static Text::String *AppendFieldAnno(Text::StringBuilderUTF8 *sb, DB::ColDef *col, Data::StringMap<Bool> *importMap); //return colName, need release
		static void AppendFieldDef(Text::StringBuilderUTF8 *sb, DB::ColDef *col, Text::String *colName, Data::StringMap<Bool> *importMap);
		static void AppendConstrHdr(Text::StringBuilderUTF8 *sb, DB::ColDef *col, Text::String *colName, Bool isLast);
		static void AppendConstrItem(Text::StringBuilderUTF8 *sb, Text::String *colName);
		static void AppendGetterSetter(Text::StringBuilderUTF8 *sb, DB::ColDef *col, Text::String *colName);
		static void AppendEqualsItem(Text::StringBuilderUTF8 *sb, DB::ColDef *col, Text::String *colName, Text::String *clsName, Bool isLast);
		static void AppendHashCodeItem(Text::StringBuilderUTF8 *sb, Text::String *colName, Bool isLast);
		static void AppendFieldOrderItem(Text::StringBuilderUTF8 *sb, Text::String *colName, Bool isLast);
		
	public:
		static DB::DBTool *OpenJDBC(Text::String *url, Text::String *username, Text::String *password, IO::LogTool *log, Net::SocketFactory *sockf);
		static Bool ToJavaEntity(Text::StringBuilderUTF8 *sb, Text::String *schemaName, Text::String *tableName, Text::String *databaseName, DB::ReadingDB *db);
	};
}
#endif
