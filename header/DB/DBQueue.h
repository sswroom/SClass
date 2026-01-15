#ifndef _SM_DB_DBQUEUE
#define _SM_DB_DBQUEUE
#include "Data/ArrayListNN.hpp"
#include "DB/DBTool.h"
#include "Sync/Event.h"

namespace DB
{
	class DBHandler;
	class DBQueue
	{
	public:
		typedef void (CALLBACKFUNC DBReadHdlr)(AnyType userData, AnyType userData2, NN<DB::DBTool> db, Optional<DB::DBReader> r);
		typedef Bool (CALLBACKFUNC DBToolHdlr)(AnyType userData, AnyType userData2, NN<DB::DBTool> db);

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
			NN<Text::String> str;
			DBReadHdlr hdlr;

			Int32 progId;

			AnyType userData;
			AnyType userData2;

			SQLCmd(UnsafeArray<const UTF8Char> sql, UIntOS sqlLen, Int32 progId, DBReadHdlr hdlr, AnyType userData, AnyType userData2);
			virtual ~SQLCmd();			
			virtual CmdType GetCmdType() const;
			virtual Int32 GetProgId() const;
			NN<Text::String> GetSQL() const;
		};

		class SQLGroup : public IDBCmd
		{
		public:
			Data::ArrayListStringNN strs;
			DBReadHdlr hdlr;
			Int32 progId;
			AnyType userData;
			AnyType userData2;

			SQLGroup(Data::ArrayListObj<Text::String*> *strs, Int32 progId, DBReadHdlr hdlr, AnyType userData, AnyType userData2);
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
		Data::ArrayListObj<DB::DBHandler *> dbList;

	public:
		Data::ArrayListObj<IDBCmd*> **sqlList;
		Data::ArrayListObj<IDBCmd**> **sqlList2;

	public:
		UInt64 sqlCnt;
		UInt64 lostCnt;
		NN<Text::String> name;

	public:
		IO::LogTool *log;
		Bool stopping;

	private:
		Sync::Mutex mut;
		UIntOS dbSize;
		UIntOS nextDB;

	public:
		DBQueue(NN<DBTool> db, IO::LogTool *log, Text::CStringNN name, UIntOS dbSize);
		DBQueue(NN<Data::ArrayListNN<DBTool>> dbs, IO::LogTool *log, NN<Text::String> name, UIntOS dbSize);
		DBQueue(NN<Data::ArrayListNN<DBTool>> dbs, IO::LogTool *log, Text::CStringNN name, UIntOS dbSize);
		~DBQueue();

		void AddDB(NN<DB::DBTool> db);

		void ToStop();
		void AddSQL(UnsafeArray<const UTF8Char> sql, UIntOS sqlLen);
		void AddSQL(UnsafeArray<const UTF8Char> sql, UIntOS sqlLen, Priority priority, Int32 progId, DBReadHdlr hdlr, AnyType userData, AnyType userData2);
		void AddTrans(Priority priority, Int32 progId, DBToolHdlr hdlr, AnyType userData, AnyType userData2);
		void GetDB(Priority priority, Int32 progId, DBToolHdlr hdlr, AnyType userData, AnyType userData2);
		void RemoveSQLs(Int32 progId);
		UIntOS GetDataCnt() const;
		UIntOS GetQueueCnt() const;
		UIntOS GetConnCnt() const;
		DB::SQLType GetSQLType() const;
		Bool IsAxisAware() const;
		Int8 GetTzQhr() const;
		UnsafeArray<UTF8Char> ToString(UnsafeArray<UTF8Char> buff);
		UIntOS GetNextCmds(IDBCmd **cmds); //max 200 cmds
		UnsafeArray<UTF8Char> DBDateTime(UnsafeArray<UTF8Char> buff, Data::DateTime *dat);
		UnsafeArray<UTF8Char> DBInt32(UnsafeArray<UTF8Char> buff, Int32 val);
		UnsafeArray<UTF8Char> DBInt64(UnsafeArray<UTF8Char> buff, Int64 val);
		UnsafeArray<UTF8Char> DBStrW(UnsafeArray<UTF8Char> buff, const WChar *val);
		UnsafeArray<UTF8Char> DBDbl(UnsafeArray<UTF8Char> buff, Double val);
		UnsafeArray<UTF8Char> DBBool(UnsafeArray<UTF8Char> buff, Bool val);
		Bool IsExecTimeout();
	};

	class DBHandler
	{
	private:
		NN<DBQueue> dbQ;
		NN<DBTool> db;
		Sync::Event evt;
		Sync::Mutex mut;
		Bool running;
		Bool processing;
		Data::DateTime procTime;

	public:
		DBHandler(NN<DBQueue> dbQ, NN<DBTool> db);
		~DBHandler();

	public:
		UInt32 GetDataCnt();

	private:
		void WriteError(UnsafeArray<const UTF8Char> errMsg, NN<Text::String> sqlCmd);
		static UInt32 __stdcall ProcessSQL(AnyType userObj);

	public:
		void Wake();
		Bool IsTimeout(NN<Data::DateTime> currTime);
	};
}
#endif
