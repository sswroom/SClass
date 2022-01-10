#ifndef _SM_DB_SQLITEFILE
#define _SM_DB_SQLITEFILE
#include "Data/ArrayListStrUTF8.h"
#include "DB/DBConn.h"
#include "DB/DBReader.h"
#include "DB/DBTool.h"
#include "IO/IStreamData.h"
#include "Text/StringBuilderUTF.h"

namespace DB
{
	class SQLiteFile : public DB::DBConn
	{
	private:
		Text::String *fileName;
		void *db;
		Data::ArrayListStrUTF8 *tableNames;
		Bool delOnClose;
		Text::String *lastErrMsg;

		void Init();
	public:
		SQLiteFile(Text::String *fileName);
		SQLiteFile(const UTF8Char *fileName);
		virtual ~SQLiteFile();

		virtual DB::DBUtil::ServerType GetSvrType();
		virtual ConnType GetConnType();
		virtual Int8 GetTzQhr();
		virtual void ForceTz(Int8 tzQhr);
		virtual void GetConnName(Text::StringBuilderUTF *sb);
		virtual void Close();
		virtual OSInt ExecuteNonQuery(const UTF8Char *sql);
		virtual OSInt ExecuteNonQueryC(const UTF8Char *sql, UOSInt sqlLen);
		virtual DBReader *ExecuteReader(const UTF8Char *sql);
		virtual DBReader *ExecuteReaderC(const UTF8Char *sql, UOSInt sqlLen);
		virtual void CloseReader(DBReader *r);
		virtual void GetErrorMsg(Text::StringBuilderUTF *str);
		virtual Bool IsLastDataError();
		virtual void Reconnect();

		virtual void *BeginTransaction();
		virtual void Commit(void *tran);
		virtual void Rollback(void *tran);

		virtual UOSInt GetTableNames(Data::ArrayList<const UTF8Char*> *names); // no need to release
		virtual DBReader *GetTableData(const UTF8Char *tableName, Data::ArrayList<Text::String*> *columnNames, UOSInt ofst, UOSInt maxCnt, const UTF8Char *ordering, Data::QueryConditions *condition);

		void SetDeleteOnClose(Bool delOnClose);
		Bool IsError();
		Text::String *GetFileName();

		static DBTool *CreateDBTool(Text::String *fileName, IO::LogTool *log, const UTF8Char *logPrefix);
		static DBTool *CreateDBTool(const UTF8Char *fileName, IO::LogTool *log, const UTF8Char *logPrefix);
	};

	class SQLiteReader : public DB::DBReader
	{
	private:
		typedef struct
		{
			DB::DBUtil::ColType colType;
			void *colData;
			Int64 dataVal;
			Bool isNull;
		} ColumnData;
	private:
		SQLiteFile *conn;
		void *hStmt;
		UOSInt colCnt;
		DB::DBUtil::ColType *colTypes;
		Bool isFirst;
		Bool firstResult;
		
		void UpdateColTypes();
	public:
		SQLiteReader(SQLiteFile *conn, void *hStmt);
		~SQLiteReader();

		virtual Bool ReadNext();
		virtual UOSInt ColCount();
		virtual OSInt GetRowChanged();

		virtual Int32 GetInt32(UOSInt colIndex);
		virtual Int64 GetInt64(UOSInt colIndex);
		virtual WChar *GetStr(UOSInt colIndex, WChar *buff);
		virtual Bool GetStr(UOSInt colIndex, Text::StringBuilderUTF *sb);
		virtual Text::String *GetNewStr(UOSInt colIndex);
		virtual UTF8Char *GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize);
		virtual DateErrType GetDate(UOSInt colIndex, Data::DateTime *outVal);
		virtual Double GetDbl(UOSInt colIndex);
		virtual Bool GetBool(UOSInt colIndex);
		virtual UOSInt GetBinarySize(UOSInt colIndex);
		virtual UOSInt GetBinary(UOSInt colIndex, UInt8 *buff);
		virtual Math::Vector2D *GetVector(UOSInt colIndex);
		virtual Bool GetUUID(UOSInt colIndex, Data::UUID *uuid);

		virtual UTF8Char *GetName(UOSInt colIndex, UTF8Char *buff);
		virtual Bool IsNull(UOSInt colIndex);
		virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, UOSInt *colSize);
		virtual Bool GetColDef(UOSInt colIndex, DB::ColDef *colDef);
	};
}
#endif
