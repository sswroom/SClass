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
		const WChar *ns;
		void *pService;
		static Int32 securityCnt;

		void InitQuery(const WChar *ns);
	public:
		WMIQuery();
		WMIQuery(const WChar *ns);
		virtual ~WMIQuery();

		Bool IsError();

		virtual DB::SQLType GetSQLType() const;
		virtual ConnType GetConnType() const;
		virtual Int8 GetTzQhr() const;
		virtual void ForceTz(Int8 tzQhr);
		virtual void GetConnName(NN<Text::StringBuilderUTF8> sb);
		virtual void Close();
		virtual OSInt ExecuteNonQuery(Text::CStringNN sql);
		OSInt ExecuteNonQueryW(const WChar *sql);
		virtual Optional<DB::DBReader> ExecuteReader(Text::CStringNN sql);
		Optional<DB::DBReader> ExecuteReaderW(const WChar *sql);
		virtual Bool IsLastDataError();

		virtual Optional<DB::DBTransaction> BeginTransaction();
		virtual void Commit(NN<DB::DBTransaction> tran);
		virtual void Rollback(NN<DB::DBTransaction> tran);

		virtual UOSInt QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names);
		virtual Optional<DB::DBReader> QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Data::ArrayListStringNN *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition);
		virtual void CloseReader(NN<DB::DBReader> reader);
		virtual void GetLastErrorMsg(NN<Text::StringBuilderUTF8> str);
		virtual void Reconnect();

		const WChar *GetNS();

		static UOSInt GetNSList(Data::ArrayList<const WChar *> *nsList);
		static void FreeNSList(Data::ArrayList<const WChar *> *nsList);
	};
}
#endif
