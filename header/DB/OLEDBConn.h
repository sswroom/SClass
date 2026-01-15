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
		void Init(UnsafeArray<const WChar> connStr);
	public:
		OLEDBConn(UnsafeArray<const WChar> connStr, NN<IO::LogTool> log);
		virtual ~OLEDBConn();

		virtual DB::SQLType GetSQLType() const;
		virtual ConnType GetConnType() const;
		virtual Int8 GetTzQhr() const;
		virtual void ForceTz(Int8 tzQhr);
		virtual void GetConnName(NN<Text::StringBuilderUTF8> sb);
		virtual void Close();
		virtual IntOS ExecuteNonQuery(Text::CStringNN sql);
//		virtual IntOS ExecuteNonQuery(const WChar *sql);
		virtual void GetLastErrorMsg(NN<Text::StringBuilderUTF8> str);
		virtual Bool IsLastDataError();
		virtual void Reconnect();

		virtual UIntOS QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names);
		virtual Optional<DBReader> QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> columnNames, UIntOS ofst, UIntOS maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition);
		virtual Optional<DBReader> ExecuteReader(Text::CStringNN sql);
//		virtual DBReader *ExecuteReader(const WChar *sql);
		virtual void CloseReader(NN<DBReader> r);

		virtual Optional<DB::DBTransaction> BeginTransaction();
		virtual void Commit(NN<DB::DBTransaction> tran);
		virtual void Rollback(NN<DB::DBTransaction> tran);

		ConnError GetConnError();
		UnsafeArrayOpt<const WChar> GetConnStr();
		static const ProviderInfo *GetProviderInfo(const UTF8Char *providerName);
	};

	class OLEDBReader : public DB::DBReader
	{
	private:
		struct ClassData;

		ClassData *clsData;

	public:
		OLEDBReader(void *pIRowset, IntOS rowChanged);
		virtual ~OLEDBReader();

		virtual Bool ReadNext();
		virtual UIntOS ColCount();
		virtual IntOS GetRowChanged();

		virtual Int32 GetInt32(UIntOS colIndex);
		virtual Int64 GetInt64(UIntOS colIndex);
		virtual UnsafeArrayOpt<WChar> GetStr(UIntOS colIndex, UnsafeArray<WChar> buff);
		virtual Bool GetStr(UIntOS colIndex, NN<Text::StringBuilderUTF8> sb);
		virtual Optional<Text::String> GetNewStr(UIntOS colIndex);
		virtual UnsafeArrayOpt<UTF8Char> GetStr(UIntOS colIndex, UnsafeArray<UTF8Char> buff, UIntOS buffSize);
		virtual Data::Timestamp GetTimestamp(UIntOS colIndex);
		virtual Double GetDblOrNAN(UIntOS colIndex);
		virtual Bool GetBool(UIntOS colIndex);
		virtual UIntOS GetBinarySize(UIntOS colIndex);
		virtual UIntOS GetBinary(UIntOS colIndex, UnsafeArray<UInt8> buff);
		virtual Optional<Math::Geometry::Vector2D> GetVector(UIntOS colIndex);
		virtual Bool GetUUID(UIntOS colIndex, NN<Data::UUID> uuid);

		//virtual WChar *GetName(UIntOS colIndex);
		virtual UnsafeArrayOpt<UTF8Char> GetName(UIntOS colIndex, UnsafeArray<UTF8Char> buff);
		virtual Bool IsNull(UIntOS colIndex);
		virtual DB::DBUtil::ColType GetColType(UIntOS colIndex, OptOut<UIntOS> colSize);
		virtual Bool GetColDef(UIntOS colIndex, NN<DB::ColDef> colDef);

		DB::DBUtil::ColType DBType2ColType(UInt16 dbType);
	};
}
#endif
