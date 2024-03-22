#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/DBQueue.h"
#include "IO/FileStream.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/UTF8Writer.h"

DB::DBQueue::SQLCmd::SQLCmd(const UTF8Char *sql, UOSInt sqlLen, Int32 progId, DB::DBQueue::DBReadHdlr hdlr, AnyType userData, AnyType userData2)
{
	this->str = Text::String::New(sql, sqlLen);
	this->hdlr = hdlr;
	this->userData = userData;
	this->userData2 = userData2;
	this->progId = progId;
}

DB::DBQueue::SQLCmd::~SQLCmd()
{
	this->str->Release();
}

DB::DBQueue::CmdType DB::DBQueue::SQLCmd::GetCmdType() const
{
	return CmdType::SQLCmd;
}

Int32 DB::DBQueue::SQLCmd::GetProgId() const
{
	return this->progId;
}

NotNullPtr<Text::String> DB::DBQueue::SQLCmd::GetSQL() const
{
	return this->str;
}

DB::DBQueue::SQLGroup::SQLGroup(Data::ArrayList<Text::String*> *strs, Int32 progId, DBReadHdlr hdlr, AnyType userData, AnyType userData2)
{
	UOSInt i = 0;
	UOSInt j = strs->GetCount();
	while (i < j)
	{
		this->strs.Add(strs->GetItem(i)->Clone());
		i++;
	}
	this->hdlr = hdlr;
	this->userData = userData;
	this->userData2 = userData2;
	this->progId = progId;
}

DB::DBQueue::SQLGroup::~SQLGroup()
{
	this->strs.FreeAll();
}

DB::DBQueue::CmdType DB::DBQueue::SQLGroup::GetCmdType() const
{
	return CmdType::SQLGroup;
}

Int32 DB::DBQueue::SQLGroup::GetProgId() const
{
	return this->progId;
}

DB::DBQueue::SQLTrans::SQLTrans(Int32 progId, DBToolHdlr hdlr, AnyType userData, AnyType userData2)
{
	this->hdlr = hdlr;
	this->userData = userData;
	this->userData2 = userData2;
	this->progId = progId;
}

DB::DBQueue::SQLTrans::~SQLTrans()
{
}

DB::DBQueue::CmdType DB::DBQueue::SQLTrans::GetCmdType() const
{
	return CmdType::SQLTrans;
}

Int32 DB::DBQueue::SQLTrans::GetProgId() const
{
	return this->progId;
};

DB::DBQueue::SQLGetDB::SQLGetDB(Int32 progId, DBToolHdlr hdlr, AnyType userData, AnyType userData2)
{
	this->hdlr = hdlr;
	this->userData = userData;
	this->userData2 = userData2;
	this->progId = progId;
}

DB::DBQueue::SQLGetDB::~SQLGetDB()
{
}

DB::DBQueue::CmdType DB::DBQueue::SQLGetDB::GetCmdType() const
{
	return CmdType::SQLGetDB;
}

Int32 DB::DBQueue::SQLGetDB::GetProgId() const
{
	return this->progId;
};

DB::DBQueue::DBQueue(NotNullPtr<DBTool> db, IO::LogTool *log, Text::CString name, UOSInt dbSize)
{
	this->db1 = db.Ptr();
	this->dbSize = dbSize / 200;
	sqlList = MemAlloc(Data::ArrayList<IDBCmd*>*, (UOSInt)DB::DBQueue::Priority::Highest + 1);
	sqlList2 = MemAlloc(Data::ArrayList<IDBCmd**>*, (UOSInt)DB::DBQueue::Priority::Highest + 1);
	UOSInt i = (UOSInt)DB::DBQueue::Priority::Highest + 1;
	while (i-- > 0)
	{
		NEW_CLASS(sqlList[i], Data::ArrayList<IDBCmd*>());
		NEW_CLASS(sqlList2[i], Data::ArrayList<IDBCmd**>());
	}
	this->sqlCnt = 0;
	this->lostCnt = 0;
	this->log = log;
	this->name = Text::String::New(name);
	this->nextDB = 0;
	this->stopping = false;
	DB::DBHandler *dbHdlr;
	NEW_CLASS(dbHdlr, DB::DBHandler(this, db));
	this->dbList.Add(dbHdlr);
}

DB::DBQueue::DBQueue(NotNullPtr<Data::ArrayListNN<DBTool>> dbs, IO::LogTool *log, NotNullPtr<Text::String> name, UOSInt dbSize)
{
	this->db1 = dbs->GetItem(0);
	this->dbSize = dbSize / 200;
	sqlList = MemAlloc(Data::ArrayList<IDBCmd*>*, (UOSInt)DB::DBQueue::Priority::Highest + 1);
	sqlList2 = MemAlloc(Data::ArrayList<IDBCmd**>*, (UOSInt)DB::DBQueue::Priority::Highest + 1);
	UOSInt i = (UOSInt)DB::DBQueue::Priority::Highest + 1;
	while (i-- > 0)
	{
		NEW_CLASS(sqlList[i], Data::ArrayList<IDBCmd*>());
		NEW_CLASS(sqlList2[i], Data::ArrayList<IDBCmd**>());
	}
	sqlCnt = 0;
	lostCnt = 0;
	this->log = log;
	this->name = name->Clone();
	this->nextDB = 0;
	stopping = false;
	Data::ArrayIterator<NotNullPtr<DB::DBTool>> it = dbs->Iterator();
	while (it.HasNext())
	{
		DB::DBHandler *dbHdlr;
		NEW_CLASS(dbHdlr, DB::DBHandler(this, it.Next()));
		this->dbList.Add(dbHdlr);
	}
}

DB::DBQueue::DBQueue(NotNullPtr<Data::ArrayListNN<DBTool>> dbs, IO::LogTool *log, Text::CString name, UOSInt dbSize)
{
	this->db1 = dbs->GetItem(0);
	this->dbSize = dbSize / 200;
	sqlList = MemAlloc(Data::ArrayList<IDBCmd*>*, (UOSInt)DB::DBQueue::Priority::Highest + 1);
	sqlList2 = MemAlloc(Data::ArrayList<IDBCmd**>*, (UOSInt)DB::DBQueue::Priority::Highest + 1);
	UOSInt i = (UOSInt)DB::DBQueue::Priority::Highest + 1;
	while (i-- > 0)
	{
		NEW_CLASS(sqlList[i], Data::ArrayList<IDBCmd*>());
		NEW_CLASS(sqlList2[i], Data::ArrayList<IDBCmd**>());
	}
	sqlCnt = 0;
	lostCnt = 0;
	this->log = log;
	this->name = Text::String::New(name);
	this->nextDB = 0;
	stopping = false;
	Data::ArrayIterator<NotNullPtr<DB::DBTool>> it = dbs->Iterator();
	while (it.HasNext())
	{
		DB::DBHandler *dbHdlr;
		NEW_CLASS(dbHdlr, DB::DBHandler(this, it.Next()));
		this->dbList.Add(dbHdlr);
	}
}

DB::DBQueue::~DBQueue()
{
	this->ToStop();
	UOSInt i;
	UOSInt j;
	UOSInt k;
	DB::DBQueue::IDBCmd *c;
	DB::DBQueue::IDBCmd **carr;
	IO::FileStream *fs = 0;
	Text::UTF8Writer *writer = 0;
	DB::DBHandler *dbHdlr;
	i = this->dbList.GetCount();
	while (i-- > 0)
	{
		dbHdlr = this->dbList.GetItem(i);
		DEL_CLASS(dbHdlr);
	}

	Sync::MutexUsage mutUsage(this->mut);
	i = (UOSInt)DB::DBQueue::Priority::Highest + 1;
	while (i-- > 0)
	{
		j = sqlList[i]->GetCount();
		while (j-- > 0)
		{
			c = sqlList[i]->GetItem(j);
			if (c->GetCmdType() == CmdType::SQLCmd)
			{
				NotNullPtr<IO::FileStream> nnfs;
				if (fs == 0)
				{
					NEW_CLASSNN(nnfs, IO::FileStream(CSTR("FailSQL.txt"), IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
					fs = nnfs.Ptr();
					NEW_CLASS(writer, Text::UTF8Writer(nnfs));
				}
				NotNullPtr<Text::String> sql = ((DB::DBQueue::SQLCmd*)c)->GetSQL();
				writer->WriteStrC(sql->v, sql->leng);
				writer->WriteLineC(UTF8STRC(";"));
			}
			DEL_CLASS(c);
		}
		DEL_CLASS(sqlList[i]);
		j = sqlList2[i]->GetCount();
		while (j-- > 0)
		{
			carr = sqlList2[i]->GetItem(j);
			k = 200;
			while (k-- > 0)
			{
				c = carr[k];
				if (c->GetCmdType() == CmdType::SQLCmd)
				{
					NotNullPtr<IO::FileStream> nnfs;
					if (fs == 0)
					{
						NEW_CLASSNN(nnfs, IO::FileStream(CSTR("FailSQL.txt"), IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
						fs = nnfs.Ptr();
						NEW_CLASS(writer, Text::UTF8Writer(nnfs));
					}
					NotNullPtr<Text::String> sql = ((DB::DBQueue::SQLCmd*)c)->GetSQL();
					writer->WriteStrC(sql->v, sql->leng);
					writer->WriteLineC(UTF8STRC(";"));
				}
				DEL_CLASS(c);
			}
			MemFree(carr);
		}
		DEL_CLASS(sqlList2[i]);
	}
	if (fs)
	{
		DEL_CLASS(writer);
		DEL_CLASS(fs);
	}
	mutUsage.EndUse();
	MemFree(sqlList);
	MemFree(sqlList2);
	this->name->Release();
}

void DB::DBQueue::AddDB(NotNullPtr<DB::DBTool> db)
{
	DB::DBHandler *dbHdlr;
	NEW_CLASS(dbHdlr, DB::DBHandler(this, db));
	this->dbList.Add(dbHdlr);
}

void DB::DBQueue::ToStop()
{
	if (!stopping)
	{
		stopping = true;
		UOSInt i;
		i = this->dbList.GetCount();
		while (i-- > 0)
		{
			this->dbList.GetItem(i)->Wake();
		}
	}
}

void DB::DBQueue::AddSQL(const UTF8Char *sql, UOSInt sqlLen)
{
	this->AddSQL(sql, sqlLen, Priority::Lowest, 0, 0, 0, 0);
}

void DB::DBQueue::AddSQL(const UTF8Char *sql, UOSInt sqlLen, Priority priority, Int32 progId, DBReadHdlr hdlr, AnyType userData, AnyType userData2)
{
	if (priority > DB::DBQueue::Priority::Highest)
		priority = DB::DBQueue::Priority::Highest;
	if (priority < DB::DBQueue::Priority::Lowest)
		priority = DB::DBQueue::Priority::Lowest;
	UOSInt ipriority = (UOSInt)priority;
	Sync::MutexUsage mutUsage(this->mut);
	SQLCmd *cmd;
	NEW_CLASS(cmd, SQLCmd(sql, sqlLen, progId, hdlr, userData, userData2));
	sqlList[ipriority]->Add(cmd);
	if (sqlList[ipriority]->GetCount() > 4000)
	{
		IDBCmd *arr[200];
		sqlList[ipriority]->GetRange(arr, 0, 200);
		sqlList[ipriority]->RemoveRange(0, 200);
		if (sqlList2[ipriority]->GetCount() > this->dbSize)
		{
			OSInt i = 10;
			Bool lost = true;
			while (i-- > 0)
			{
				if (sqlList2[ipriority]->GetCount() > this->dbSize)
				{
					Sync::SimpleThread::Sleep(200);
				}
				else
				{
					IDBCmd **sqlArr = MemAlloc(IDBCmd*, 200);
					MemCopyNO(sqlArr, arr, 200 * sizeof(IDBCmd*));
					sqlList2[ipriority]->Add(sqlArr);
					lost = false;
					break;
				}
			}
			if (lost)
				lostCnt += 200;
		}
		else
		{
			IDBCmd **sqlArr = MemAlloc(IDBCmd*, 200);
			MemCopyNO(sqlArr, arr, 200 * sizeof(IDBCmd*));
			sqlList2[ipriority]->Add(sqlArr);
		}
	}
	this->dbList.GetItem(this->nextDB)->Wake();
	this->nextDB = (this->nextDB + 1) % this->dbList.GetCount();
	mutUsage.EndUse();
}

void DB::DBQueue::AddTrans(Priority priority, Int32 progId, DBToolHdlr hdlr, AnyType userData, AnyType userData2)
{
	if (priority > DB::DBQueue::Priority::Highest)
		priority = DB::DBQueue::Priority::Highest;
	if (priority < DB::DBQueue::Priority::Lowest)
		priority = DB::DBQueue::Priority::Lowest;
	UOSInt ipriority = (UOSInt)priority;
	DB::DBQueue::SQLTrans *trans;
	NEW_CLASS(trans, DB::DBQueue::SQLTrans(progId, hdlr, userData, userData2));
	Sync::MutexUsage mutUsage(this->mut);
	sqlList[ipriority]->Add(trans);
	this->dbList.GetItem(this->nextDB)->Wake();
	this->nextDB = (this->nextDB + 1) % this->dbList.GetCount();
	mutUsage.EndUse();
}

void DB::DBQueue::GetDB(Priority priority, Int32 progId, DBToolHdlr hdlr, AnyType userData, AnyType userData2)
{
	if (priority > DB::DBQueue::Priority::Highest)
		priority = DB::DBQueue::Priority::Highest;
	if (priority < DB::DBQueue::Priority::Lowest)
		priority = DB::DBQueue::Priority::Lowest;
	UOSInt ipriority = (UOSInt)priority;
	DB::DBQueue::SQLGetDB *trans;
	NEW_CLASS(trans, DB::DBQueue::SQLGetDB(progId, hdlr, userData, userData2));
	Sync::MutexUsage mutUsage(this->mut);
	sqlList[ipriority]->Add(trans);
	this->dbList.GetItem(this->nextDB)->Wake();
	this->nextDB = (this->nextDB + 1) % this->dbList.GetCount();
	mutUsage.EndUse();
}

void DB::DBQueue::RemoveSQLs(Int32 progId)
{
	UOSInt i = (UOSInt)DB::DBQueue::Priority::Highest + 1;
	UOSInt j;
	DB::DBQueue::IDBCmd *cmd;
	Sync::MutexUsage mutUsage(this->mut);
	while (i-- > 0)
	{
		j = sqlList[i]->GetCount();
		while (j-- > 0)
		{
			cmd = (DB::DBQueue::IDBCmd*)sqlList[i]->GetItem(j);
			if (cmd->GetProgId() == progId)
			{
				sqlList[i]->RemoveAt(j);
				DEL_CLASS(cmd);
			}
		}
	}
	mutUsage.EndUse();
}

UOSInt DB::DBQueue::GetDataCnt() const
{
	UOSInt i = this->dbList.GetCount();
	UOSInt cnt = 0;
	while (i-- > 0)
	{
		cnt += this->dbList.GetItem(i)->GetDataCnt();
	}
	return cnt;
}

UOSInt DB::DBQueue::GetQueueCnt() const
{
	UOSInt cnt = 0;
	UOSInt i = (UOSInt)DB::DBQueue::Priority::Highest + 1;
	Sync::MutexUsage mutUsage(this->mut);
	while (i-- > 0)
	{
		cnt += sqlList2[i]->GetCount() * 200;
		cnt += sqlList[i]->GetCount();
	}
	mutUsage.EndUse();
	return cnt;
}

UOSInt DB::DBQueue::GetConnCnt() const
{
	return this->dbList.GetCount();
}

UTF8Char *DB::DBQueue::ToString(UTF8Char *buff)
{
	return this->name->ConcatTo(buff);
}

DB::SQLType DB::DBQueue::GetSQLType() const
{
	NotNullPtr<DB::DBTool> db;
	if (this->db1.SetTo(db))
		return db->GetSQLType();
	return DB::SQLType::Unknown;
}

Bool DB::DBQueue::IsAxisAware() const
{
	NotNullPtr<DB::DBTool> db;
	if (this->db1.SetTo(db))
		return db->IsAxisAware();
	return false;
}

Int8 DB::DBQueue::GetTzQhr() const
{
	NotNullPtr<DB::DBTool> db;
	if (this->db1.SetTo(db))
		return db->GetTzQhr();
	return 0;
}

UOSInt DB::DBQueue::GetNextCmds(IDBCmd **cmds)
{
	Sync::MutexUsage mutUsage(this->mut);
	void **c;
	UOSInt i;
	UOSInt j;
	UOSInt cnt = 0;

	i = (UOSInt)DB::DBQueue::Priority::Highest + 1;
	while (i-- > 0)
	{
		if (sqlList2[i]->GetCount() > 0)
		{
			c = (void**)sqlList2[i]->GetItem(0);
			sqlList2[i]->RemoveAt(0);
			cnt = 200;
			MemCopyNO(cmds, c, 200 * sizeof(void*));
			MemFree(c);
			break;
		}
		else if (sqlList[i]->GetCount() > 0)
		{
			j = sqlList[i]->GetCount() / 20;
			if (j <= 0)
				j = 1;
			if (j > 200)
				j = 200;

			sqlList[i]->GetRange(cmds, 0, j);
			sqlList[i]->RemoveRange(0, j);
			cnt = j;
			break;
		}
	}
	mutUsage.EndUse();
	return cnt;
}

UTF8Char *DB::DBQueue::DBDateTime(UTF8Char *buff, Data::DateTime *dat)
{
	NotNullPtr<DB::DBTool> db;
	if (this->db1.SetTo(db))
		return db->DBDateTime(buff, dat);
	return DB::DBUtil::SDBDateTime(buff, dat, DB::SQLType::Unknown, 0);
}

UTF8Char *DB::DBQueue::DBInt32(UTF8Char *buff, Int32 val)
{
	return DB::DBUtil::SDBInt32(buff, val, this->GetSQLType());
}

UTF8Char *DB::DBQueue::DBInt64(UTF8Char *buff, Int64 val)
{
	return DB::DBUtil::SDBInt64(buff, val, this->GetSQLType());
}

UTF8Char *DB::DBQueue::DBStrW(UTF8Char *buff, const WChar *val)
{
	return DB::DBUtil::SDBStrW(buff, val, this->GetSQLType());
}

UTF8Char *DB::DBQueue::DBDbl(UTF8Char *buff, Double val)
{
	return DB::DBUtil::SDBDbl(buff, val, this->GetSQLType());
}

UTF8Char *DB::DBQueue::DBBool(UTF8Char *buff, Bool val)
{
	return DB::DBUtil::SDBBool(buff, val, this->GetSQLType());
}

Bool DB::DBQueue::IsExecTimeout()
{
	DB::DBHandler *db;
	UOSInt i;
	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	i = this->dbList.GetCount();
	while (i-- > 0)
	{
		db = this->dbList.GetItem(i);
		if (db->IsTimeout(&dt))
		{
			return true;
		}
	}
	return false;
}

DB::DBHandler::DBHandler(DB::DBQueue *dbQ, NotNullPtr<DB::DBTool> db)
{
	this->processing = false;
	this->dbQ = dbQ;
	this->db = db;
	this->running = false;
	Sync::ThreadUtil::Create(ProcessSQL, this);
}

DB::DBHandler::~DBHandler()
{
	while (this->running)
	{
		Sync::SimpleThread::Sleep(10);
	}
	this->db.Delete();
}

UInt32 DB::DBHandler::GetDataCnt()
{
	return db->GetDataCnt();
}

void DB::DBHandler::WriteError(const UTF8Char *errMsg, NotNullPtr<Text::String> sqlCmd)
{
	this->dbQ->log->LogMessage(CSTR("SQL: Failed"), IO::LogHandler::LogLevel::Error);

	Sync::MutexUsage mutUsage(this->mut);
	{
		IO::FileStream fs(CSTR("FailSQL.txt"), IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		Text::UTF8Writer writer(fs);
		writer.WriteStrC(sqlCmd->v, sqlCmd->leng);
		writer.WriteLineC(UTF8STRC(";"));
	}
	mutUsage.EndUse();
}

UInt32 __stdcall DB::DBHandler::ProcessSQL(AnyType userObj)
{
	NotNullPtr<DB::DBHandler> me = userObj.GetNN<DB::DBHandler>();
	me->running = true;

	NotNullPtr<Text::String> s;
	UOSInt i = 0;
	DB::DBQueue::SQLCmd *cmd;
	DB::DBQueue::SQLGroup *grp;
	DB::DBQueue::IDBCmd *cmds[200];
	UOSInt cmdSize;
	UOSInt l;
	DB::DBQueue::IDBCmd *c;
	OSInt sqlRet;
	Bool found;
	while (!me->dbQ->stopping)
	{
		me->evt.Wait(1000);
		if (me->dbQ->stopping)
			break;
		found = true;
		while (found)
		{
			found = false;
			cmdSize = me->dbQ->GetNextCmds(cmds);
			if (cmdSize > 0)
			{
				me->procTime.SetCurrTimeUTC();
				me->processing = true;
				l = 0;
				while (l < cmdSize)
				{
					c = (DB::DBQueue::IDBCmd *)cmds[l];
					switch (c->GetCmdType())
					{
					case DB::DBQueue::CmdType::SQLCmd:
						me->procTime.SetCurrTimeUTC();
						cmd = (DB::DBQueue::SQLCmd*)c;
						s = cmd->str;
						if (cmd->hdlr == 0)
						{
							i = 3;
							while ((sqlRet = me->db->ExecuteNonQuery(s->ToCString())) == -2)
							{
								i -= 1;
								if (i <= 0)
								{
									me->WriteError((const UTF8Char*)"3 Times", s);
									me->dbQ->sqlCnt -= 1;
									break;
								}
							}
							if (sqlRet == -3)
							{
								me->WriteError((const UTF8Char*)"Data Error", s);
								me->dbQ->sqlCnt -= 1;
							}
							me->dbQ->sqlCnt += 1;
						}
						else
						{
							i = 3;
							Optional<DB::DBReader> r = me->db->ExecuteReader(s->ToCString());
							NotNullPtr<DB::DBReader> nnr;
							while (r.IsNull())
							{
								i -= 1;
								if (i <= 0)
								{
									me->WriteError((const UTF8Char*)"3 Times", s);
									break;
								}
								r = me->db->ExecuteReader(s->ToCString());
							}
							if (r.SetTo(nnr))
							{
								me->dbQ->sqlCnt += 1;
								cmd->hdlr(cmd->userData, cmd->userData2, me->db, nnr);
								me->db->CloseReader(nnr);
							}
							else
							{
								cmd->hdlr(cmd->userData, cmd->userData2, me->db, 0);
							}
						}
						break;
					case DB::DBQueue::CmdType::SQLTrans:
						{
							Bool res = false;
							me->procTime.SetCurrTimeUTC();
							me->db->BeginTrans();
							DB::DBQueue::SQLTrans *obj = (DB::DBQueue::SQLTrans*)c;
							res = obj->hdlr(obj->userData, obj->userData2, me->db);
							if (res == false)
							{
								me->dbQ->log->LogMessage(CSTR("DB Trans failed"), IO::LogHandler::LogLevel::Error);
							}
							me->db->EndTrans(res);
							break;
						}
					case DB::DBQueue::CmdType::SQLGetDB:
						{
							Bool res = false;
							me->procTime.SetCurrTimeUTC();
							DB::DBQueue::SQLGetDB *obj = (DB::DBQueue::SQLGetDB*)c;
							res = obj->hdlr(obj->userData, obj->userData2, me->db);
							if (res == false)
							{
								me->dbQ->log->LogMessage(CSTR("DB GetDB error"), IO::LogHandler::LogLevel::Error);
							}
							break;
						}
					case DB::DBQueue::CmdType::SQLGroup:
						{
							Bool hasError = false;
							grp = (DB::DBQueue::SQLGroup *)c;
							me->procTime.SetCurrTimeUTC();
							me->db->BeginTrans();
							UOSInt k;
							k = 0;
							if (grp->hdlr == 0)
							{
								i = 3;
								while (k < grp->strs.GetCount())
								{
									s = Text::String::OrEmpty(grp->strs.GetItem(k));
									if (me->db->ExecuteNonQuery(s->ToCString()) == -2)
									{
										i -= 1;
										if (i <= 0)
										{
											me->WriteError((const UTF8Char*)"3 Times", s);
											hasError = true;
											break;
										}
									}
									else
									{
										k += 1;
									}
								}
								me->dbQ->sqlCnt += k;
								me->db->EndTrans(!hasError);
							}
							else
							{
								Optional<DB::DBReader> rdr = 0;
								NotNullPtr<DB::DBReader> nnrdr;
								i = 3;
								while (k < grp->strs.GetCount())
								{
									s = Text::String::OrEmpty(grp->strs.GetItem(k));
									rdr = me->db->ExecuteReader(s->ToCString());
									if (!rdr.SetTo(nnrdr))
									{
										i -= 1;
										if (i <= 0)
										{
											me->WriteError((const UTF8Char*)"3 Times", s);
											hasError = true;
											break;
										}
									}
									else
									{
										k += 1;
										if (k < grp->strs.GetCount())
										{
											me->db->CloseReader(nnrdr);
										}
									}
								}
								
								me->dbQ->sqlCnt += k;
								if (rdr.SetTo(nnrdr))
								{
									grp->hdlr(grp->userData, grp->userData2, me->db, nnrdr);
									me->db->CloseReader(nnrdr);
								}
								else
								{
									grp->hdlr(grp->userData, grp->userData2, me->db, 0);
								}
								me->db->EndTrans(!hasError);
							}
						}
						break;
					}
					DEL_CLASS(c);

					found = true;
					l += 1;
				}
				me->processing = false;
			}
		}
	}
	me->running = false;
	return 0;
}

void DB::DBHandler::Wake()
{
	this->evt.Set();
}

Bool DB::DBHandler::IsTimeout(Data::DateTime *currTime)
{
	if (!this->processing)
		return false;
	return currTime->DiffMS(this->procTime) > 60000;
}
