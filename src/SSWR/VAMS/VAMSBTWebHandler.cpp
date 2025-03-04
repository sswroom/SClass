#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "SSWR/VAMS/VAMSBTWebHandler.h"

Bool __stdcall SSWR::VAMS::VAMSBTWebHandler::DevData(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebServiceHandler> hdlr)
{
	NN<SSWR::VAMS::VAMSBTWebHandler> me = NN<SSWR::VAMS::VAMSBTWebHandler>::ConvertFrom(hdlr);
	req->ParseHTTPForm();
	NN<Text::String> avlNo;
	Int64 ts;
	Int32 progId;
	Int16 rssi;
	if (!req->GetHTTPFormStr(CSTR("avlNo")).SetTo(avlNo) || !req->GetHTTPFormInt64(CSTR("ts"), ts) || !req->GetHTTPFormInt32(CSTR("progId"), progId) || !req->GetHTTPFormInt16(CSTR("rssi"), rssi))
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
		return true;
	}

	me->btList->AddItem(avlNo, progId, ts, rssi);
	return me->ResponseJSONStr(req, resp, 0, CSTR("{\"status\":\"ok\"}"));
}

Bool __stdcall SSWR::VAMS::VAMSBTWebHandler::KAData(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> hdlr)
{
	NN<SSWR::VAMS::VAMSBTWebHandler> me = NN<SSWR::VAMS::VAMSBTWebHandler>::ConvertFrom(hdlr);
	req->ParseHTTPForm();
	Int32 progId;
	if (!req->GetHTTPFormInt32(CSTR("progId"), progId))
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
		return true;
	}
	me->btList->KARecv(progId);
	return me->ResponseJSONStr(req, resp, 0, CSTR("{\"status\":\"ok\"}"));
}

Bool __stdcall SSWR::VAMS::VAMSBTWebHandler::LogData(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> hdlr)
{
	NN<SSWR::VAMS::VAMSBTWebHandler> me = NN<SSWR::VAMS::VAMSBTWebHandler>::ConvertFrom(hdlr);
	Int32 progId;
	Int32 logDate;
	if (!req->GetQueryValueI32(CSTR("progId"), progId) || !req->GetQueryValueI32(CSTR("logDate"), logDate))
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
		return true;
	}
	if (!me->btList->HasProg(progId))
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
		return true;
	}
	if (logDate < 20000000 || logDate >= 21000000)
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
		return true;
	}
	UOSInt dataSize;
	UnsafeArray<const UInt8> reqData;
	if (req->GetReqData(dataSize).SetTo(reqData) && dataSize > 0)
	{
		UTF8Char sbuff[512];
		UnsafeArray<UTF8Char> sptr;
		sptr = me->logPath->ConcatTo(sbuff);
		if (sptr[-1] != IO::Path::PATH_SEPERATOR)
		{
			*sptr++ = IO::Path::PATH_SEPERATOR;
		}
		sptr = Text::StrInt32(sptr, logDate / 100);
		IO::Path::CreateDirectory(CSTRP(sbuff, sptr));
		*sptr++ = IO::Path::PATH_SEPERATOR;
		sptr = Text::StrConcatC(sptr, UTF8STRC("IPC"));
		sptr = Text::StrInt32(sptr, progId);
		*sptr++ = '_';
		sptr = Text::StrInt32(sptr, logDate);
		sptr = Text::StrConcatC(sptr, UTF8STRC(".log"));
		IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		fs.Write(Data::ByteArrayR(reqData, dataSize));
		return me->ResponseJSONStr(req, resp, 0, CSTR("{\"status\":\"ok\"}"));
	}
	else
	{
		resp->SetStatusCode(Net::WebStatus::SC_BAD_REQUEST);
		resp->AddDefHeaders(req);
		resp->AddCacheControl(0);
		resp->ResponseText(CSTR("File error"));
		return true;
	}
}

Bool __stdcall SSWR::VAMS::VAMSBTWebHandler::ListData(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> hdlr)
{
	NN<SSWR::VAMS::VAMSBTWebHandler> me = NN<SSWR::VAMS::VAMSBTWebHandler>::ConvertFrom(hdlr);
	Data::ArrayList<Int32> progList;
	me->btList->GetProgList(progList);
	Text::StringBuilderUTF8 sb;
	Data::DateTime dt;
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i;
	UOSInt j;
	Int32 progId;
	Int64 ts;
	sb.AppendC(UTF8STRC("<html><head><title>Prog List</title></head><body>\r\n"));
	sb.AppendC(UTF8STRC("<table border=\"1\">\r\n"));
	sb.AppendC(UTF8STRC("<tr><th>Prog Id</th><th>Last KA Time</th></tr>\r\n"));
	i = 0;
	j = progList.GetCount();
	while (i < j)
	{
		progId = progList.GetItem(i);
		sb.AppendC(UTF8STRC("<tr><td><a href=\"listitem?progId="));
		sb.AppendI32(progId);
		sb.AppendC(UTF8STRC("\">"));
		sb.AppendI32(progId);
		sb.AppendC(UTF8STRC("</a></td><td>"));
		ts = me->btList->GetLastKeepAlive(progId);
		if (ts)
		{
			dt.SetTicks(ts);
			dt.ToLocalTime();
			sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff zzz");
			sb.AppendP(sbuff, sptr);
		}
		else
		{
			sb.AppendUTF8Char('-');
		}
		sb.AppendC(UTF8STRC("</td></tr>"));
		i++;
	}
	sb.AppendC(UTF8STRC("</table></body></html>"));
	resp->AddDefHeaders(req);
	resp->AddCacheControl(0);
	resp->ResponseText(sb.ToCString(), CSTR("text/html"));
	return true;
}

Bool __stdcall SSWR::VAMS::VAMSBTWebHandler::ListItem(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> hdlr)
{
	NN<SSWR::VAMS::VAMSBTWebHandler> me = NN<SSWR::VAMS::VAMSBTWebHandler>::ConvertFrom(hdlr);
	Int32 progId;
	if (!req->GetQueryValueI32(CSTR("progId"), progId))
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
		return true;
	}
	Data::ArrayListNN<SSWR::VAMS::VAMSBTList::AvlBleItem> itemList;
	NN<SSWR::VAMS::VAMSBTList::AvlBleItem> item;
	me->btList->QueryByProgId(itemList, progId, 30000);
	Text::StringBuilderUTF8 sb;
	Data::DateTime dt;
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i;
	UOSInt j;
	sb.AppendC(UTF8STRC("<html><head><title>Item List</title></head><body>\r\n"));
	sb.AppendC(UTF8STRC("<h2>Prog Id: "));
	sb.AppendI32(progId);
	sb.AppendC(UTF8STRC("</h2><table border=\"1\">\r\n"));
	sb.AppendC(UTF8STRC("<tr><th>AVL No</th><th>Last Recv Time</th><th>RSSI</th></tr>\r\n"));
	i = 0;
	j = itemList.GetCount();
	while (i < j)
	{
		item = itemList.GetItemNoCheck(i);
		sb.AppendC(UTF8STRC("<tr><td>"));
		sb.Append(item->avlNo);
		sb.AppendC(UTF8STRC("</td><td>"));
		dt.SetTicks(item->lastDevTS);
		dt.ToLocalTime();
		sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff zzz");
		sb.AppendP(sbuff, sptr);
		sb.AppendC(UTF8STRC("</td><td>"));
		sb.AppendI16(item->rssi);
		sb.AppendC(UTF8STRC("</td></tr>"));
		i++;
	}
	sb.AppendC(UTF8STRC("</table><a href=\"listdata\">Back</a></body></html>"));
	resp->AddDefHeaders(req);
	resp->AddCacheControl(0);
	resp->ResponseText(sb.ToCString(), CSTR("text/html"));
	return true;
}

SSWR::VAMS::VAMSBTWebHandler::VAMSBTWebHandler(NN<Text::String> logPath, VAMSBTList *btList)
{
	this->logPath = logPath->Clone();
	this->btList = btList;

	this->AddService(CSTR("/devdata"), Net::WebUtil::RequestMethod::HTTP_POST, DevData);
	this->AddService(CSTR("/kadata"), Net::WebUtil::RequestMethod::HTTP_POST, KAData);
	this->AddService(CSTR("/logdata"), Net::WebUtil::RequestMethod::HTTP_POST, LogData);
	this->AddService(CSTR("/listdata"), Net::WebUtil::RequestMethod::HTTP_GET, ListData);
	this->AddService(CSTR("/listitem"), Net::WebUtil::RequestMethod::HTTP_GET, ListItem);
}

SSWR::VAMS::VAMSBTWebHandler::~VAMSBTWebHandler()
{
	this->logPath->Release();
	DEL_CLASS(this->btList);
}
