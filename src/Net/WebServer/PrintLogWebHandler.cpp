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
	Text::StrConcat(Net::SocketUtil::GetAddrName(sbuff, req->GetClientAddr(), req->GetClientPort()), (const UTF8Char*)": ");
	Text::StringBuilderUTF8 sb;
	sb.Append(sbuff);
	sb.Append((const UTF8Char*)req->GetReqMethodStr());
	sb.AppendChar(' ', 1);
	sb.Append(req->GetRequestURI());
	sb.AppendChar(' ', 1);
	sb.Append((const UTF8Char*)Net::WebServer::IWebRequest::RequestProtocolGetName(req->GetProtocol()));
	this->writer->WriteLineC(sb.ToString(), sb.GetLength());
	Data::ArrayList<Text::String*> headers;
	req->GetHeaderNames(&headers);
	Text::String *header;
	UOSInt i = 0;
	UOSInt j = headers.GetCount();
	while (i < j)
	{
		sb.ClearStr();
		sb.Append(sbuff);
		header = headers.GetItem(i);
		sb.Append(header);
		sb.AppendC(UTF8STRC(": "));
		req->GetHeader(&sb, header->v);
		this->writer->WriteLineC(sb.ToString(), sb.GetLength());
		i++;
	}
	this->writer->WriteLine(sbuff);
	Net::WebServer::PrintLogWebResponse plResp(resp, this->writer, sbuff);
	this->hdlr->WebRequest(req, &plResp);
}

void Net::WebServer::PrintLogWebHandler::Release()
{
	this->hdlr->Release();
	DEL_CLASS(this);
}
