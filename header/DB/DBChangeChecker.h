#ifndef _SM_DB_DBCHANGECHECKER
#define _SM_DB_DBCHANGECHECKER
#include "DB/ReadingDB.h"
#include "DB/SQLBuilder.h"

namespace DB
{
	class DBChangeChecker
	{
	public:
		typedef Bool (CALLBACKFUNC SQLHandler)(AnyType userObj, Text::CStringNN sql);
		static const UIntOS TEXT_COL = (UIntOS)-3;
		static const UIntOS UNKNOWN_COL = (UIntOS)-2;
	private:
		Optional<Text::String> lastError;
		UIntOS dataFileRowCnt;
		UIntOS noChgCnt;
		UIntOS newRowCnt;
		UIntOS delRowCnt;
		UIntOS updateCnt;

		NN<DB::ReadingDB> srcConn;
		Text::CString srcSchema;
		Text::CStringNN srcTable;
		Optional<Data::QueryConditions> srcCond;

		Optional<DB::ReadingDB> dataConn;
		Text::CString dataSchema;
		Text::CString dataTable;
		Optional<Data::QueryConditions> dataCond;
		Int8 dataTz;
		Bool dataFileNoHeader;

		Data::ArrayListNative<UIntOS> colInd;
		Data::ArrayListStringNN colStr;
		Text::CString nullStr;
		UIntOS keyCol1;
		UIntOS keyCol2;
		Bool srConv;
		Bool simpleShape;
		Bool fixError;

		Bool IsColIndexValid(NN<Data::ArrayListNative<UIntOS>> colInd, NN<DB::TableDef> destTable);
		void SetLastError(Text::CStringNN msg);
		DB::SQLType GetDBSQLType() const;
		Optional<Text::String> GetNewText(UIntOS colIndex) const;
		NN<Text::String> GetNewTextNN(UIntOS colIndex) const;
		static void __stdcall AppendCol(NN<DB::SQLBuilder> sql, NN<DB::ColDef> col, Optional<Text::String> s, Int8 tzQhr, UInt32 srid);
	public:
		DBChangeChecker(NN<DB::ReadingDB> srcDB, Text::CString srcSchema, Text::CStringNN srcTable);
		~DBChangeChecker();

		void SetDataTable(NN<DB::ReadingDB> dataDB, Text::CString dataSchema, Text::CStringNN dataTable, Int8 dataTz, Bool dataFileNoHeader);
		void SetNullStr(Text::CStringNN nullStr);
		void SetKeyCols(UIntOS keyCol1, UIntOS keyCol2);
		void SetSrcCond(Optional<Data::QueryConditions> srcCond);
		void SetDataCond(Optional<Data::QueryConditions> dataCond);
		void SetColMapping(NN<Data::ArrayListNative<UIntOS>> colInd, NN<Data::ArrayListStringNN> colStr);
		void SetSRConv(Bool srConv);
		void SetSimpleShape(Bool simpleShape);
		void SetFixError(Bool fixError);

		Bool InitColMapping(NN<Data::ArrayListNative<UIntOS>> colInd);
		Bool CheckChange();
		Bool GenerateSQL(DB::SQLType sqlType, Bool axisAware, SQLHandler sqlHdlr, AnyType userObj);

		Optional<Text::String> GetLastError() const;
		UIntOS GetDataFileRowCnt() const;
		UIntOS GetNoChgCnt() const;
		UIntOS GetNewRowCnt() const;
		UIntOS GetDelRowCnt() const;
		UIntOS GetUpdateCnt() const;
	};
}
#endif
