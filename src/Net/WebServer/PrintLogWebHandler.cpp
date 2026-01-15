#include "Stdafx.h"
#include "Net/WebServer/PrintLogWebHandler.h"
#include "Net/WebServer/PrintLogWebResponse.h"
#include "Text/StringBuilderUTF8.h"

Net::WebServer::PrintLogWebHandler::PrintLogWebHandler(NN<Net::WebServer::WebHandler> hdlr, NN<IO::Writer> writer)
{
	this->hdlr = hdlr;
	this->writer = writer;
}

Net::WebServer::PrintLogWebHandler::~PrintLogWebHandler()
{
	this->hdlr.Delete();
}

void Net::WebServer::PrintLogWebHandler::DoWebRequest(NN<WebRequest> req, NN<WebResponse> resp)
{
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrConcatC(Net::SocketUtil::GetAddrName(sbuff, req->GetClientAddr(), req->GetClientPort()).Or(sbuff), UTF8STRC(": "));
	Text::StringBuilderUTF8 sb;
	sb.AppendC(sbuff, (UIntOS)(sptr - sbuff));
	Text::CStringNN reqMeth = req->GetReqMethodStr();
	sb.AppendC(reqMeth.v, reqMeth.leng);
	sb.AppendUTF8Char(' ');
	sb.Append(req->GetRequestURI());
	sb.AppendUTF8Char(' ');
	sb.Append(Net::WebServer::WebRequest::RequestProtocolGetName(req->GetProtocol()));
	this->writer->WriteLine(sb.ToCString());
	Data::ArrayListStringNN headers;
	req->GetHeaderNames(headers);
	NN<Text::String> header;
	Data::ArrayIterator<NN<Text::String>> it = headers.Iterator();
	while (it.HasNext())
	{
		sb.ClearStr();
		sb.AppendC(sbuff, (UIntOS)(sptr - sbuff));
		header = it.Next();
		sb.Append(header);
		sb.AppendC(UTF8STRC(": "));
		req->GetHeaderC(sb, header->ToCString());
		this->writer->WriteLine(sb.ToCString());
	}
	this->writer->WriteLine(CSTRP(sbuff, sptr));
	Net::WebServer::PrintLogWebResponse plResp(resp, this->writer, CSTRP(sbuff, sptr));
	this->hdlr->DoWebRequest(req, plResp);
}
