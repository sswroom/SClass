#ifndef _SM_DB_JAVADBUTIL
#define _SM_DB_JAVADBUTIL
#include "DB/DBTool.h"
#include "Net/SocketFactory.h"

namespace DB
{
	class JavaDBUtil
	{
	private:
		static Text::String *AppendFieldAnno(Text::StringBuilderUTF *sb, DB::ColDef *col); //return colName, need release
		static void AppendFieldDef(Text::StringBuilderUTF *sb, DB::ColDef *col, Text::String *colName);
		static void AppendConstrHdr(Text::StringBuilderUTF *sb, DB::ColDef *col, Text::String *colName, Bool isLast);
		static void AppendConstrItem(Text::StringBuilderUTF *sb, Text::String *colName);
		static void AppendGetterSetter(Text::StringBuilderUTF *sb, DB::ColDef *col, Text::String *colName);
		static void AppendEqualsItem(Text::StringBuilderUTF *sb, DB::ColDef *col, Text::String *colName, Text::String *clsName, Bool isLast);
		static void AppendHashCodeItem(Text::StringBuilderUTF *sb, Text::String *colName, Bool isLast);
		static void AppendFieldOrderItem(Text::StringBuilderUTF *sb, Text::String *colName, Bool isLast);
		
	public:
		static DB::DBTool *OpenJDBC(Text::String *url, Text::String *username, Text::String *password, IO::LogTool *log, Net::SocketFactory *sockf);
		static Bool ToJavaEntity(Text::StringBuilderUTF *sb, Text::String *tableName, DB::ReadingDBTool *db);
	};
}
#endif
