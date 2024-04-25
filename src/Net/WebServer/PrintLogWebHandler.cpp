#include "Stdafx.h"
#include "Net/WebServer/PrintLogWebHandler.h"
#include "Net/WebServer/PrintLogWebResponse.h"
#include "Text/StringBuilderUTF8.h"

Net::WebServer::PrintLogWebHandler::PrintLogWebHandler(NN<Net::WebServer::IWebHandler> hdlr, NN<IO::Writer> writer)
{
	this->hdlr = hdlr;
	this->writer = writer;
}

Net::WebServer::PrintLogWebHandler::~PrintLogWebHandler()
{
	this->hdlr.Delete();
}

void Net::WebServer::PrintLogWebHandler::WebRequest(NN<IWebRequest> req, NN<IWebResponse> resp)
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
	Data::ArrayListStringNN headers;
	req->GetHeaderNames(headers);
	NN<Text::String> header;
	Data::ArrayIterator<NN<Text::String>> it = headers.Iterator();
	while (it.HasNext())
	{
		sb.ClearStr();
		sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
		header = it.Next();
		sb.Append(header);
		sb.AppendC(UTF8STRC(": "));
		req->GetHeaderC(sb, header->ToCString());
		this->writer->WriteLineC(sb.ToString(), sb.GetLength());
	}
	this->writer->WriteLineC(sbuff, (UOSInt)(sptr - sbuff));
	Net::WebServer::PrintLogWebResponse plResp(resp, this->writer, CSTRP(sbuff, sptr));
	this->hdlr->WebRequest(req, plResp);
}
