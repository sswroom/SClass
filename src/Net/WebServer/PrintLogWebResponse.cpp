#include "Stdafx.h"
#include "Net/WebServer/PrintLogWebResponse.h"
#include "Text/StringBuilderUTF8.h"

Net::WebServer::PrintLogWebResponse::PrintLogWebResponse(Net::WebServer::IWebResponse *resp, IO::Writer *writer, const UTF8Char *prefix) : Net::WebServer::IWebResponse(resp->GetSourceNameObj())
{
	this->resp = resp;
	this->writer = writer;
	this->prefix = SCOPY_TEXT(prefix);
}

Net::WebServer::PrintLogWebResponse::~PrintLogWebResponse()
{
	SDEL_TEXT(this->prefix);
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
		if (this->prefix)
		{
			sb.Append(this->prefix);
		}
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

Bool Net::WebServer::PrintLogWebResponse::AddHeader(const UTF8Char *name, const UTF8Char *value)
{
	if (this->resp->AddHeader(name, value))
	{
		Text::StringBuilderUTF8 sb;
		if (this->prefix)
		{
			sb.Append(this->prefix);
		}
		sb.Append(name);
		sb.AppendC(UTF8STRC(": "));
		sb.Append(value);
		this->writer->WriteLineC(sb.ToString(), sb.GetLength());
		return true;
	}
	return false;
}

Bool Net::WebServer::PrintLogWebResponse::AddDefHeaders(Net::WebServer::IWebRequest *req)
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

Bool Net::WebServer::PrintLogWebResponse::ResponseSSE(Int32 timeoutMS, SSEDisconnectHandler hdlr, void *userObj)
{
	return this->resp->ResponseSSE(timeoutMS, hdlr, userObj);
}

Bool Net::WebServer::PrintLogWebResponse::SSESend(const UTF8Char *eventName, const UTF8Char *data)
{
	return this->resp->SSESend(eventName, data);
}

const UTF8Char *Net::WebServer::PrintLogWebResponse::GetRespHeaders()
{
	return this->resp->GetRespHeaders();
}

UOSInt Net::WebServer::PrintLogWebResponse::Read(UInt8 *buff, UOSInt size)
{
	return this->resp->Read(buff, size);
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
