#ifndef _SM_DB_MDBFILE
#define _SM_DB_MDBFILE
#include "DB/DBReader.h"
#include "DB/ReadingDB.h"
#include "Data/ArrayListNN.hpp"
#include "IO/StreamData.h"

namespace DB
{
	class MDBFileReader;

	class MDBFile : public DB::ReadingDB
	{
	public:
		struct ColumnInfo;
		struct TableInfo;

	private:
		NN<IO::StreamData> stmData;
		UIntOS pageSize;
		Int8 tzQhr;
		Bool error;
		UTF8Char lastError[128];
		Data::ArrayListNN<TableInfo> tables;

		Bool ReadPage(UInt32 pageNo, UnsafeArray<UInt8> buff) const;
		Bool ReadTableDef(UInt32 pageNo, TableInfo &table);
		Bool ReadCatalog();
		Optional<TableInfo> FindTable(Text::CStringNN tableName);

	public:
		MDBFile(NN<IO::StreamData> stmData);
		virtual ~MDBFile();

		virtual UIntOS QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names);
		virtual Optional<DB::DBReader> QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> columnNames, UIntOS dataOfst, UIntOS maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition);
		virtual Optional<TableDef> GetTableDef(Text::CString schemaName, Text::CStringNN tableName);
		virtual void CloseReader(NN<DB::DBReader> r);
		virtual void GetLastErrorMsg(NN<Text::StringBuilderUTF8> str);
		virtual void Reconnect();
		virtual Int8 GetTzQhr() const;
		virtual void ForceTzQhr(Int8 tzQhr);

		Bool IsError() const;

		friend class MDBFileReader;
	};
}

#endif
