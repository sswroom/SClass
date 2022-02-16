#include "Stdafx.h"
#include "Net/WebServer/PrintLogWebHandler.h"
#include "Net/WebServer/PrintLogWebResponse.h"
#include "Text/StringBuilderUTF8.h"

Net::WebServer::PrintLogWebHandler::~PrintLogWebHandler()
{
}

Net::WebServer::PrintLogWebHandler::PrintLogWebHandler(Net::WebServer::IWebHandler *hdlr, IO::Writer *writer)
{
	this->hdlr = hdlr;
	this->writer = writer;
}

void Net::WebServer::PrintLogWebHandler::WebRequest(IWebRequest *req, IWebResponse *resp)
{
	UTF8Char sbuff[128];
	UTF8Char *sptr;
	sptr = Text::StrConcatC(Net::SocketUtil::GetAddrName(sbuff, req->GetClientAddr(), req->GetClientPort()), UTF8STRC(": "));
	Text::StringBuilderUTF8 sb;
	sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
	Text::CString reqMeth = req->GetReqMethodStr();
	sb.AppendC(reqMeth.v, reqMeth.leng);
	sb.AppendUTF8Char(' ');
	sb.Append(req->GetRequestURI());
	sb.AppendUTF8Char(' ');
	sb.Append(Net::WebServer::IWebRequest::RequestProtocolGetName(req->GetProtocol()));
	this->writer->WriteLineC(sb.ToString(), sb.GetLength());
	Data::ArrayList<Text::String*> headers;
	req->GetHeaderNames(&headers);
	Text::String *header;
	UOSInt i = 0;
	UOSInt j = headers.GetCount();
	while (i < j)
	{
		sb.ClearStr();
		sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
		header = headers.GetItem(i);
		sb.Append(header);
		sb.AppendC(UTF8STRC(": "));
		req->GetHeaderC(&sb, header->v, header->leng);
		this->writer->WriteLineC(sb.ToString(), sb.GetLength());
		i++;
	}
	this->writer->WriteLineC(sbuff, (UOSInt)(sptr - sbuff));
	Net::WebServer::PrintLogWebResponse plResp(resp, this->writer, sbuff);
	this->hdlr->WebRequest(req, &plResp);
}

void Net::WebServer::PrintLogWebHandler::Release()
{
	this->hdlr->Release();
	DEL_CLASS(this);
}
