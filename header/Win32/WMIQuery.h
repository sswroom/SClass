#ifndef _SM_WIN32_WMIQUERY
#define _SM_WIN32_WMIQUERY
#include "Data/ArrayListStrUTF8.h"
#include "DB/DBConn.h"
#include "Win32/WMIReader.h"

namespace Win32
{
	class WMIQuery : public DB::DBConn
	{
	private:
		const WChar *ns;
		void *pService;
		Data::ArrayListStrUTF8 *tabNames;
		static Int32 securityCnt;

		void InitQuery(const WChar *ns);
	public:
		WMIQuery();
		WMIQuery(const WChar *ns);
		virtual ~WMIQuery();

		Bool IsError();

		virtual DB::DBUtil::ServerType GetSvrType();
		virtual ConnType GetConnType();
		virtual Int8 GetTzQhr();
		virtual void ForceTz(Int8 tzQhr);
		virtual void GetConnName(Text::StringBuilderUTF *sb);
		virtual void Close();
		virtual OSInt ExecuteNonQuery(const UTF8Char *sql);
		virtual OSInt ExecuteNonQuery(const WChar *sql);
		virtual DB::DBReader *ExecuteReader(const UTF8Char *sql);
		virtual DB::DBReader *ExecuteReader(const WChar *sql);
		virtual Bool IsLastDataError();

		virtual void *BeginTransaction();
		virtual void Commit(void *tran);
		virtual void Rollback(void *tran);

		virtual UOSInt GetTableNames(Data::ArrayList<const UTF8Char*> *names);
		virtual DB::DBReader *GetTableData(const UTF8Char *tableName, Data::ArrayList<const UTF8Char*> *columnNames, UOSInt ofst, UOSInt maxCnt, const UTF8Char *ordering, DB::QueryConditions *condition);
		virtual void CloseReader(DB::DBReader *reader);
		virtual void GetErrorMsg(Text::StringBuilderUTF *str);
		virtual void Reconnect();

		const WChar *GetNS();

		static UOSInt GetNSList(Data::ArrayList<const WChar *> *nsList);
		static void FreeNSList(Data::ArrayList<const WChar *> *nsList);
	};
}
#endif
