#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/SharedDBConn.h"
#include "Sync/MutexUsage.h"

DB::SharedDBConn::~SharedDBConn()
{
	DEL_CLASS(this->conn);
	DEL_CLASS(this->mutConn);
	DEL_CLASS(this->mutObj);
}

DB::SharedDBConn::SharedDBConn(DB::DBConn *conn)
{
	this->conn = conn;
	NEW_CLASS(this->mutConn, Sync::Mutex());
	NEW_CLASS(this->mutObj, Sync::Mutex());
	this->useCnt = 1;
}

DB::DBUtil::ServerType DB::SharedDBConn::GetSvrType()
{
	return this->conn->GetSvrType();
}

void DB::SharedDBConn::Reconnect()
{
	Sync::MutexUsage mutUsage(this->mutConn);
	this->conn->Reconnect();
}

void DB::SharedDBConn::UseObject()
{
	Sync::MutexUsage mutUsage(this->mutObj);
	this->useCnt++;
}

void DB::SharedDBConn::UnuseObject()
{
	OSInt i;
	Sync::MutexUsage mutUsage(this->mutObj);
	i = --this->useCnt;
	mutUsage.EndUse();
	if (i <= 0)
	{
		DEL_CLASS(this);
	}
}

DB::DBConn *DB::SharedDBConn::UseConn(Sync::MutexUsage *mutUsage)
{
	mutUsage->ReplaceMutex(this->mutConn);
	return this->conn;
}
