#ifndef _SM_DB_TDSCONN
#define _SM_DB_TDSCONN
#include "DB/DBConn.h"
#include "IO/LogTool.h"
#include "Sync/Mutex.h"

namespace DB
{
	class TDSConn : public DB::DBConn
	{
	private:
		static Bool inited;
		struct ClassData;
		ClassData *clsData;
		Sync::Mutex cmdMut;
		DB::SQLType sqlType;

	public:
		TDSConn(Text::CStringNN serverHost, UInt16 port, Bool encrypt, Text::CString database, Text::CString userName, Text::CString password, NotNullPtr<IO::LogTool> log, Text::StringBuilderUTF8 *errMsg);
		virtual ~TDSConn();

		Bool IsConnected() const;
		NotNullPtr<Text::String> GetConnHost() const;
		Optional<Text::String> GetConnDB() const;
		NotNullPtr<Text::String> GetConnUID() const;
		NotNullPtr<Text::String> GetConnPWD() const;

		virtual DB::SQLType GetSQLType() const;
		virtual ConnType GetConnType() const;
		virtual Int8 GetTzQhr() const;
		virtual void ForceTz(Int8 tzQhr);
		virtual void GetConnName(NotNullPtr<Text::StringBuilderUTF8> sb);
		virtual void Close();
		virtual OSInt ExecuteNonQuery(Text::CStringNN sql);
		virtual DBReader *ExecuteReader(Text::CStringNN sql);
		virtual void CloseReader(NotNullPtr<DBReader> r);
		virtual void GetLastErrorMsg(NotNullPtr<Text::StringBuilderUTF8> str);
		virtual Bool IsLastDataError();
		virtual void Reconnect();

		virtual void *BeginTransaction();
		virtual void Commit(void *tran);
		virtual void Rollback(void *tran);

		virtual UOSInt QueryTableNames(Text::CString schemaName, NotNullPtr<Data::ArrayListStringNN> names);
		virtual DBReader *QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayListStringNN *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition);
	};
}
#endif
