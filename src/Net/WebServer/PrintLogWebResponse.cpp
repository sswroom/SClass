#include "Stdafx.h"
#include "Net/WebServer/PrintLogWebResponse.h"
#include "Text/StringBuilderUTF8.h"

Net::WebServer::PrintLogWebResponse::PrintLogWebResponse(NN<Net::WebServer::IWebResponse> resp, NN<IO::Writer> writer, Text::CString prefix) : Net::WebServer::IWebResponse(resp->GetSourceNameObj())
{
	this->resp = resp;
	this->writer = writer;
	this->prefix = Text::String::NewOrNull(prefix);
}

Net::WebServer::PrintLogWebResponse::~PrintLogWebResponse()
{
	OPTSTR_DEL(this->prefix);
}

void Net::WebServer::PrintLogWebResponse::EnableWriteBuffer()
{
	this->resp->EnableWriteBuffer();
}

Bool Net::WebServer::PrintLogWebResponse::SetStatusCode(Net::WebStatus::StatusCode code)
{
	if (this->resp->SetStatusCode(code))
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendOpt(this->prefix);
		sb.AppendC(UTF8STRC("Status Code = "));
		sb.AppendI32(code);
		this->writer->WriteLineC(sb.ToString(), sb.GetLength());
		return true;
	}
	return false;
}

Int32 Net::WebServer::PrintLogWebResponse::GetStatusCode()
{
	return this->resp->GetStatusCode();
}

Bool Net::WebServer::PrintLogWebResponse::AddHeader(Text::CStringNN name, Text::CStringNN value)
{
	if (this->resp->AddHeader(name, value))
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendOpt(this->prefix);
		sb.Append(name);
		sb.AppendC(UTF8STRC(": "));
		sb.Append(value);
		this->writer->WriteLineC(sb.ToString(), sb.GetLength());
		return true;
	}
	return false;
}

Bool Net::WebServer::PrintLogWebResponse::AddDefHeaders(NN<Net::WebServer::IWebRequest> req)
{
	return this->resp->AddDefHeaders(req);
}

UInt64 Net::WebServer::PrintLogWebResponse::GetRespLength()
{
	return this->resp->GetRespLength();
}

void Net::WebServer::PrintLogWebResponse::ShutdownSend()
{
	this->resp->ShutdownSend();
}

Bool Net::WebServer::PrintLogWebResponse::ResponseSSE(Data::Duration timeout, SSEDisconnectHandler hdlr, AnyType userObj)
{
	return this->resp->ResponseSSE(timeout, hdlr, userObj);
}

Bool Net::WebServer::PrintLogWebResponse::SSESend(const UTF8Char *eventName, const UTF8Char *data)
{
	return this->resp->SSESend(eventName, data);
}

Bool Net::WebServer::PrintLogWebResponse::SwitchProtocol(ProtocolHandler *protoHdlr)
{
	return this->resp->SwitchProtocol(protoHdlr);
}

Text::CString Net::WebServer::PrintLogWebResponse::GetRespHeaders()
{
	return this->resp->GetRespHeaders();
}

Bool Net::WebServer::PrintLogWebResponse::IsDown() const
{
	return this->resp->IsDown();
}

UOSInt Net::WebServer::PrintLogWebResponse::Read(const Data::ByteArray &buff)
{
	return this->resp->Read(buff);
}

UOSInt Net::WebServer::PrintLogWebResponse::Write(const UInt8 *buff, UOSInt size)
{
	return this->resp->Write(buff, size);
}

Int32 Net::WebServer::PrintLogWebResponse::Flush()
{
	return this->resp->Flush();
}

void Net::WebServer::PrintLogWebResponse::Close()
{
	this->resp->Close();
}

Bool Net::WebServer::PrintLogWebResponse::Recover()
{
	return this->resp->Recover();
}

IO::StreamType Net::WebServer::PrintLogWebResponse::GetStreamType() const
{
	return IO::StreamType::WebConnection;
}
