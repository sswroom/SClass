#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/DBQueue.h"
#include "IO/FileStream.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/UTF8Writer.h"

DB::DBQueue::SQLCmd::SQLCmd(const UTF8Char *str, Int32 progId, DB::DBQueue::DBReadHdlr hdlr, void *userData, void *userData2)
{
	this->str = Text::StrCopyNew(str);
	this->hdlr = hdlr;
	this->userData = userData;
	this->userData2 = userData2;
	this->progId = progId;
}

DB::DBQueue::SQLCmd::~SQLCmd()
{
	Text::StrDelNew(this->str);
}

DB::DBQueue::CmdType DB::DBQueue::SQLCmd::GetCmdType()
{
	return CmdType::SQLCmd;
}

Int32 DB::DBQueue::SQLCmd::GetProgId()
{
	return this->progId;
}

const UTF8Char *DB::DBQueue::SQLCmd::GetSQL()
{
	return this->str;
}

DB::DBQueue::SQLGroup::SQLGroup(Data::ArrayList<const UTF8Char*> *strs, Int32 progId, DBReadHdlr hdlr, void *userData, void *userData2)
{
	UOSInt i = 0;
	UOSInt j = strs->GetCount();
	NEW_CLASS(this->strs, Data::ArrayList<const UTF8Char*>());
	while (i < j)
	{
		this->strs->Add(Text::StrCopyNew(strs->GetItem(i)));
		i++;
	}
	this->hdlr = hdlr;
	this->userData = userData;
	this->userData2 = userData2;
	this->progId = progId;
}

DB::DBQueue::SQLGroup::~SQLGroup()
{
	UOSInt i = this->strs->GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(this->strs->GetItem(i));
	}
	DEL_CLASS(this->strs);
}

DB::DBQueue::CmdType DB::DBQueue::SQLGroup::GetCmdType()
{
	return CmdType::SQLGroup;
}

Int32 DB::DBQueue::SQLGroup::GetProgId()
{
	return this->progId;
}

DB::DBQueue::SQLTrans::SQLTrans(Int32 progId, DBToolHdlr hdlr, void *userData, void *userData2)
{
	this->hdlr = hdlr;
	this->userData = userData;
	this->userData2 = userData2;
	this->progId = progId;
}

DB::DBQueue::SQLTrans::~SQLTrans()
{
}

DB::DBQueue::CmdType DB::DBQueue::SQLTrans::GetCmdType()
{
	return CmdType::SQLTrans;
}

Int32 DB::DBQueue::SQLTrans::GetProgId()
{
	return this->progId;
};

DB::DBQueue::SQLGetDB::SQLGetDB(Int32 progId, DBToolHdlr hdlr, void *userData, void *userData2)
{
	this->hdlr = hdlr;
	this->userData = userData;
	this->userData2 = userData2;
	this->progId = progId;
}

DB::DBQueue::SQLGetDB::~SQLGetDB()
{
}

DB::DBQueue::CmdType DB::DBQueue::SQLGetDB::GetCmdType()
{
	return CmdType::SQLGetDB;
}

Int32 DB::DBQueue::SQLGetDB::GetProgId()
{
	return this->progId;
};

DB::DBQueue::DBQueue(DBTool *db, IO::LogTool *log, const UTF8Char *name, UOSInt dbSize)
{
	this->db1 = db;
	this->dbSize = dbSize / 200;
	NEW_CLASS(this->mut, Sync::Mutex());
	sqlList = MemAlloc(Data::ArrayList<IDBCmd*>*, DB_DBQUEUE_PRIORITY_HIGHEST + 1);
	sqlList2 = MemAlloc(Data::ArrayList<IDBCmd**>*, DB_DBQUEUE_PRIORITY_HIGHEST + 1);
	OSInt i = DB_DBQUEUE_PRIORITY_HIGHEST + 1;
	while (i-- > 0)
	{
		NEW_CLASS(sqlList[i], Data::ArrayList<IDBCmd*>());
		NEW_CLASS(sqlList2[i], Data::ArrayList<IDBCmd**>());
	}
	this->sqlCnt = 0;
	this->lostCnt = 0;
	this->log = log;
	this->name = Text::StrCopyNew(name);
	this->nextDB = 0;
	this->stopping = false;
	NEW_CLASS(dbList, Data::ArrayList<DB::DBHandler *>());
	DB::DBHandler *dbHdlr;
	NEW_CLASS(dbHdlr, DB::DBHandler(this, db));
	dbList->Add(dbHdlr);
}

DB::DBQueue::DBQueue(Data::ArrayList<DBTool*> *dbs, IO::LogTool *log, const UTF8Char *name, UOSInt dbSize)
{
	this->db1 = dbs->GetItem(0);
	NEW_CLASS(this->mut, Sync::Mutex());
	this->dbSize = dbSize / 200;
	sqlList = MemAlloc(Data::ArrayList<IDBCmd*>*, DB_DBQUEUE_PRIORITY_HIGHEST + 1);
	sqlList2 = MemAlloc(Data::ArrayList<IDBCmd**>*, DB_DBQUEUE_PRIORITY_HIGHEST + 1);
	UOSInt i = DB_DBQUEUE_PRIORITY_HIGHEST + 1;
	while (i-- > 0)
	{
		NEW_CLASS(sqlList[i], Data::ArrayList<IDBCmd*>());
		NEW_CLASS(sqlList2[i], Data::ArrayList<IDBCmd**>());
	}
	sqlCnt = 0;
	lostCnt = 0;
	this->log = log;
	this->name = Text::StrCopyNew(name);
	this->nextDB = 0;
	stopping = false;
	NEW_CLASS(dbList, Data::ArrayList<DBHandler*>())
	i = 0;
	while (i < dbs->GetCount())
	{
		DB::DBHandler *dbHdlr;
		NEW_CLASS(dbHdlr, DB::DBHandler(this, (DB::DBTool *)dbs->GetItem(i)));
		dbList->Add(dbHdlr);
		i += 1;
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
	i = dbList->GetCount();
	while (i-- > 0)
	{
		dbHdlr = dbList->GetItem(i);
		DEL_CLASS(dbHdlr);
	}

	Sync::MutexUsage mutUsage(this->mut);
	i = DB_DBQUEUE_PRIORITY_HIGHEST + 1;
	while (i-- > 0)
	{
		j = sqlList[i]->GetCount();
		while (j-- > 0)
		{
			c = sqlList[i]->GetItem(j);
			if (c->GetCmdType() == CmdType::SQLCmd)
			{
				if (fs == 0)
				{
					NEW_CLASS(fs, IO::FileStream((const UTF8Char*)"FailSQL.txt", IO::FileStream::FileMode::Append, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
					NEW_CLASS(writer, Text::UTF8Writer(fs));
				}
				writer->Write(((DB::DBQueue::SQLCmd*)c)->GetSQL());
				writer->WriteLine((const UTF8Char*)";");
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
					if (fs == 0)
					{
						NEW_CLASS(fs, IO::FileStream((const UTF8Char*)"FailSQL.txt", IO::FileStream::FileMode::Append, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
						NEW_CLASS(writer, Text::UTF8Writer(fs));
					}
					writer->Write(((DB::DBQueue::SQLCmd*)c)->GetSQL());
					writer->WriteLine((const UTF8Char*)";");
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
	Text::StrDelNew(this->name);
	DEL_CLASS(dbList);
	DEL_CLASS(this->mut);
}

void DB::DBQueue::AddDB(DB::DBTool *db)
{
	DB::DBHandler *dbHdlr;
	NEW_CLASS(dbHdlr, DB::DBHandler(this, db));
	dbList->Add(dbHdlr);
}

void DB::DBQueue::ToStop()
{
	if (!stopping)
	{
		stopping = true;
		UOSInt i;
		i = dbList->GetCount();
		while (i-- > 0)
		{
			((DB::DBHandler *)dbList->GetItem(i))->Wake();
		}
	}
}

void DB::DBQueue::AddSQL(const UTF8Char *str)
{
	this->AddSQL(str, 0, 0, 0, 0, 0);
}

void DB::DBQueue::AddSQL(const UTF8Char *str, Int32 priority, Int32 progId, DBReadHdlr hdlr, void *userData, void *userData2)
{
	if (priority > DB_DBQUEUE_PRIORITY_HIGHEST)
		priority = DB_DBQUEUE_PRIORITY_HIGHEST;
	if (priority < DB_DBQUEUE_PRIORITY_LOWEST)
		priority = DB_DBQUEUE_PRIORITY_LOWEST;
	Sync::MutexUsage mutUsage(mut);
	SQLCmd *cmd;
	NEW_CLASS(cmd, SQLCmd(str, progId, hdlr, userData, userData2));
	sqlList[priority]->Add(cmd);
	if (sqlList[priority]->GetCount() > 4000)
	{
		IDBCmd *arr[200];
		sqlList[priority]->GetRange(arr, 0, 200);
		sqlList[priority]->RemoveRange(0, 200);
		if (sqlList2[priority]->GetCount() > this->dbSize)
		{
			OSInt i = 10;
			Bool lost = true;
			while (i-- > 0)
			{
				if (sqlList2[priority]->GetCount() > this->dbSize)
				{
					Sync::Thread::Sleep(200);
				}
				else
				{
					IDBCmd **sqlArr = MemAlloc(IDBCmd*, 200);
					MemCopyNO(sqlArr, arr, 200 * sizeof(IDBCmd*));
					sqlList2[priority]->Add(sqlArr);
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
			sqlList2[priority]->Add(sqlArr);
		}
	}
	((DB::DBHandler*)this->dbList->GetItem(this->nextDB))->Wake();
	this->nextDB = (this->nextDB + 1) % this->dbList->GetCount();
	mutUsage.EndUse();
}

void DB::DBQueue::AddTrans(Int32 priority, Int32 progId, DBToolHdlr hdlr, void *userData, void *userData2)
{
	if (priority > DB_DBQUEUE_PRIORITY_HIGHEST)
		priority = DB_DBQUEUE_PRIORITY_HIGHEST;
	if (priority < DB_DBQUEUE_PRIORITY_LOWEST)
		priority = DB_DBQUEUE_PRIORITY_LOWEST;
	DB::DBQueue::SQLTrans *trans;
	NEW_CLASS(trans, DB::DBQueue::SQLTrans(progId, hdlr, userData, userData2));
	Sync::MutexUsage mutUsage(mut);
	sqlList[priority]->Add(trans);
	((DB::DBHandler*)this->dbList->GetItem(this->nextDB))->Wake();
	this->nextDB = (this->nextDB + 1) % this->dbList->GetCount();
	mutUsage.EndUse();
}

void DB::DBQueue::GetDB(Int32 priority, Int32 progId, DBToolHdlr hdlr, void *userData, void *userData2)
{
	if (priority > DB_DBQUEUE_PRIORITY_HIGHEST)
		priority = DB_DBQUEUE_PRIORITY_HIGHEST;
	if (priority < DB_DBQUEUE_PRIORITY_LOWEST)
		priority = DB_DBQUEUE_PRIORITY_LOWEST;
	DB::DBQueue::SQLGetDB *trans;
	NEW_CLASS(trans, DB::DBQueue::SQLGetDB(progId, hdlr, userData, userData2));
	Sync::MutexUsage mutUsage(mut);
	sqlList[priority]->Add(trans);
	((DB::DBHandler*)this->dbList->GetItem(this->nextDB))->Wake();
	this->nextDB = (this->nextDB + 1) % this->dbList->GetCount();
	mutUsage.EndUse();
}

void DB::DBQueue::RemoveSQLs(Int32 progId)
{
	UOSInt i = DB_DBQUEUE_PRIORITY_HIGHEST + 1;
	UOSInt j;
	DB::DBQueue::IDBCmd *cmd;
	Sync::MutexUsage mutUsage(mut);
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

UOSInt DB::DBQueue::GetDataCnt()
{
	UOSInt i = dbList->GetCount();
	UOSInt cnt = 0;
	while (i-- > 0)
	{
		cnt += ((DB::DBHandler*)dbList->GetItem(i))->GetDataCnt();
	}
	return cnt;
}

UOSInt DB::DBQueue::GetQueueCnt()
{
	UOSInt cnt = 0;
	UOSInt i = DB_DBQUEUE_PRIORITY_HIGHEST + 1;
	Sync::MutexUsage mutUsage(mut);
	while (i-- > 0)
	{
		cnt += sqlList2[i]->GetCount() * 200;
		cnt += sqlList[i]->GetCount();
	}
	mutUsage.EndUse();
	return cnt;
}

UOSInt DB::DBQueue::GetConnCnt()
{
	return this->dbList->GetCount();
}

UTF8Char *DB::DBQueue::ToString(UTF8Char *buff)
{
	return Text::StrConcat(buff, this->name);
}

DB::DBUtil::ServerType DB::DBQueue::GetSvrType()
{
	return this->db1->GetSvrType();
}

Int8 DB::DBQueue::GetTzQhr()
{
	return this->db1->GetTzQhr();
}

UOSInt DB::DBQueue::GetNextCmds(IDBCmd **cmds)
{
	Sync::MutexUsage mutUsage(this->mut);
	void **c;
	UOSInt i;
	UOSInt j;
	UOSInt cnt = 0;

	i = DB_DBQUEUE_PRIORITY_HIGHEST + 1;
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

UTF8Char *DB::DBQueue::DBDate(UTF8Char *buff, Data::DateTime *dat)
{
	return db1->DBDate(buff, dat);
}

UTF8Char *DB::DBQueue::DBInt32(UTF8Char *buff, Int32 val)
{
	return db1->DBInt32(buff, val);
}

UTF8Char *DB::DBQueue::DBInt64(UTF8Char *buff, Int64 val)
{
	return db1->DBInt64(buff, val);
}

UTF8Char *DB::DBQueue::DBStrW(UTF8Char *buff, const WChar *val)
{
	return db1->DBStrW(buff, val);
}

UTF8Char *DB::DBQueue::DBDbl(UTF8Char *buff, Double val)
{
	return db1->DBDbl(buff, val);
}

UTF8Char *DB::DBQueue::DBBool(UTF8Char *buff, Bool val)
{
	return db1->DBBool(buff, val);
}

Bool DB::DBQueue::IsExecTimeout()
{
	DB::DBHandler *db;
	UOSInt i;
	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	i = this->dbList->GetCount();
	while (i-- > 0)
	{
		db = ((DB::DBHandler *)dbList->GetItem(i));
		if (db->IsTimeout(&dt))
		{
			return true;
		}
	}
	return false;
}

DB::DBHandler::DBHandler(DB::DBQueue *dbQ, DB::DBTool *db)
{
	NEW_CLASS(this->evt, Sync::Event(true, (const UTF8Char*)"DB.DBHandler.evt"));
	NEW_CLASS(this->mut, Sync::Mutex());
	NEW_CLASS(this->procTime, Data::DateTime());
	this->processing = false;
	this->dbQ = dbQ;
	this->db = db;
	this->running = false;
	Sync::Thread::Create(ProcessSQL, this);
}

DB::DBHandler::~DBHandler()
{
	while (this->running)
	{
		Sync::Thread::Sleep(10);
	}
	DEL_CLASS(this->procTime);
	DEL_CLASS(this->db);
	DEL_CLASS(this->evt);
	DEL_CLASS(this->mut);
}

UInt32 DB::DBHandler::GetDataCnt()
{
	return db->GetDataCnt();
}

void DB::DBHandler::WriteError(const UTF8Char *errMsg, const UTF8Char *sqlCmd)
{
	this->dbQ->log->LogMessage((const UTF8Char*)"SQL: Failed", IO::ILogHandler::LOG_LEVEL_ERROR);
	Text::UTF8Writer *writer;
	IO::FileStream *fs;

	Sync::MutexUsage mutUsage(this->mut);
	NEW_CLASS(fs, IO::FileStream((const UTF8Char*)"FailSQL.txt", IO::FileStream::FileMode::Append, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	NEW_CLASS(writer, Text::UTF8Writer(fs));
	writer->Write(sqlCmd);
	writer->WriteLine((const UTF8Char*)";");
	DEL_CLASS(writer);
	DEL_CLASS(fs);
	mutUsage.EndUse();
}

UInt32 __stdcall DB::DBHandler::ProcessSQL(void *userObj)
{
	DB::DBHandler *me = (DB::DBHandler*)userObj;
	me->running = true;

	const UTF8Char *s;
	UOSInt i = 0;
	DB::DBQueue::SQLCmd *cmd;
	DB::DBQueue::SQLGroup *grp;
	DB::DBQueue::IDBCmd *cmds[200];
	UOSInt cmdSize;
	UOSInt l;
	DB::DBQueue::IDBCmd *c;
	OSInt sqlRet;
	bool found;
	while (!me->dbQ->stopping)
	{
		me->evt->Wait(1000);
		if (me->dbQ->stopping)
			break;
		found = true;
		while (found)
		{
			found = false;
			cmdSize = me->dbQ->GetNextCmds(cmds);
			if (cmdSize > 0)
			{
				me->procTime->SetCurrTimeUTC();
				me->processing = true;
				l = 0;
				while (l < cmdSize)
				{
					c = (DB::DBQueue::IDBCmd *)cmds[l];
					switch (c->GetCmdType())
					{
					case DB::DBQueue::CmdType::SQLCmd:
						me->procTime->SetCurrTimeUTC();
						cmd = (DB::DBQueue::SQLCmd*)c;
						s = cmd->str;
						if (cmd->hdlr == 0)
						{
							i = 3;
							while ((sqlRet = me->db->ExecuteNonQuery(s)) == -2)
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
							DB::DBReader *r = me->db->ExecuteReader(s);
							while (r == 0)
							{
								i -= 1;
								if (i <= 0)
								{
									me->WriteError((const UTF8Char*)"3 Times", s);
									break;
								}
								r = me->db->ExecuteReader(s);
							}
							if (r)
							{
								me->dbQ->sqlCnt += 1;
								cmd->hdlr(cmd->userData, cmd->userData2, me->db, r);
								me->db->CloseReader(r);
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
							me->procTime->SetCurrTimeUTC();
							me->db->BeginTrans();
							DB::DBQueue::SQLTrans *obj = (DB::DBQueue::SQLTrans*)c;
							res = obj->hdlr(obj->userData, obj->userData2, me->db);
							if (res == false)
							{
								me->dbQ->log->LogMessage((const UTF8Char*)"DB Trans failed", IO::ILogHandler::LOG_LEVEL_ERROR);
							}
							me->db->EndTrans(res);
							break;
						}
					case DB::DBQueue::CmdType::SQLGetDB:
						{
							Bool res = false;
							me->procTime->SetCurrTimeUTC();
							DB::DBQueue::SQLGetDB *obj = (DB::DBQueue::SQLGetDB*)c;
							res = obj->hdlr(obj->userData, obj->userData2, me->db);
							if (res == false)
							{
								me->dbQ->log->LogMessage((const UTF8Char*)"DB GetDB error", IO::ILogHandler::LOG_LEVEL_ERROR);
							}
							break;
						}
					case DB::DBQueue::CmdType::SQLGroup:
						{
							Bool hasError = false;
							grp = (DB::DBQueue::SQLGroup *)c;
							me->procTime->SetCurrTimeUTC();
							me->db->BeginTrans();
							UOSInt k;
							k = 0;
							if (grp->hdlr == 0)
							{
								i = 3;
								while (k < grp->strs->GetCount())
								{
									s = grp->strs->GetItem(k);
									if (me->db->ExecuteNonQuery(s) == -2)
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
								DB::DBReader *rdr = 0;
								i = 3;
								while (k < grp->strs->GetCount())
								{
									s = grp->strs->GetItem(k);
									rdr = me->db->ExecuteReader(s);
									if (rdr == 0)
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
										if (k < grp->strs->GetCount())
										{
											me->db->CloseReader(rdr);
										}
									}
								}
								
								me->dbQ->sqlCnt += k;
								if (rdr)
								{
									grp->hdlr(grp->userData, grp->userData2, me->db, rdr);
									me->db->CloseReader(rdr);
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
	evt->Set();
}

Bool DB::DBHandler::IsTimeout(Data::DateTime *currTime)
{
	if (!this->processing)
		return false;
	return currTime->DiffMS(this->procTime) > 60000;
}
