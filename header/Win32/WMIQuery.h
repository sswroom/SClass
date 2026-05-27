#ifndef _SM_WIN32_WMIQUERY
#define _SM_WIN32_WMIQUERY
#include "Data/ArrayListString.h"
#include "DB/DBConn.h"
#include "Win32/WMIReader.h"

namespace Win32
{
	class WMIQuery : public DB::DBConn
	{
	private:
		UnsafeArray<const WChar> ns;
		void *pService;
		static Int32 securityCnt;
		Int8 tzQhr;

		void InitQuery(UnsafeArray<const WChar> ns);
	public:
		WMIQuery();
		WMIQuery(UnsafeArray<const WChar> ns);
		virtual ~WMIQuery();

		Bool IsError();

		virtual DB::SQLType GetSQLType() const;
		virtual ConnType GetConnType() const;
		virtual void GetConnName(NN<Text::StringBuilderUTF8> sb);
		virtual void Close();
		virtual IntOS ExecuteNonQuery(Text::CStringNN sql);
		IntOS ExecuteNonQueryW(UnsafeArray<const WChar> sql);
		virtual Optional<DB::DBReader> ExecuteReader(Text::CStringNN sql);
		Optional<DB::DBReader> ExecuteReaderW(UnsafeArray<const WChar> sql);
		virtual Bool IsLastDataError();

		virtual Optional<DB::DBTransaction> BeginTransaction();
		virtual void Commit(NN<DB::DBTransaction> tran);
		virtual void Rollback(NN<DB::DBTransaction> tran);

		virtual UIntOS QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names);
		virtual Optional<DB::DBReader> QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> columnNames, UIntOS ofst, UIntOS maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition);
		virtual void CloseReader(NN<DB::DBReader> reader);
		virtual void GetLastErrorMsg(NN<Text::StringBuilderUTF8> str);
		virtual void Reconnect();
		virtual Int8 GetTzQhr() const;
		virtual void ForceTzQhr(Int8 tzQhr);

		UnsafeArray<const WChar> GetNS();

		static UIntOS GetNSList(NN<Data::ArrayListArr<const WChar>> nsList);
		static void FreeNSList(NN<Data::ArrayListArr<const WChar>> nsList);
	};
}
#endif
