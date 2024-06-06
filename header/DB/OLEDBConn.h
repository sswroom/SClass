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
			DB::SQLType sqlType;
		} ProviderInfo;

		static ProviderInfo providerInfo[];
	private:
		struct ClassData;
		ClassData *clsData;
		ConnError connErr;

	protected:
		OLEDBConn(NN<IO::LogTool> log);
		void Init(const WChar *connStr);
	public:
		OLEDBConn(const WChar *connStr, NN<IO::LogTool> log);
		virtual ~OLEDBConn();

		virtual DB::SQLType GetSQLType() const;
		virtual ConnType GetConnType() const;
		virtual Int8 GetTzQhr() const;
		virtual void ForceTz(Int8 tzQhr);
		virtual void GetConnName(NN<Text::StringBuilderUTF8> sb);
		virtual void Close();
		virtual OSInt ExecuteNonQuery(Text::CStringNN sql);
//		virtual OSInt ExecuteNonQuery(const WChar *sql);
		virtual void GetLastErrorMsg(NN<Text::StringBuilderUTF8> str);
		virtual Bool IsLastDataError();
		virtual void Reconnect();

		virtual UOSInt QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names);
		virtual Optional<DBReader> QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition);
		virtual Optional<DBReader> ExecuteReader(Text::CStringNN sql);
//		virtual DBReader *ExecuteReader(const WChar *sql);
		virtual void CloseReader(NN<DBReader> r);

		virtual Optional<DB::DBTransaction> BeginTransaction();
		virtual void Commit(NN<DB::DBTransaction> tran);
		virtual void Rollback(NN<DB::DBTransaction> tran);

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
		virtual Bool GetStr(UOSInt colIndex, NN<Text::StringBuilderUTF8> sb);
		virtual Optional<Text::String> GetNewStr(UOSInt colIndex);
		virtual UnsafeArrayOpt<UTF8Char> GetStr(UOSInt colIndex, UnsafeArray<UTF8Char> buff, UOSInt buffSize);
		virtual Data::Timestamp GetTimestamp(UOSInt colIndex);
		virtual Double GetDbl(UOSInt colIndex);
		virtual Bool GetBool(UOSInt colIndex);
		virtual UOSInt GetBinarySize(UOSInt colIndex);
		virtual UOSInt GetBinary(UOSInt colIndex, UInt8 *buff);
		virtual Optional<Math::Geometry::Vector2D> GetVector(UOSInt colIndex);
		virtual Bool GetUUID(UOSInt colIndex, NN<Data::UUID> uuid);

		//virtual WChar *GetName(UOSInt colIndex);
		virtual UnsafeArrayOpt<UTF8Char> GetName(UOSInt colIndex, UnsafeArray<UTF8Char> buff);
		virtual Bool IsNull(UOSInt colIndex);
		virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, OptOut<UOSInt> colSize);
		virtual Bool GetColDef(UOSInt colIndex, NN<DB::ColDef> colDef);

		DB::DBUtil::ColType DBType2ColType(UInt16 dbType);
	};
}
#endif
