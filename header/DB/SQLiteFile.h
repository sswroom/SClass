#ifndef _SM_DB_SQLITEFILE
#define _SM_DB_SQLITEFILE
#include "Data/ArrayListStrUTF8.h"
#include "DB/DBConn.h"
#include "DB/DBReader.h"
#include "DB/DBTool.h"
#include "IO/StreamData.h"
#include "Text/StringBuilderUTF8.h"

namespace DB
{
	class SQLiteFile : public DB::DBConn
	{
	private:
		Text::String *fileName;
		void *db;
//		Data::ArrayList<Text::CString> tableNames;
		Bool delOnClose;
		Text::String *lastErrMsg;

		void Init();
	public:
		SQLiteFile(Text::String *fileName);
		SQLiteFile(Text::CString fileName);
		virtual ~SQLiteFile();

		virtual DB::DBUtil::SQLType GetSQLType() const;
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

		void SetDeleteOnClose(Bool delOnClose);
		Bool IsError();
		Text::String *GetFileName();

		static Math::Geometry::Vector2D *GPGeometryParse(const UInt8 *buff, UOSInt buffSize);
		
		static DBTool *CreateDBTool(Text::String *fileName, IO::LogTool *log, Text::CString logPrefix);
		static DBTool *CreateDBTool(Text::CString fileName, IO::LogTool *log, Text::CString logPrefix);
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
		virtual Bool GetStr(UOSInt colIndex, Text::StringBuilderUTF8 *sb);
		virtual Text::String *GetNewStr(UOSInt colIndex);
		virtual UTF8Char *GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize);
		virtual Data::Timestamp GetTimestamp(UOSInt colIndex);
		virtual Double GetDbl(UOSInt colIndex);
		virtual Bool GetBool(UOSInt colIndex);
		virtual UOSInt GetBinarySize(UOSInt colIndex);
		virtual UOSInt GetBinary(UOSInt colIndex, UInt8 *buff);
		virtual Math::Geometry::Vector2D *GetVector(UOSInt colIndex);
		virtual Bool GetUUID(UOSInt colIndex, Data::UUID *uuid);

		virtual UTF8Char *GetName(UOSInt colIndex, UTF8Char *buff);
		virtual Bool IsNull(UOSInt colIndex);
		virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, UOSInt *colSize);
		virtual Bool GetColDef(UOSInt colIndex, DB::ColDef *colDef);
	};
}
#endif
