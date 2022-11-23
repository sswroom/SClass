#ifndef _SM_DB_DBQUEUE
#define _SM_DB_DBQUEUE
#include "Data/ArrayList.h"
#include "DB/DBTool.h"
#include "Sync/Event.h"

namespace DB
{
	class DBHandler;
	class DBQueue
	{
	public:
		typedef void (__stdcall *DBReadHdlr)(void *userData, void *userData2, DB::DBTool *db, DB::DBReader *r);
		typedef Bool (__stdcall *DBToolHdlr)(void *userData, void *userData2, DB::DBTool *db);

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
			virtual Int32 GetProgId() = 0;
			virtual CmdType GetCmdType() = 0;
		};

		class SQLCmd : public IDBCmd
		{
		public:
			Text::String *str;
			DBReadHdlr hdlr;

			Int32 progId;

			void *userData;
			void *userData2;

			SQLCmd(const UTF8Char *sql, UOSInt sqlLen, Int32 progId, DBReadHdlr hdlr, void *userData, void *userData2);
			virtual ~SQLCmd();			
			virtual CmdType GetCmdType();
			virtual Int32 GetProgId();
			Text::String *GetSQL();
		};

		class SQLGroup : public IDBCmd
		{
		public:
			Data::ArrayList<Text::String*> strs;
			DBReadHdlr hdlr;
			Int32 progId;
			void *userData;
			void *userData2;

			SQLGroup(Data::ArrayList<Text::String*> *strs, Int32 progId, DBReadHdlr hdlr, void *userData, void *userData2);
			virtual ~SQLGroup();
			virtual CmdType GetCmdType();
			virtual Int32 GetProgId();
		};

		class SQLTrans : public IDBCmd
		{
		public:
			DBToolHdlr hdlr;
			void *userData;
			void *userData2;
			Int32 progId;

			SQLTrans(Int32 progId, DBToolHdlr hdlr, void *userData, void *userData2);
			virtual ~SQLTrans();
			virtual CmdType GetCmdType();
			virtual Int32 GetProgId();
		};

		class SQLGetDB : public IDBCmd
		{
		public:
			DBToolHdlr hdlr;
			void *userData;
			void *userData2;
			Int32 progId;

			SQLGetDB(Int32 progId, DBToolHdlr hdlr, void *userData, void *userData2);
			virtual ~SQLGetDB();
			virtual CmdType GetCmdType();
			virtual Int32 GetProgId();
		};

	private:
		DBTool *db1;
		Data::ArrayList<DB::DBHandler *> dbList;

	public:
		Data::ArrayList<IDBCmd*> **sqlList;
		Data::ArrayList<IDBCmd**> **sqlList2;

	public:
		UInt64 sqlCnt;
		UInt64 lostCnt;
		Text::String *name;

	public:
		IO::LogTool *log;
		Bool stopping;

	private:
		Sync::Mutex mut;
		UOSInt dbSize;
		UOSInt nextDB;

	public:
		DBQueue(DBTool *db, IO::LogTool *log, Text::CString name, UOSInt dbSize);
		DBQueue(Data::ArrayList<DBTool*> *dbs, IO::LogTool *log, Text::String *name, UOSInt dbSize);
		DBQueue(Data::ArrayList<DBTool*> *dbs, IO::LogTool *log, Text::CString name, UOSInt dbSize);
		~DBQueue();

		void AddDB(DB::DBTool *db);

		void ToStop();
		void AddSQL(const UTF8Char *sql, UOSInt sqlLen);
		void AddSQL(const UTF8Char *sql, UOSInt sqlLen, Priority priority, Int32 progId, DBReadHdlr hdlr, void *userData, void *userData2);
		void AddTrans(Priority priority, Int32 progId, DBToolHdlr hdlr, void *userData, void *userData2);
		void GetDB(Priority priority, Int32 progId, DBToolHdlr hdlr, void *userData, void *userData2);
		void RemoveSQLs(Int32 progId);
		UOSInt GetDataCnt();
		UOSInt GetQueueCnt();
		UOSInt GetConnCnt();
		DB::DBUtil::SQLType GetSQLType();
		Int8 GetTzQhr();
		UTF8Char *ToString(UTF8Char *buff);
		UOSInt GetNextCmds(IDBCmd **cmds); //max 200 cmds
		UTF8Char *DBDate(UTF8Char *buff, Data::DateTime *dat);
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
		DBTool *db;
		Sync::Event evt;
		Sync::Mutex mut;
		Bool running;
		Bool processing;
		Data::DateTime procTime;

	public:
		DBHandler(DBQueue *dbQ, DBTool *db);
		~DBHandler();

	public:
		UInt32 GetDataCnt();

	private:
		void WriteError(const UTF8Char *errMsg, Text::String *sqlCmd);
		static UInt32 __stdcall ProcessSQL(void *userObj);

	public:
		void Wake();
		Bool IsTimeout(Data::DateTime *currTime);
	};
}
#endif
