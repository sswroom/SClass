#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/DBReader.h"
#include "DB/DBTool.h"
#include "DB/SQLBuilder.h"
#include "Math/Point.h"
#include "Math/Polygon.h"
#include "Net/WebServer/RESTfulHandler.h"
#include "Text/JSONBuilder.h"

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
		dtype = row->GetFieldDataType(col->GetColName()->v);
		sb.ClearStr();
		row->AppendVarNameForm(&sb, col->GetColName()->v);
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
			dt = row->GetValueDate(col->GetColName()->v);
			if (dt)
			{
				dt->ToString(sbuff, "yyyy-MM-ddTHH:mm:ss.fffzzzz");
				json->ObjectAddStrUTF8(sb.ToCString(), sbuff);
			}
			else
			{
				json->ObjectAddStrUTF8(sb.ToCString(), 0);
			}
			break;
		case DB::DBRow::DT_VECTOR:
			this->AppendVector(json, sb.ToCString(), row->GetValueVector(col->GetColName()->v));
			break;
		case DB::DBRow::DT_BINARY:
		case DB::DBRow::DT_UNKNOWN:
			json->ObjectAddStr(sb.ToCString(), CSTR("?"));
			break;
		}
		i++;
	}
}

void Net::WebServer::RESTfulHandler::AppendVector(Text::JSONBuilder *json, Text::CString name, Math::Vector2D *vec)
{
	switch (vec->GetVectorType())
	{
	case Math::Vector2D::VectorType::Polygon:
	case Math::Vector2D::VectorType::Polyline:
		{
			UOSInt nPtOfst;
			UInt32 *ptOfsts;
			UOSInt nPoint;
			Double *points;
			UOSInt i;
			UOSInt j;
			UOSInt k;
			Math::PointCollection *pg = (Math::PointCollection*)vec;
			json->ObjectBeginObject(name);
			if (vec->GetVectorType() == Math::Vector2D::VectorType::Polygon)
			{
				json->ObjectAddStr(CSTR("type"), CSTR("Polygon"));
			}
			else
			{
				json->ObjectAddStr(CSTR("type"), CSTR("Polyline"));
			}
			json->ObjectBeginArray(CSTR("coordinates"));
			ptOfsts = pg->GetPtOfstList(&nPtOfst);
			points = pg->GetPointList(&nPoint);
			j = ptOfsts[0];
			i = 0;
			while (i < nPtOfst)
			{
				i++;
				if (i >= nPtOfst)
				{
					k = nPoint;
				}
				else
				{
					k = ptOfsts[i];
				}
				json->ArrayBeginArray();
				while (j < k)
				{
					json->ArrayBeginArray();
					json->ArrayAddFloat64(points[j << 1]);
					json->ArrayAddFloat64(points[(j << 1) + 1]);
					json->ArrayEnd();
					j++;
				}
				json->ArrayEnd();
			}
			json->ArrayEnd();
			json->ObjectEnd();
		}
		break;
	case Math::Vector2D::VectorType::Point:
		{
			Double x;
			Double y;
			Math::Point *pt = (Math::Point*)vec;
			json->ObjectBeginObject(name);
			json->ObjectAddStr(CSTR("type"), CSTR("Point"));
			json->ObjectBeginArray(CSTR("coordinates"));
			pt->GetCenter(&x, &y);
			json->ArrayAddFloat64(x);
			json->ArrayAddFloat64(y);
			json->ArrayEnd();
			json->ObjectEnd();
		}
		break;
	case Math::Vector2D::VectorType::Multipoint:
	case Math::Vector2D::VectorType::Image:
	case Math::Vector2D::VectorType::String:
	case Math::Vector2D::VectorType::Ellipse:
	case Math::Vector2D::VectorType::PieArea:
	case Math::Vector2D::VectorType::Unknown:
	default:
		json->ObjectAddStr(name, CSTR("?"));
		break;
	}
}

Net::WebServer::RESTfulHandler::RESTfulHandler(DB::DBCache *dbCache)
{
	this->dbCache = dbCache;
	this->noLinks = false;
}

Net::WebServer::RESTfulHandler::~RESTfulHandler()
{
}

Bool Net::WebServer::RESTfulHandler::ProcessRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq)
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
			DB::DBRow *row;
			const UTF8Char *tableName;
			Int64 ikey;
			if (!Text::StrToInt64(&subReq.v[1 + i], &ikey))
			{
				resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
				return true;
			}
			tableName = Text::StrCopyNewC(subReq.v, (UOSInt)i);
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
				if (!this->noLinks)
				{
					sbURI.ClearStr();
					req->GetRequestURLBase(&sbURI);
					sptr = req->GetRequestPath(sbuff, sizeof(sbuff));
					sbURI.AppendP(sbuff, sptr);
					json.ObjectBeginObject(CSTR("_links"));
					json.ObjectBeginObject(CSTR("self"));
					json.ObjectAddStr(CSTR("href"), sbURI.ToCString());
					json.ObjectEnd();
					json.ObjectEnd();
				}
				json.ObjectEnd();
			}
			this->dbCache->FreeTableItem(row);
			resp->AddDefHeaders(req);
			resp->AddCacheControl(0);
			resp->AddContentType(CSTR("application/json"));
			resp->AddContentLength(sb.GetLength());
			resp->Write(sb.ToString(), sb.GetLength());
			return true;
		}
		else
		{
			Text::StringBuilderUTF8 sbURI;
			Text::StringBuilderUTF8 sb;
			if (!this->dbCache->IsTableExist(subReq.v))
			{
				resp->SetStatusCode(Net::WebStatus::SC_NOT_FOUND);
				resp->AddDefHeaders(req);
				resp->AddCacheControl(0);
				resp->AddContentLength(0);
				return true;
			}
			else
			{
				DB::PageRequest *page = ParsePageReq(req);
				Text::JSONBuilder json(&sb, Text::JSONBuilder::OT_OBJECT);
				Data::ArrayList<DB::DBRow*> rows;
				DB::DBRow *row;
				Int64 ikey;
				this->dbCache->GetTableData(&rows, subReq.v, page);
				json.ObjectBeginObject(CSTR("_embedded"));
				json.ObjectBeginArray(subReq);
				UOSInt i = 0;
				UOSInt j = rows.GetCount();
				while (i < j)
				{
					row = rows.GetItem(i);
					json.ArrayBeginObject();
					this->BuildJSON(&json, row);
					if (!this->noLinks && row->GetSinglePKI64(&ikey))
					{
						sbURI.ClearStr();
						req->GetRequestURLBase(&sbURI);
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
					i++;
				}
				this->dbCache->FreeTableData(&rows);
				json.ArrayEnd();
				json.ObjectEnd();

				OSInt cnt = this->dbCache->GetRowCount(subReq.v);
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
					req->GetRequestURLBase(&sbURI);
					sbURI.AppendP(sbuff, sptr);
					sbURI.AppendC(UTF8STRC("?page=0&size="));
					sbURI.AppendUOSInt(page->GetPageSize());
					json.ObjectBeginObject(CSTR("first"));
					json.ObjectAddStr(CSTR("href"), sbURI.ToCString());
					json.ObjectEnd();
					sbURI.ClearStr();
					req->GetRequestURLBase(&sbURI);
					sbURI.Append(req->GetRequestURI());
					json.ObjectBeginObject(CSTR("self"));
					json.ObjectAddStr(CSTR("href"), sbURI.ToCString());
					json.ObjectEnd();
					if (page->GetPageNum() + 1 < pageCnt)
					{
						sbURI.ClearStr();
						req->GetRequestURLBase(&sbURI);
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
					req->GetRequestURLBase(&sbURI);
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
			}
			resp->AddDefHeaders(req);
			resp->AddCacheControl(0);
			resp->AddContentType(CSTR("application/json"));
			resp->AddContentLength(sb.GetLength());
			resp->Write(sb.ToString(), sb.GetLength());
			return true;
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

DB::PageRequest *Net::WebServer::RESTfulHandler::ParsePageReq(Net::WebServer::IWebRequest *req)
{
	UInt32 pageNum = 0;
	UInt32 pageSize = 20;
	req->GetQueryValueU32(CSTR("page"), &pageNum);
	req->GetQueryValueU32(CSTR("size"), &pageSize);
	if (pageSize <= 0)
	{
		pageSize = 20;
	}
	DB::PageRequest *page;
	NEW_CLASS(page, DB::PageRequest(pageNum, pageSize));
	Text::String *sort = req->GetQueryValue(CSTR("sort"));
	if (sort)
	{
		Text::StringBuilderUTF8 sb;
		sb.Append(sort);
		Text::PString sarr[2];
		UOSInt i;
		UOSInt j;
		Bool desc;
		sarr[1].v = sb.ToString();
		sarr[1].leng = sb.GetLength();
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
