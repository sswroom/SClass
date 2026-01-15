#include "Stdafx.h"
#include "Net/RESTResource.h"

void Net::RESTResource::BuildURL(NN<Text::StringBuilderUTF8> sb, Text::CStringNN command) const
{
	sb->Append(this->url);
	if (command.leng > 0)
	{
		if (!sb->EndsWith('/'))
		{
			sb->AppendUTF8Char('/');
		}
		sb->Append(command);
	}
}

Net::WebStatus::StatusCode Net::RESTResource::DoRequest(Text::CStringNN command, Net::WebUtil::RequestMethod method, Optional<RESTParam> param, NN<IO::Stream> respStm)
{
	NN<RESTParam> nnparam;
	Text::StringBuilderUTF8 sb;
	this->BuildURL(sb, command);
	NN<Net::HTTPClient> cli;
	if (this->cli.SetTo(cli))
	{
		cli->Connect(sb.ToCString(), method, 0, 0, true);
	}
	else
	{
		cli = Net::HTTPClient::CreateConnect(this->clif, this->ssl, sb.ToCString(), method, true);
		if (cli->IsError())
		{
			cli.Delete();
			return Net::WebStatus::StatusCode::SC_UNKNOWN;
		}
		if (this->timeout.IsPositiveNonZero())
		{
			cli->SetTimeout(this->timeout);
		}
		this->cli = cli;
	}
	if (param.SetTo(nnparam))
	{
		nnparam->WriteParam(cli);
	}
	Net::WebStatus::StatusCode status = cli->GetRespStatus();
	cli->ReadAllContent(respStm, 16384, 1048576);
	return status;
}

Net::RESTResource::RESTResource(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Text::CStringNN url)
{
	this->clif = clif;
	this->ssl = ssl;
	this->url = Text::String::New(url);
	this->cli = nullptr;
	this->timeout = 0;
}

Net::RESTResource::~RESTResource()
{
	this->url->Release();
	this->cli.Delete();
}

void Net::RESTResource::SetTimeout(Data::Duration timeout)
{
	this->timeout = timeout;
	NN<Net::HTTPClient> cli;
	if (this->cli.SetTo(cli))
	{
		cli->SetTimeout(timeout);
	}
}

NN<Net::RESTResource> Net::RESTResource::CreateSubResource(Text::CStringNN subPath)
{
	Text::StringBuilderUTF8 sb;
	sb.Append(this->url);
	if (!sb.EndsWith('/')) sb.AppendUTF8Char('/');
	sb.Append(subPath);
	NN<RESTResource> res;
	NEW_CLASSNN(res, Net::RESTResource(this->clif, this->ssl, sb.ToCString()));
	return res;
}

Net::WebStatus::StatusCode Net::RESTResource::Get(Text::CStringNN command, NN<IO::Stream> respStm)
{
	return this->DoRequest(command, Net::WebUtil::RequestMethod::HTTP_GET, nullptr, respStm);
}

Net::WebStatus::StatusCode Net::RESTResource::Post(Text::CStringNN command, NN<RESTParam> param, NN<IO::Stream> respStm)
{
	return this->DoRequest(command, Net::WebUtil::RequestMethod::HTTP_POST, param, respStm);
}

Net::WebStatus::StatusCode Net::RESTResource::Put(Text::CStringNN command, NN<RESTParam> param, NN<IO::Stream> respStm)
{
	return this->DoRequest(command, Net::WebUtil::RequestMethod::HTTP_PUT, param, respStm);
}

Net::WebStatus::StatusCode Net::RESTResource::Delete(Text::CStringNN command, Optional<RESTParam> param, NN<IO::Stream> respStm)
{
	return this->DoRequest(command, Net::WebUtil::RequestMethod::HTTP_DELETE, param, respStm);
}
