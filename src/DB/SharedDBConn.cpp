#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/SharedDBConn.h"

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
	this->mutConn->Lock();
	this->conn->Reconnect();
	this->mutConn->Unlock();
}

void DB::SharedDBConn::UseObject()
{
	this->mutObj->Lock();
	this->useCnt++;
	this->mutObj->Unlock();
}

void DB::SharedDBConn::UnuseObject()
{
	OSInt i;
	this->mutObj->Lock();
	i = --this->useCnt;
	this->mutObj->Unlock();
	if (i <= 0)
	{
		DEL_CLASS(this);
	}
}

DB::DBConn *DB::SharedDBConn::BeginUseConn()
{
	this->mutConn->Lock();
	return this->conn;
}

void DB::SharedDBConn::EndUseConn()
{
	this->mutConn->Unlock();
}
