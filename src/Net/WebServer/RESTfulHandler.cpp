#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/DBReader.h"
#include "DB/DBTool.h"
#include "DB/SQLBuilder.h"
#include "Net/WebServer/RESTfulHandler.h"

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

Net::WebServer::RESTfulHandler::RESTfulHandler(DB::DBCache *dbCache)
{
	this->dbCache = dbCache;
}

Net::WebServer::RESTfulHandler::~RESTfulHandler()
{
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
			resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
			return true;
		}
		else
		{
			Text::StringBuilderUTF8 sb;
			Data::ArrayList<DB::DBRow*> rows;
			DB::DBRow *row;
			this->dbCache->GetTableData(&rows, &subReq[1]);
			UOSInt i = 0;
			UOSInt j = rows.GetCount();
			while (i < j)
			{
				row = rows.GetItem(i);
				sb.ClearStr();
				row->ToString(&sb);
				printf("%s\r\n", sb.ToString());
				i++;
			}
			this->dbCache->FreeTableData(&rows);
			resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
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
