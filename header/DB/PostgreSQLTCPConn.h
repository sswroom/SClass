#ifndef _SM_DB_POSTGRESQLTCPCONN
#define _SM_DB_POSTGRESQLTCPCONN
#include "Data/ArrayListNative.hpp"
#include "Data/ArrayListNN.hpp"
#include "Data/ArrayListStringNN.h"
#include "DB/DBConn.h"
#include "DB/DBReader.h"
#include "DB/DBTool.h"
#include "IO/LogTool.h"
#include "Net/TCPClientFactory.h"

namespace DB
{
	class PostgreSQLTCPConn : public DB::DBConn
	{
	protected:
		struct ClassData;
		NN<ClassData> clsData;

		Bool isTran;
		NN<Text::String> server;
		UInt16 port;
		NN<Text::String> database;
		Optional<Text::String> uid;
		Optional<Text::String> pwd;
		NN<IO::LogTool> log;
		Int8 tzQhr;

		Bool Connect();
		void InitConnection();
		Int32 readPacket(UnsafeArray<UInt8> buff, UIntOS buffSize);
		Bool sendPacket(UInt8 msgType, UnsafeArray<UInt8> data, UIntOS dataLen);
		Bool parseAuthentication();
		Bool parseBackendKeyData();
		Bool sendStartupPacket(Text::CStringNN user, Text::CStringNN database);
		Bool parseRowDescription(NN<Data::ArrayListStringNN> colNames, NN<Data::ArrayListNative<UInt32>> types, NN<Data::ArrayListNative<Int32>> typeMods);
		Bool parseDataRow(UIntOS colCount, NN<Data::ArrayListObj<UnsafeArray<UInt8>>> values, NN<Data::ArrayListNative<UInt32>> lengths);
		Bool parseCommandComplete(OutParam<IntOS> rowChanged);
		Bool parseErrorResponse(NN<Text::StringBuilderUTF8> errMsg);

	public:
		PostgreSQLTCPConn(NN<Text::String> server, UInt16 port, Optional<Text::String> uid, Optional<Text::String> pwd, NN<Text::String> database, NN<IO::LogTool> log);
		PostgreSQLTCPConn(Text::CStringNN server, UInt16 port, Text::CString uid, Text::CString pwd, Text::CStringNN database, NN<IO::LogTool> log);
		virtual ~PostgreSQLTCPConn();
		virtual DB::SQLType GetSQLType() const;
		virtual ConnType GetConnType() const;
		virtual void GetConnName(NN<Text::StringBuilderUTF8> sb);
		virtual void Close();
		virtual void Dispose();
		virtual IntOS ExecuteNonQuery(Text::CStringNN sql);
		virtual Optional<DBReader> ExecuteReader(Text::CStringNN sql);
		virtual void CloseReader(NN<DB::DBReader> r);
		virtual void GetLastErrorMsg(NN<Text::StringBuilderUTF8> str);
		virtual Bool IsLastDataError();
		virtual void Reconnect();
		virtual Int8 GetTzQhr() const;
		virtual void ForceTzQhr(Int8 tzQhr);

		virtual Optional<DB::DBTransaction> BeginTransaction();
		virtual void Commit(NN<DB::DBTransaction> tran);
		virtual void Rollback(NN<DB::DBTransaction> tran);

		virtual UIntOS QuerySchemaNames(NN<Data::ArrayListStringNN> names);
		virtual UIntOS QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names);
		virtual Optional<DBReader> QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> columnNames, UIntOS ofst, UIntOS maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition);

		Bool IsConnError();
		NN<Text::String> GetConnServer() const;
		UInt16 GetConnPort() const;
		NN<Text::String> GetConnDB() const;
		Optional<Text::String> GetConnUID() const;
		Optional<Text::String> GetConnPWD() const;
		Bool ChangeDatabase(Text::CStringNN databaseName);
		
		UInt32 GetBackendPID() const;
		Int32 GetCancelKey() const;

		static Optional<DBTool> CreateDBTool(NN<Net::TCPClientFactory> clif, NN<Text::String> serverName, UInt16 port, Optional<Text::String> dbName, NN<Text::String> uid, NN<Text::String> pwd, NN<IO::LogTool> log, Text::CString logPrefix);
		static Optional<DBTool> CreateDBTool(NN<Net::TCPClientFactory> clif, Text::CStringNN serverName, UInt16 port, Text::CString dbName, Text::CStringNN uid, Text::CStringNN pwd, NN<IO::LogTool> log, Text::CString logPrefix);
	};

	class PostgreSQLTCPReader : public DB::DBReader
	{
	private:
		Int8 tzQhr;
		UInt32 backendPID;
		Int32 cancelKey;
		
		Data::ArrayListNative<UnsafeArray<UInt8>> rowValues;
		Data::ArrayListNative<UInt32> valueLengths;
		Data::ArrayListNative<UInt32> columnTypes;
		Data::ArrayListNative<Int32> columnTypeMods;
		Data::ArrayListNN<UTF8Char*> columnNames;
		UIntOS colCount;
		UIntOS rowCount;
		UIntOS currRow;
		Int32 readPacket(UnsafeArray<UInt8> buff, UIntOS buffSize);
		Bool sendPacket(UInt8 msgType, UnsafeArray<UInt8> data, UIntOS dataLen);
		Bool parseAuthentication();
		Bool parseBackendKeyData();
		Bool sendStartupPacket(Text::CStringNN user, Text::CStringNN database);
		Bool parseRowDescription(NN<Data::ArrayListStringNN> colNames, NN<Data::ArrayListNative<UInt32>> types, NN<Data::ArrayListNative<Int32>> typeMods);
		Bool parseDataRow(UIntOS colCount, NN<Data::ArrayListObj<UnsafeArray<UInt8>>> values, NN<Data::ArrayListNative<UInt32>> lengths);
		Bool parseCommandComplete(OutParam<IntOS> rowChanged);
		Bool parseErrorResponse(NN<Text::StringBuilderUTF8> errMsg);

	public:
		PostgreSQLTCPReader(UInt32 backendPID, Int32 cancelKey, NN<Data::ArrayListStringNN> colNames, NN<Data::ArrayListNative<UnsafeArray<UInt8>>> values, NN<Data::ArrayListNative<UInt32>> lengths, NN<Data::ArrayListNative<UInt32>> types, NN<Data::ArrayListNative<Int32>> typeMods);
		virtual ~PostgreSQLTCPReader();

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

		virtual UnsafeArrayOpt<UTF8Char> GetName(UIntOS colIndex, UnsafeArray<UTF8Char> buff);
		virtual Bool IsNull(UIntOS colIndex);
		virtual DB::DBUtil::ColType GetColType(UIntOS colIndex, OptOut<UIntOS> colSize);
		virtual Bool GetColDef(UIntOS colIndex, NN<DB::ColDef> colDef);

		DB::DBUtil::ColType DBType2ColType(UInt32 dbType);
	};
}

#endif
