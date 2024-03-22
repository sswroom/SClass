#ifndef _SM_DB_DBQUEUE
#define _SM_DB_DBQUEUE
#include "Data/ArrayList.h"
#include "Data/ArrayListNN.h"
#include "DB/DBTool.h"
#include "Sync/Event.h"

namespace DB
{
	class DBHandler;
	class DBQueue
	{
	public:
		typedef void (__stdcall *DBReadHdlr)(AnyType userData, AnyType userData2, NotNullPtr<DB::DBTool> db, Optional<DB::DBReader> r);
		typedef Bool (__stdcall *DBToolHdlr)(AnyType userData, AnyType userData2, NotNullPtr<DB::DBTool> db);

		enum class Priority
		{
			Priority_0,
			Priority_1,
			Priority_2,
			Priority_3,
			Priority_4,
			Priority_5,
			Priority_6,
			Priority_7,
			Priority_8,
			Priority_9,

			Lowest = Priority_0,
			Highest = Priority_9
		};

		enum class CmdType
		{
			SQLCmd,
			SQLGroup,
			SQLTrans,
			SQLGetDB
		};


		class IDBCmd
		{
		public:
			virtual ~IDBCmd(){};
			virtual Int32 GetProgId() const = 0;
			virtual CmdType GetCmdType() const = 0;
		};

		class SQLCmd : public IDBCmd
		{
		public:
			NotNullPtr<Text::String> str;
			DBReadHdlr hdlr;

			Int32 progId;

			AnyType userData;
			AnyType userData2;

			SQLCmd(const UTF8Char *sql, UOSInt sqlLen, Int32 progId, DBReadHdlr hdlr, AnyType userData, AnyType userData2);
			virtual ~SQLCmd();			
			virtual CmdType GetCmdType() const;
			virtual Int32 GetProgId() const;
			NotNullPtr<Text::String> GetSQL() const;
		};

		class SQLGroup : public IDBCmd
		{
		public:
			Data::ArrayListStringNN strs;
			DBReadHdlr hdlr;
			Int32 progId;
			AnyType userData;
			AnyType userData2;

			SQLGroup(Data::ArrayList<Text::String*> *strs, Int32 progId, DBReadHdlr hdlr, AnyType userData, AnyType userData2);
			virtual ~SQLGroup();
			virtual CmdType GetCmdType() const;
			virtual Int32 GetProgId() const;
		};

		class SQLTrans : public IDBCmd
		{
		public:
			DBToolHdlr hdlr;
			AnyType userData;
			AnyType userData2;
			Int32 progId;

			SQLTrans(Int32 progId, DBToolHdlr hdlr, AnyType userData, AnyType userData2);
			virtual ~SQLTrans();
			virtual CmdType GetCmdType() const;
			virtual Int32 GetProgId() const;
		};

		class SQLGetDB : public IDBCmd
		{
		public:
			DBToolHdlr hdlr;
			AnyType userData;
			AnyType userData2;
			Int32 progId;

			SQLGetDB(Int32 progId, DBToolHdlr hdlr, AnyType userData, AnyType userData2);
			virtual ~SQLGetDB();
			virtual CmdType GetCmdType() const;
			virtual Int32 GetProgId() const;
		};

	private:
		Optional<DBTool> db1;
		Data::ArrayList<DB::DBHandler *> dbList;

	public:
		Data::ArrayList<IDBCmd*> **sqlList;
		Data::ArrayList<IDBCmd**> **sqlList2;

	public:
		UInt64 sqlCnt;
		UInt64 lostCnt;
		NotNullPtr<Text::String> name;

	public:
		IO::LogTool *log;
		Bool stopping;

	private:
		Sync::Mutex mut;
		UOSInt dbSize;
		UOSInt nextDB;

	public:
		DBQueue(NotNullPtr<DBTool> db, IO::LogTool *log, Text::CString name, UOSInt dbSize);
		DBQueue(NotNullPtr<Data::ArrayListNN<DBTool>> dbs, IO::LogTool *log, NotNullPtr<Text::String> name, UOSInt dbSize);
		DBQueue(NotNullPtr<Data::ArrayListNN<DBTool>> dbs, IO::LogTool *log, Text::CString name, UOSInt dbSize);
		~DBQueue();

		void AddDB(NotNullPtr<DB::DBTool> db);

		void ToStop();
		void AddSQL(const UTF8Char *sql, UOSInt sqlLen);
		void AddSQL(const UTF8Char *sql, UOSInt sqlLen, Priority priority, Int32 progId, DBReadHdlr hdlr, AnyType userData, AnyType userData2);
		void AddTrans(Priority priority, Int32 progId, DBToolHdlr hdlr, AnyType userData, AnyType userData2);
		void GetDB(Priority priority, Int32 progId, DBToolHdlr hdlr, AnyType userData, AnyType userData2);
		void RemoveSQLs(Int32 progId);
		UOSInt GetDataCnt() const;
		UOSInt GetQueueCnt() const;
		UOSInt GetConnCnt() const;
		DB::SQLType GetSQLType() const;
		Bool IsAxisAware() const;
		Int8 GetTzQhr() const;
		UTF8Char *ToString(UTF8Char *buff);
		UOSInt GetNextCmds(IDBCmd **cmds); //max 200 cmds
		UTF8Char *DBDateTime(UTF8Char *buff, Data::DateTime *dat);
		UTF8Char *DBInt32(UTF8Char *buff, Int32 val);
		UTF8Char *DBInt64(UTF8Char *buff, Int64 val);
		UTF8Char *DBStrW(UTF8Char *buff, const WChar *val);
		UTF8Char *DBDbl(UTF8Char *buff, Double val);
		UTF8Char *DBBool(UTF8Char *buff, Bool val);
		Bool IsExecTimeout();
	};

	class DBHandler
	{
	private:
		DBQueue *dbQ;
		NotNullPtr<DBTool> db;
		Sync::Event evt;
		Sync::Mutex mut;
		Bool running;
		Bool processing;
		Data::DateTime procTime;

	public:
		DBHandler(DBQueue *dbQ, NotNullPtr<DBTool> db);
		~DBHandler();

	public:
		UInt32 GetDataCnt();

	private:
		void WriteError(const UTF8Char *errMsg, NotNullPtr<Text::String> sqlCmd);
		static UInt32 __stdcall ProcessSQL(AnyType userObj);

	public:
		void Wake();
		Bool IsTimeout(Data::DateTime *currTime);
	};
}
#endif
