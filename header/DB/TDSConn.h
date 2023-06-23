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
		TDSConn(Text::CString serverHost, UInt16 port, Bool encrypt, Text::CString database, Text::CString userName, Text::CString password, IO::LogTool *log, Text::StringBuilderUTF8 *errMsg);
		virtual ~TDSConn();

		Bool IsConnected() const;
		Text::String *GetConnHost() const;
		Text::String *GetConnDB() const;
		Text::String *GetConnUID() const;
		Text::String *GetConnPWD() const;

		virtual DB::SQLType GetSQLType() const;
		virtual ConnType GetConnType() const;
		virtual Int8 GetTzQhr() const;
		virtual void ForceTz(Int8 tzQhr);
		virtual void GetConnName(Text::StringBuilderUTF8 *sb);
		virtual void Close();
		virtual OSInt ExecuteNonQuery(Text::CString sql);
		virtual DBReader *ExecuteReader(Text::CString sql);
		virtual void CloseReader(DBReader *r);
		virtual void GetLastErrorMsg(Text::StringBuilderUTF8 *str);
		virtual Bool IsLastDataError();
		virtual void Reconnect();

		virtual void *BeginTransaction();
		virtual void Commit(void *tran);
		virtual void Rollback(void *tran);

		virtual UOSInt QueryTableNames(Text::CString schemaName, Data::ArrayList<Text::String*> *names);
		virtual DBReader *QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayList<Text::String*> *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition);
	};
}
#endif
