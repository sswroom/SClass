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
				dt->ToString(sbuff, "yyyy-MM-ddTHH:mm:ss.fffzzzz");
				json->ObjectAddStrUTF8(sb.ToString(), sbuff);
			}
			else
			{
				json->ObjectAddStrUTF8(sb.ToString(), 0);
			}
			break;
		case DB::DBRow::DT_VECTOR:
			this->AppendVector(json, sb.ToString(), row->GetValueVector(col->GetColName()));
			break;
		case DB::DBRow::DT_BINARY:
		case DB::DBRow::DT_UNKNOWN:
			json->ObjectAddStrUTF8(sb.ToString(), (const UTF8Char*)"?");
			break;
		}
		i++;
	}
}

void Net::WebServer::RESTfulHandler::AppendVector(Text::JSONBuilder *json, const UTF8Char *name, Math::Vector2D *vec)
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
				json->ObjectAddStrUTF8((const UTF8Char*)"type", (const UTF8Char*)"Polygon");
			}
			else
			{
				json->ObjectAddStrUTF8((const UTF8Char*)"type", (const UTF8Char*)"Polyline");
			}
			json->ObjectBeginArray((const UTF8Char*)"coordinates");
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
			json->ObjectAddStrUTF8((const UTF8Char*)"type", (const UTF8Char*)"Point");
			json->ObjectBeginArray((const UTF8Char*)"coordinates");
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
		json->ObjectAddStrUTF8(name, (const UTF8Char*)"?");
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

Bool Net::WebServer::RESTfulHandler::ProcessRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq)
{
	UTF8Char sbuff[256];
	if (this->DoRequest(req, resp, subReq))
	{
		return true;
	}

	if (req->GetReqMethod() == Net::WebServer::IWebRequest::RequestMethod::HTTP_GET)
	{
		UOSInt i = Text::StrIndexOf(&subReq[1], '/');
		if (i != INVALID_INDEX)
		{
			DB::DBRow *row;
			const UTF8Char *tableName;
			Int64 ikey;
			if (!Text::StrToInt64(&subReq[2 + i], &ikey))
			{
				resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
				return true;
			}
			tableName = Text::StrCopyNewC(&subReq[1], (UOSInt)i);
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
					req->GetRequestPath(sbuff, sizeof(sbuff));
					sbURI.Append(sbuff);
					json.ObjectBeginObject((const UTF8Char*)"_links");
					json.ObjectBeginObject((const UTF8Char*)"self");
					json.ObjectAddStrUTF8((const UTF8Char*)"href", sbURI.ToString());
					json.ObjectEnd();
					json.ObjectEnd();
				}
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
			if (!this->dbCache->IsTableExist(&subReq[1]))
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
				this->dbCache->GetTableData(&rows, &subReq[1], page);
				json.ObjectBeginObject((const UTF8Char*)"_embedded");
				json.ObjectBeginArray(&subReq[1]);
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

				OSInt cnt = this->dbCache->GetRowCount(&subReq[1]);
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
					req->GetRequestPath(sbuff, sizeof(sbuff));
					json.ObjectBeginObject((const UTF8Char*)"_links");
					sbURI.ClearStr();
					req->GetRequestURLBase(&sbURI);
					sbURI.Append(sbuff);
					sbURI.Append((const UTF8Char*)"?page=0&size=");
					sbURI.AppendUOSInt(page->GetPageSize());
					json.ObjectBeginObject((const UTF8Char*)"first");
					json.ObjectAddStrUTF8((const UTF8Char*)"href", sbURI.ToString());
					json.ObjectEnd();
					sbURI.ClearStr();
					req->GetRequestURLBase(&sbURI);
					sbURI.Append(req->GetRequestURI());
					json.ObjectBeginObject((const UTF8Char*)"self");
					json.ObjectAddStrUTF8((const UTF8Char*)"href", sbURI.ToString());
					json.ObjectEnd();
					if (page->GetPageNum() + 1 < pageCnt)
					{
						sbURI.ClearStr();
						req->GetRequestURLBase(&sbURI);
						sbURI.Append(sbuff);
						sbURI.Append((const UTF8Char*)"?page=");
						sbURI.AppendUOSInt(page->GetPageNum() + 1);
						sbURI.Append((const UTF8Char*)"&size=");
						sbURI.AppendUOSInt(page->GetPageSize());
						json.ObjectBeginObject((const UTF8Char*)"next");
						json.ObjectAddStrUTF8((const UTF8Char*)"href", sbURI.ToString());
						json.ObjectEnd();
					}
					sbURI.ClearStr();
					req->GetRequestURLBase(&sbURI);
					sbURI.Append(sbuff);
					sbURI.Append((const UTF8Char*)"?page=");
					sbURI.AppendUOSInt(pageCnt - 1);
					sbURI.Append((const UTF8Char*)"&size=");
					sbURI.AppendUOSInt(page->GetPageSize());
					json.ObjectBeginObject((const UTF8Char*)"last");
					json.ObjectAddStrUTF8((const UTF8Char*)"href", sbURI.ToString());
					json.ObjectEnd();
					json.ObjectEnd();
				}

				json.ObjectBeginObject((const UTF8Char*)"page");
				json.ObjectAddUInt64((const UTF8Char*)"size", page->GetPageSize());
				json.ObjectAddInt64((const UTF8Char*)"totalElements", cnt);
				json.ObjectAddUInt64((const UTF8Char*)"totalPages", pageCnt);
				json.ObjectAddUInt64((const UTF8Char*)"number", page->GetPageNum());
				json.ObjectEnd();

				json.ObjectEnd();
				DEL_CLASS(page);
			}
			resp->AddDefHeaders(req);
			resp->AddCacheControl(0);
			resp->AddContentType((const UTF8Char*)"application/json");
			resp->AddContentLength(sb.GetLength());
			resp->Write(sb.ToString(), sb.GetLength());
			return true;
		}
	}
	else if (req->GetReqMethod() == Net::WebServer::IWebRequest::RequestMethod::HTTP_POST)
	{

	}
	else if (req->GetReqMethod() == Net::WebServer::IWebRequest::RequestMethod::HTTP_PUT)
	{

	}
	else if (req->GetReqMethod() == Net::WebServer::IWebRequest::RequestMethod::HTTP_PATCH)
	{

	}
	else if (req->GetReqMethod() == Net::WebServer::IWebRequest::RequestMethod::HTTP_DELETE)
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
	req->GetQueryValueU32((const UTF8Char*)"page", &pageNum);
	req->GetQueryValueU32((const UTF8Char*)"size", &pageSize);
	if (pageSize <= 0)
	{
		pageSize = 20;
	}
	DB::PageRequest *page;
	NEW_CLASS(page, DB::PageRequest(pageNum, pageSize));
	const UTF8Char *sort = req->GetQueryValue((const UTF8Char *)"sort");
	if (sort)
	{
		Text::StringBuilderUTF8 sb;
		sb.Append(sort);
		UTF8Char *sarr[2];
		UOSInt i;
		UOSInt j;
		Bool desc;
		sarr[1] = sb.ToString();
		while (true)
		{
			i = Text::StrSplit(sarr, 2, sarr[1], Net::WebServer::IWebRequest::PARAM_SEPERATOR);
			j = Text::StrIndexOf(sarr[0], ',');
			desc = false;
			if (j != INVALID_INDEX)
			{
				sarr[0][j] = 0;
				desc = Text::StrEqualsICase(&sarr[0][j + 1], (const UTF8Char*)"DESC");
			}
			page->Sort(sarr[0], desc);
			if (i != 2)
			{
				break;
			}
		}
	}
	return page;
}
