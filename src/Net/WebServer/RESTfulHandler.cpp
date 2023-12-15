#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/DBReader.h"
#include "DB/DBTool.h"
#include "DB/SQLBuilder.h"
#include "Math/GeoJSONWriter.h"
#include "Math/Geometry/Point.h"
#include "Math/Geometry/Polygon.h"
#include "Net/WebServer/RESTfulHandler.h"
#include "Text/JSONBuilder.h"

void Net::WebServer::RESTfulHandler::BuildJSON(NotNullPtr<Text::JSONBuilder> json, NotNullPtr<DB::DBRow> row)
{
	Text::StringBuilderUTF8 sb;
	NotNullPtr<DB::ColDef> col;
	Data::Timestamp ts;
	UTF8Char sbuff[64];
	DB::DBRow::DataType dtype;
	NotNullPtr<Math::Geometry::Vector2D> vec;
	DB::TableDef *table = row->GetTableDef();
	Data::ArrayIterator<NotNullPtr<DB::ColDef>> it = table->ColIterator();
	while (it.HasNext())
	{
		col = it.Next();
		dtype = row->GetFieldDataType(col->GetColName()->v);
		sb.ClearStr();
		row->AppendVarNameForm(sb, col->GetColName()->v);
		switch (dtype)
		{
		case DB::DBRow::DT_STRING:
			json->ObjectAddStrUTF8(sb.ToCString(), row->GetValueStr(col->GetColName()->v));
			break;
		case DB::DBRow::DT_DOUBLE:
			json->ObjectAddFloat64(sb.ToCString(), row->GetValueDouble(col->GetColName()->v));
			break;
		case DB::DBRow::DT_INT64:
			json->ObjectAddInt64(sb.ToCString(), row->GetValueInt64(col->GetColName()->v));
			break;
		case DB::DBRow::DT_DATETIME:
			ts = row->GetValueDate(col->GetColName()->v);
			if (!ts.IsNull())
			{
				ts.ToString(sbuff, "yyyy-MM-ddTHH:mm:ss.fffzzzz");
				json->ObjectAddStrUTF8(sb.ToCString(), sbuff);
			}
			else
			{
				json->ObjectAddStrUTF8(sb.ToCString(), 0);
			}
			break;
		case DB::DBRow::DT_VECTOR:
			if (vec.Set(row->GetValueVector(col->GetColName()->v)))
				this->AppendVector(json, sb.ToCString(), vec);
			else
				json->ObjectAddNull(sb.ToCString());
			break;
		case DB::DBRow::DT_BINARY:
		case DB::DBRow::DT_UNKNOWN:
			json->ObjectAddStr(sb.ToCString(), CSTR("?"));
			break;
		}
	}
}

void Net::WebServer::RESTfulHandler::AppendVector(NotNullPtr<Text::JSONBuilder> json, Text::CStringNN name, NotNullPtr<Math::Geometry::Vector2D> vec)
{
	Math::GeoJSONWriter writer;
	json->ObjectBeginObject(name);
	if (!writer.ToGeometry(json, vec))
	{
		json->ObjectAddStr(CSTR("UnsupportedType"), Math::Geometry::Vector2D::VectorTypeGetName(vec->GetVectorType()));
	}
	json->ObjectEnd();
}

Net::WebServer::RESTfulHandler::RESTfulHandler(DB::DBCache *dbCache)
{
	this->dbCache = dbCache;
	this->noLinks = false;
}

Net::WebServer::RESTfulHandler::~RESTfulHandler()
{
}

Bool Net::WebServer::RESTfulHandler::ProcessRequest(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	if (this->DoRequest(req, resp, subReq))
	{
		return true;
	}

	if (req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_GET)
	{
		subReq = subReq.Substring(1);
		UOSInt i = subReq.IndexOf('/');
		if (i != INVALID_INDEX)
		{
			NotNullPtr<DB::DBRow> row;
			NotNullPtr<Text::String> tableName;
			Int64 ikey;
			if (!Text::StrToInt64(&subReq.v[1 + i], ikey))
			{
				resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
				return true;
			}
			tableName = Text::String::New(subReq.v, (UOSInt)i);
			if (!row.Set(this->dbCache->GetTableItem(tableName->ToCString(), ikey)))
			{
				tableName->Release();
				resp->SetStatusCode(Net::WebStatus::SC_NOT_FOUND);
				this->AddResponseHeaders(req, resp);
				resp->AddCacheControl(0);
				resp->AddContentLength(0);
				return true;
			}
			tableName->Release();

			Text::StringBuilderUTF8 sbURI;
			Text::JSONBuilder json(Text::JSONBuilder::OT_OBJECT);
			this->BuildJSON(json, row);
			if (!this->noLinks)
			{
				sbURI.ClearStr();
				req->GetRequestURLBase(sbURI);
				sptr = req->GetRequestPath(sbuff, sizeof(sbuff));
				sbURI.AppendP(sbuff, sptr);
				json.ObjectBeginObject(CSTR("_links"));
				json.ObjectBeginObject(CSTR("self"));
				json.ObjectAddStr(CSTR("href"), sbURI.ToCString());
				json.ObjectEnd();
				json.ObjectEnd();
			}
			json.ObjectEnd();
			this->dbCache->FreeTableItem(row);
			this->ResponseJSONStr(req, resp, 0, json.Build());
			return true;
		}
		else
		{
			if (!this->dbCache->IsTableExist(subReq))
			{
				resp->SetStatusCode(Net::WebStatus::SC_NOT_FOUND);
				this->AddResponseHeaders(req, resp);
				resp->AddCacheControl(0);
				resp->AddContentLength(0);
				return true;
			}
			else
			{
				Text::StringBuilderUTF8 sbURI;
				Text::StringBuilderUTF8 sb;
				DB::PageRequest *page = ParsePageReq(req);
				Text::JSONBuilder json(Text::JSONBuilder::OT_OBJECT);
				Data::ArrayListNN<DB::DBRow> rows;
				NotNullPtr<DB::DBRow> row;
				Int64 ikey;
				this->dbCache->QueryTableData(rows, subReq, page);
				json.ObjectBeginObject(CSTR("_embedded"));
				json.ObjectBeginArray(subReq);
				Data::ArrayIterator<NotNullPtr<DB::DBRow>> it = rows.Iterator();
				while (it.HasNext())
				{
					row = it.Next();
					json.ArrayBeginObject();
					this->BuildJSON(json, row);
					if (!this->noLinks && row->GetSinglePKI64(&ikey))
					{
						sbURI.ClearStr();
						req->GetRequestURLBase(sbURI);
						sptr = req->GetRequestPath(sbuff, sizeof(sbuff));
						sbURI.AppendP(sbuff, sptr);
						if (!sbURI.EndsWith('/'))
						{
							sbURI.AppendUTF8Char('/');
						}
						sbURI.AppendI64(ikey);
						json.ObjectBeginObject(CSTR("_links"));
						json.ObjectBeginObject(CSTR("self"));
						json.ObjectAddStr(CSTR("href"), sbURI.ToCString());
						json.ObjectEnd();
						json.ObjectEnd();
					}
					json.ObjectEnd();
				}
				this->dbCache->FreeTableData(rows);
				json.ArrayEnd();
				json.ObjectEnd();

				OSInt cnt = this->dbCache->GetRowCount(subReq);
				if (cnt < 0)
				{
					cnt = 0;
				}
				UOSInt pageCnt = (UOSInt)cnt / page->GetPageSize();
				if (pageCnt * page->GetPageSize() < (UOSInt)cnt)
				{
					pageCnt++;
				}

				if (!this->noLinks)
				{
					sptr = req->GetRequestPath(sbuff, sizeof(sbuff));
					json.ObjectBeginObject(CSTR("_links"));
					sbURI.ClearStr();
					req->GetRequestURLBase(sbURI);
					sbURI.AppendP(sbuff, sptr);
					sbURI.AppendC(UTF8STRC("?page=0&size="));
					sbURI.AppendUOSInt(page->GetPageSize());
					json.ObjectBeginObject(CSTR("first"));
					json.ObjectAddStr(CSTR("href"), sbURI.ToCString());
					json.ObjectEnd();
					sbURI.ClearStr();
					req->GetRequestURLBase(sbURI);
					sbURI.Append(req->GetRequestURI());
					json.ObjectBeginObject(CSTR("self"));
					json.ObjectAddStr(CSTR("href"), sbURI.ToCString());
					json.ObjectEnd();
					if (page->GetPageNum() + 1 < pageCnt)
					{
						sbURI.ClearStr();
						req->GetRequestURLBase(sbURI);
						sbURI.AppendP(sbuff, sptr);
						sbURI.AppendC(UTF8STRC("?page="));
						sbURI.AppendUOSInt(page->GetPageNum() + 1);
						sbURI.AppendC(UTF8STRC("&size="));
						sbURI.AppendUOSInt(page->GetPageSize());
						json.ObjectBeginObject(CSTR("next"));
						json.ObjectAddStr(CSTR("href"), sbURI.ToCString());
						json.ObjectEnd();
					}
					sbURI.ClearStr();
					req->GetRequestURLBase(sbURI);
					sbURI.AppendP(sbuff, sptr);
					sbURI.AppendC(UTF8STRC("?page="));
					sbURI.AppendUOSInt(pageCnt - 1);
					sbURI.AppendC(UTF8STRC("&size="));
					sbURI.AppendUOSInt(page->GetPageSize());
					json.ObjectBeginObject(CSTR("last"));
					json.ObjectAddStr(CSTR("href"), sbURI.ToCString());
					json.ObjectEnd();
					json.ObjectEnd();
				}

				json.ObjectBeginObject(CSTR("page"));
				json.ObjectAddUInt64(CSTR("size"), page->GetPageSize());
				json.ObjectAddInt64(CSTR("totalElements"), cnt);
				json.ObjectAddUInt64(CSTR("totalPages"), pageCnt);
				json.ObjectAddUInt64(CSTR("number"), page->GetPageNum());
				json.ObjectEnd();

				json.ObjectEnd();
				DEL_CLASS(page);
				return this->ResponseJSONStr(req, resp, 0, json.Build());
			}
		}
	}
	else if (req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_POST)
	{

	}
	else if (req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_PUT)
	{

	}
	else if (req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_PATCH)
	{

	}
	else if (req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_DELETE)
	{

	}
	else
	{
		return false;
	}
	return false;
}

DB::PageRequest *Net::WebServer::RESTfulHandler::ParsePageReq(NotNullPtr<Net::WebServer::IWebRequest> req)
{
	UInt32 pageNum = 0;
	UInt32 pageSize = 20;
	req->GetQueryValueU32(CSTR("page"), pageNum);
	req->GetQueryValueU32(CSTR("size"), pageSize);
	if (pageSize <= 0)
	{
		pageSize = 20;
	}
	DB::PageRequest *page;
	NEW_CLASS(page, DB::PageRequest(pageNum, pageSize));
	NotNullPtr<Text::String> sort;
	if (req->GetQueryValue(CSTR("sort")).SetTo(sort))
	{
		Text::StringBuilderUTF8 sb;
		sb.Append(sort);
		Text::PString sarr[2];
		UOSInt i;
		UOSInt j;
		Bool desc;
		sarr[1] = sb;
		while (true)
		{
			i = Text::StrSplitP(sarr, 2, sarr[1], Net::WebServer::IWebRequest::PARAM_SEPERATOR);
			j = Text::StrIndexOfChar(sarr[0].v, ',');
			desc = false;
			if (j != INVALID_INDEX)
			{
				sarr[0].v[j] = 0;
				desc = Text::StrEqualsICaseC(&sarr[0].v[j + 1], sarr[0].leng - j - 1, UTF8STRC("DESC"));
			}
			page->Sort(sarr[0].v, desc);
			if (i != 2)
			{
				break;
			}
		}
	}
	return page;
}
