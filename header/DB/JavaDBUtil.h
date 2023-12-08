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
		static NotNullPtr<Text::String> AppendFieldAnno(NotNullPtr<Text::StringBuilderUTF8> sb, DB::ColDef *col, Data::StringMap<Bool> *importMap); //return colName, need release
		static void AppendFieldDef(NotNullPtr<Text::StringBuilderUTF8> sb, DB::ColDef *col, NotNullPtr<Text::String> colName, Data::StringMap<Bool> *importMap);
		static void AppendConstrHdr(NotNullPtr<Text::StringBuilderUTF8> sb, DB::ColDef *col, NotNullPtr<Text::String> colName, Bool isLast);
		static void AppendConstrItem(NotNullPtr<Text::StringBuilderUTF8> sb, NotNullPtr<Text::String> colName);
		static void AppendGetterSetter(NotNullPtr<Text::StringBuilderUTF8> sb, DB::ColDef *col, NotNullPtr<Text::String> colName);
		static void AppendEqualsItem(NotNullPtr<Text::StringBuilderUTF8> sb, DB::ColDef *col, NotNullPtr<Text::String> colName, NotNullPtr<Text::String> clsName, Bool isLast);
		static void AppendHashCodeItem(NotNullPtr<Text::StringBuilderUTF8> sb, NotNullPtr<Text::String> colName, Bool isLast);
		static void AppendFieldOrderItem(NotNullPtr<Text::StringBuilderUTF8> sb, NotNullPtr<Text::String> colName, Bool isLast);
		
	public:
		static Optional<DB::DBTool> OpenJDBC(Text::String *url, Text::String *username, Text::String *password, NotNullPtr<IO::LogTool> log, NotNullPtr<Net::SocketFactory> sockf);
		static Bool ToJavaEntity(NotNullPtr<Text::StringBuilderUTF8> sb, Text::String *schemaName, Text::String *tableName, Text::String *databaseName, NotNullPtr<DB::ReadingDB> db);
	};
}
#endif
