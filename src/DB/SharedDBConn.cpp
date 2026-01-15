#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/SharedDBConn.h"
#include "Sync/MutexUsage.h"

DB::SharedDBConn::~SharedDBConn()
{
	this->conn.Delete();
}

DB::SharedDBConn::SharedDBConn(NN<DB::DBConn> conn)
{
	this->conn = conn;
	this->useCnt = 1;
}

DB::SQLType DB::SharedDBConn::GetSQLType()
{
	return this->conn->GetSQLType();
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
	IntOS i;
	Sync::MutexUsage mutUsage(this->mutObj);
	i = --this->useCnt;
	mutUsage.EndUse();
	if (i <= 0)
	{
		DEL_CLASS(this);
	}
}

NN<DB::DBConn> DB::SharedDBConn::UseConn(NN<Sync::MutexUsage> mutUsage)
{
	mutUsage->ReplaceMutex(this->mutConn);
	return this->conn;
}
