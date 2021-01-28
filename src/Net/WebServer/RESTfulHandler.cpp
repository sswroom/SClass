#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/DBReader.h"
#include "DB/DBTool.h"
#include "DB/SQLBuilder.h"
#include "Net/WebServer/RESTfulHandler.h"

#include <stdio.h>

Net::WebServer::RESTfulHandler::RESTfulHandler(DB::DBConn *conn, IO::LogTool *log)
{
	this->conn = conn;
	this->log = log;
	NEW_CLASS(this->tables, Data::StringUTF8Map<Net::WebServer::RESTfulHandler::TableInfo*>());
	NEW_CLASS(this->db, DB::DBTool(this->conn, false, log, false, (const UTF8Char*)"REST: "));

	Data::ArrayList<const UTF8Char *> tableNames;
	Net::WebServer::RESTfulHandler::TableInfo *table;
	DB::SQLBuilder sql(this->db->GetSvrType());
	const UTF8Char *tableName;
	DB::TableDef *def;
	DB::DBReader *r;
	this->conn->GetTableNames(&tableNames);
	UOSInt i = 0;
	UOSInt j = tableNames.GetCount();
	while (i < j)
	{
		tableName = tableNames.GetItem(i);
		def = this->db->GetTableDef(tableName);
		if (def)
		{
			table = MemAlloc(Net::WebServer::RESTfulHandler::TableInfo, 1);
			table->name = Text::StrCopyNew(tableName);
			table->def = def;
			table->rowCnt = 0;

			sql.Clear();
			sql.AppendCmd((const UTF8Char*)"select count(*) from ");
			sql.AppendCol(table->name);
			r = this->db->ExecuteReader(sql.ToString());
			if (r)
			{
				if (r->ReadNext())
				{
					table->rowCnt = r->GetInt32(0);
				}
				this->db->CloseReader(r);
			}
			this->tables->Put(table->name, table);
		}
		i++;
	}
}

Net::WebServer::RESTfulHandler::~RESTfulHandler()
{
	Data::ArrayList<Net::WebServer::RESTfulHandler::TableInfo*> *tableList = this->tables->GetValues();
	Net::WebServer::RESTfulHandler::TableInfo *table;
	UOSInt i = tableList->GetCount();
	while (i-- > 0)
	{
		table = tableList->GetItem(i);
		DEL_CLASS(table->def);
		Text::StrDelNew(table->name);
		MemFree(table);
	}
	DEL_CLASS(this->tables);
	DEL_CLASS(this->db);
}

Bool Net::WebServer::RESTfulHandler::ProcessRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq)
{
	if (this->DoRequest(req, resp, subReq))
	{
		return true;
	}

	if (req->GetReqMethod() == Net::WebServer::IWebRequest::REQMETH_HTTP_GET)
	{
		OSInt i = Text::StrIndexOf(&subReq[1], '/');
		if (i >= 0)
		{
			/////////////////////////
		}
		else
		{
			/////////////////////////
		}
		printf("Subreq = %s\r\n", subReq);
	}
	else if (req->GetReqMethod() == Net::WebServer::IWebRequest::REQMETH_HTTP_POST)
	{

	}
	else if (req->GetReqMethod() == Net::WebServer::IWebRequest::REQMETH_HTTP_PUT)
	{

	}
	else if (req->GetReqMethod() == Net::WebServer::IWebRequest::REQMETH_HTTP_PATCH)
	{

	}
	else if (req->GetReqMethod() == Net::WebServer::IWebRequest::REQMETH_HTTP_DELETE)
	{

	}
	else
	{
		return false;
	}
	return false;
}

void Net::WebServer::RESTfulHandler::GetTableNames(Data::ArrayList<const UTF8Char*> *tableNames)
{
	tableNames->AddRange(this->tables->GetKeys());
}

OSInt Net::WebServer::RESTfulHandler::GetRowCount(const UTF8Char *tableName)
{
	Net::WebServer::RESTfulHandler::TableInfo *table = this->tables->Get(tableName);
	if (table == 0)
		return -1;
	return table->rowCnt;
};
