#ifndef _SM_DB_JAVADBUTIL
#define _SM_DB_JAVADBUTIL
#include "Data/StringMapNative.hpp"
#include "DB/DBTool.h"
#include "Net/SocketFactory.h"

namespace DB
{
	class JavaDBUtil
	{
	private:
		static NN<Text::String> AppendFieldAnno(NN<Text::StringBuilderUTF8> sb, NN<DB::ColDef> col, NN<Data::StringMapNative<Bool>> importMap); //return colName, need release
		static void AppendFieldDef(NN<Text::StringBuilderUTF8> sb, NN<DB::ColDef> col, NN<Text::String> colName, NN<Data::StringMapNative<Bool>> importMap);
		static void AppendConstrHdr(NN<Text::StringBuilderUTF8> sb, NN<DB::ColDef> col, NN<Text::String> colName, Bool isLast);
		static void AppendConstrItem(NN<Text::StringBuilderUTF8> sb, NN<Text::String> colName);
		static void AppendGetterSetter(NN<Text::StringBuilderUTF8> sb, NN<DB::ColDef> col, NN<Text::String> colName);
		static void AppendEqualsItem(NN<Text::StringBuilderUTF8> sb, NN<DB::ColDef> col, NN<Text::String> colName, NN<Text::String> clsName, Bool isLast);
		static void AppendHashCodeItem(NN<Text::StringBuilderUTF8> sb, NN<Text::String> colName, Bool isLast);
		static void AppendFieldOrderItem(NN<Text::StringBuilderUTF8> sb, NN<Text::String> colName, Bool isLast);
		
	public:
		static Optional<DB::DBTool> OpenJDBC(NN<Text::String> url, Optional<Text::String> username, Optional<Text::String> password, NN<IO::LogTool> log, NN<Net::SocketFactory> sockf);
		static Bool ToJavaEntity(NN<Text::StringBuilderUTF8> sb, Optional<Text::String> schemaName, NN<Text::String> tableName, Optional<Text::String> databaseName, NN<DB::ReadingDB> db);
	};
}
#endif
