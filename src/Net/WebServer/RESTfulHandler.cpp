#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/DBReader.h"
#include "DB/DBTool.h"
#include "DB/SQLBuilder.h"
#include "Net/WebServer/RESTfulHandler.h"
#include "Text/JSONBuilder.h"

#include <stdio.h>

/*
{
  "_embedded" : {
    "user" : [ {
      "id" : 1,
      "status" : "ACTIVE",
      "lanAcc" : "simon",
      "displayName" : "Simon Wong",
      "email" : "abc@abc.com",
      "contactNo" : "88888888",
      "positionId" : 3,
      "photo" : "#",
      "rmks" : "",
      "dtCreate" : "2020-10-09T16:00:00.000+00:00",
      "dtModify" : "2020-10-09T16:00:00.000+00:00",
      "uidCreate" : 0,
      "uidModify" : 0,
      "roleIds" : [ ],
      "usergroupIds" : [ 64, 19, 20, 36, 5, 6, 56, 74, 43, 29, 31 ],
      "_links" : {
        "self" : {
          "href" : "https://127.0.0.1:8443/api/user/1"
        },
        "user" : {
          "href" : "https://127.0.0.1:8443/api/user/1{?projection}",
          "templated" : true
        }
      }
    }, {
      "id" : 3,
      "status" : "DELETED",
      "lanAcc" : "abc",
      "displayName" : "abc",
      "email" : "abc@abc.com",
      "contactNo" : "88888888",
      "positionId" : 11,
      "photo" : "#",
      "rmks" : "",
      "dtCreate" : "2020-10-09T16:00:00.000+00:00",
      "dtModify" : "2020-10-09T16:00:00.000+00:00",
      "uidCreate" : 0,
      "uidModify" : 0,
      "roleIds" : [ ],
      "usergroupIds" : [ 3 ],
      "_links" : {
        "self" : {
          "href" : "https://127.0.0.1:8443/api/user/3"
        },
        "user" : {
          "href" : "https://127.0.0.1:8443/api/user/3{?projection}",
          "templated" : true
        }
      }
    } ]
  },
  "_links" : {
    "first" : {
      "href" : "https://127.0.0.1:8443/api/user?page=0&size=20"
    },
    "self" : {
      "href" : "https://127.0.0.1:8443/api/user"
    },
    "next" : {
      "href" : "https://127.0.0.1:8443/api/user?page=1&size=20"
    },
    "last" : {
      "href" : "https://127.0.0.1:8443/api/user?page=1&size=20"
    },
    "profile" : {
      "href" : "https://127.0.0.1:8443/api/profile/user"
    },
    "search" : {
      "href" : "https://127.0.0.1:8443/api/user/search"
    }
  },
  "page" : {
    "size" : 20,
    "totalElements" : 40,
    "totalPages" : 2,
    "number" : 0
  }
}
*/

void Net::WebServer::RESTfulHandler::BuildJSON(Text::JSONBuilder *json, DB::DBRow *row)
{
	Text::StringBuilderUTF8 sb;
	DB::ColDef *col;
	Data::DateTime *dt;
	UTF8Char sbuff[64];
	DB::DBRow::DataType dtype;
	DB::TableDef *table = row->GetTableDef();
	UOSInt i = 0;
	UOSInt j = table->GetColCnt();
	while (i < j)
	{
		col = table->GetCol(i);
		dtype = row->GetFieldDataType(col->GetColName());
		sb.ClearStr();
		row->AppendVarNameForm(&sb, col->GetColName());
		switch (dtype)
		{
		case DB::DBRow::DT_STRING:
			json->ObjectAddStrUTF8(sb.ToString(), row->GetValueStr(col->GetColName()));
			break;
		case DB::DBRow::DT_DOUBLE:
			json->ObjectAddFloat64(sb.ToString(), row->GetValueDouble(col->GetColName()));
			break;
		case DB::DBRow::DT_INT64:
			json->ObjectAddInt64(sb.ToString(), row->GetValueInt64(col->GetColName()));
			break;
		case DB::DBRow::DT_DATETIME:
			dt = row->GetValueDate(col->GetColName());
			if (dt)
			{
				dt->ToString(sbuff, "yyyy-MM-ddTHH:mm:ss.fffzzz");
				json->ObjectAddStrUTF8(sb.ToString(), sbuff);
			}
			else
			{
				json->ObjectAddStrUTF8(sb.ToString(), 0);
			}
			break;
		case DB::DBRow::DT_VECTOR:
		case DB::DBRow::DT_BINARY:
		case DB::DBRow::DT_UNKNOWN:
			json->ObjectAddStrUTF8(sb.ToString(), (const UTF8Char*)"?");
			break;
		}
		i++;
	}
}

Net::WebServer::RESTfulHandler::RESTfulHandler(DB::DBCache *dbCache)
{
	this->dbCache = dbCache;
}

Net::WebServer::RESTfulHandler::~RESTfulHandler()
{
}

Bool Net::WebServer::RESTfulHandler::ProcessRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq)
{
	UTF8Char sbuff[256];
	if (this->DoRequest(req, resp, subReq))
	{
		return true;
	}

	if (req->GetReqMethod() == Net::WebServer::IWebRequest::REQMETH_HTTP_GET)
	{
		OSInt i = Text::StrIndexOf(&subReq[1], '/');
		if (i >= 0)
		{
			DB::DBRow *row;
			const UTF8Char *tableName;
			Int64 ikey;
			if (!Text::StrToInt64(&subReq[2 + i], &ikey))
			{
				resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
				return true;
			}
			tableName = Text::StrCopyNewC(&subReq[1], i);
			row = this->dbCache->GetTableItem(tableName, ikey);
			Text::StrDelNew(tableName);
			if (row == 0)
			{
				resp->SetStatusCode(Net::WebStatus::SC_NOT_FOUND);
				resp->AddDefHeaders(req);
				resp->AddCacheControl(0);
				resp->AddContentLength(0);
				return true;
			}

			Text::StringBuilderUTF8 sbURI;
			Text::StringBuilderUTF8 sb;
			{
				Text::JSONBuilder json(&sb, Text::JSONBuilder::OT_OBJECT);
				this->BuildJSON(&json, row);
				sbURI.ClearStr();
				req->GetRequestURLBase(&sbURI);
				req->GetRequestPath(sbuff, sizeof(sbuff));
				sbURI.Append(sbuff);
				json.ObjectBeginObject((const UTF8Char*)"_links");
				json.ObjectBeginObject((const UTF8Char*)"self");
				json.ObjectAddStrUTF8((const UTF8Char*)"href", sbURI.ToString());
				json.ObjectEnd();
				json.ObjectEnd();
				json.ObjectEnd();
			}
			this->dbCache->FreeTableItem(row);
			resp->AddDefHeaders(req);
			resp->AddCacheControl(0);
			resp->AddContentType((const UTF8Char*)"application/json");
			resp->AddContentLength(sb.GetLength());
			resp->Write(sb.ToString(), sb.GetLength());
			return true;
		}
		else
		{
			Text::StringBuilderUTF8 sbURI;
			Text::StringBuilderUTF8 sb;
			{
				Text::JSONBuilder json(&sb, Text::JSONBuilder::OT_OBJECT);
				Data::ArrayList<DB::DBRow*> rows;
				DB::DBRow *row;
				Int64 ikey;
				this->dbCache->GetTableData(&rows, &subReq[1]);
				json.ObjectBeginObject((const UTF8Char*)"_embedded");
				json.ObjectBeginArray(&subReq[1]);
				UOSInt i = 0;
				UOSInt j = rows.GetCount();
				while (i < j)
				{
					row = rows.GetItem(i);
					json.ArrayBeginObject();
					this->BuildJSON(&json, row);
					if (row->GetSinglePKI64(&ikey))
					{
						sbURI.ClearStr();
						req->GetRequestURLBase(&sbURI);
						req->GetRequestPath(sbuff, sizeof(sbuff));
						sbURI.Append(sbuff);
						if (!sbURI.EndsWith('/'))
						{
							sbURI.AppendChar('/', 1);
						}
						sbURI.AppendI64(ikey);
						json.ObjectBeginObject((const UTF8Char*)"_links");
						json.ObjectBeginObject((const UTF8Char*)"self");
						json.ObjectAddStrUTF8((const UTF8Char*)"href", sbURI.ToString());
						json.ObjectEnd();
						json.ObjectEnd();
					}
					json.ObjectEnd();
					i++;
				}
				this->dbCache->FreeTableData(&rows);
				json.ArrayEnd();
				json.ObjectEnd();
				json.ObjectEnd();
			}
			resp->AddDefHeaders(req);
			resp->AddCacheControl(0);
			resp->AddContentType((const UTF8Char*)"application/json");
			resp->AddContentLength(sb.GetLength());
			resp->Write(sb.ToString(), sb.GetLength());
			return true;
		}
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
