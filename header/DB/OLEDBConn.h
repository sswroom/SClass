#ifndef _SM_DB_OLEDBCONN
#define _SM_DB_OLEDBCONN
#include "DB/DBConn.h"
#include "DB/DBReader.h"
#include "IO/LogTool.h"

namespace DB
{
	class OLEDBConn : public DB::DBConn
	{
	public:
		typedef enum
		{
			CE_NONE,
			CE_COCREATE,
			CE_GETDATASOURCE,
			CE_DBINITIALIZE,
			CE_GET_CREATE_SESS
		} ConnError;
	private:
		typedef struct
		{
			const Char *providerName;
			const Char *displayName;
			DB::DBUtil::ServerType svrType;
		} ProviderInfo;

		static ProviderInfo providerInfo[];
	private:
		struct ClassData;
		ClassData *clsData;
		ConnError connErr;

	protected:
		OLEDBConn(IO::LogTool *log);
		void Init(const WChar *connStr);
	public:
		OLEDBConn(const WChar *connStr, IO::LogTool *log);
		virtual ~OLEDBConn();

		virtual DB::DBUtil::ServerType GetSvrType();
		virtual ConnType GetConnType();
		virtual Int8 GetTzQhr();
		virtual void ForceTz(Int8 tzQhr);
		virtual void GetConnName(Text::StringBuilderUTF8 *sb);
		virtual void Close();
		virtual OSInt ExecuteNonQuery(const UTF8Char *sql);
//		virtual OSInt ExecuteNonQuery(const WChar *sql);
		virtual void GetErrorMsg(Text::StringBuilderUTF8 *str);
		virtual Bool IsLastDataError();
		virtual void Reconnect();

		virtual UOSInt GetTableNames(Data::ArrayList<const UTF8Char*> *names);
		virtual DBReader *GetTableData(const UTF8Char *tableName, Data::ArrayList<Text::String*> *columnNames, UOSInt ofst, UOSInt maxCnt, const UTF8Char *ordering, Data::QueryConditions *condition);
		virtual DBReader *ExecuteReader(const UTF8Char *sql);
//		virtual DBReader *ExecuteReader(const WChar *sql);
		virtual void CloseReader(DBReader *r);

		virtual void *BeginTransaction();
		virtual void Commit(void *tran);
		virtual void Rollback(void *tran);

		ConnError GetConnError();
		const WChar *GetConnStr();
		static const ProviderInfo *GetProviderInfo(const UTF8Char *providerName);
	};

	class OLEDBReader : public DB::DBReader
	{
	private:
		struct ClassData;

		ClassData *clsData;

	public:
		OLEDBReader(void *pIRowset, OSInt rowChanged);
		virtual ~OLEDBReader();

		virtual Bool ReadNext();
		virtual UOSInt ColCount();
		virtual OSInt GetRowChanged();

		virtual Int32 GetInt32(UOSInt colIndex);
		virtual Int64 GetInt64(UOSInt colIndex);
		virtual WChar *GetStr(UOSInt colIndex, WChar *buff);
		virtual Bool GetStr(UOSInt colIndex, Text::StringBuilderUTF8 *sb);
		virtual Text::String *GetNewStr(UOSInt colIndex);
		virtual UTF8Char *GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize);
		virtual DateErrType GetDate(UOSInt colIndex, Data::DateTime *outVal);
		virtual Double GetDbl(UOSInt colIndex);
		virtual Bool GetBool(UOSInt colIndex);
		virtual UOSInt GetBinarySize(UOSInt colIndex);
		virtual UOSInt GetBinary(UOSInt colIndex, UInt8 *buff);
		virtual Math::Vector2D *GetVector(UOSInt colIndex);
		virtual Bool GetUUID(UOSInt colIndex, Data::UUID *uuid);

		//virtual WChar *GetName(UOSInt colIndex);
		virtual UTF8Char *GetName(UOSInt colIndex, UTF8Char *buff);
		virtual Bool IsNull(UOSInt colIndex);
		virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, UOSInt *colSize);
		virtual Bool GetColDef(UOSInt colIndex, DB::ColDef *colDef);

		DB::DBUtil::ColType DBType2ColType(UInt16 dbType);
	};
}
#endif
