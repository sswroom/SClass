#include "Stdafx.h"
#include "Net/HTTPClient.h"
#include "SSWR/ServerMonitor/URLMonitorClient.h"

SSWR::ServerMonitor::URLMonitorClient::URLMonitorClient(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Text::CStringNN target, Int32 timeoutMS)
{
	this->clif = clif;
	this->ssl = ssl;
	this->timeoutMS = timeoutMS;
	UIntOS i = target.IndexOf('|');
	if (i != INVALID_INDEX)
	{
		this->url = Text::String::New(target.v, i);
		this->containsText = Text::String::New(target.Substring(i + 1));
	}
	else
	{
		this->url = Text::String::New(target);
		this->containsText = nullptr;
	}
}

SSWR::ServerMonitor::URLMonitorClient::URLMonitorClient(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Text::CStringNN url, Text::CString containsText, Int32 timeoutMS)
{
	this->clif = clif;
	this->ssl = ssl;
	this->url = Text::String::New(url);
	this->containsText = Text::String::NewOrNull(containsText);
	this->timeoutMS = timeoutMS;
}

SSWR::ServerMonitor::URLMonitorClient::~URLMonitorClient()
{
	this->url->Release();
	OPTSTR_DEL(this->containsText);
}

Bool SSWR::ServerMonitor::URLMonitorClient::HasError() const
{
	return !this->url->StartsWith(CSTR("http://")) && !this->url->StartsWith(CSTR("https://"));
}

Bool SSWR::ServerMonitor::URLMonitorClient::ServerValid()
{
	if (!this->url->StartsWith(CSTR("http://")) && !this->url->StartsWith(CSTR("https://")))
	{
		return false;
	}
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->clif, this->ssl, this->url->ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, false);
	if (cli->IsError())
	{
		cli.Delete();
		return false;
	}
	cli->SetTimeout((UInt32)this->timeoutMS);
	Net::WebStatus::StatusCode status = cli->GetRespStatus();
	if (status == Net::WebStatus::SC_NO_CONTENT)
	{
		cli.Delete();
		return this->containsText.IsNull();
	}
	else if (status == Net::WebStatus::SC_OK)
	{
		NN<Text::String> containsText;
		if (!this->containsText.SetTo(containsText))
		{
			cli.Delete();
			return true;
		}
		Text::StringBuilderUTF8 sb;
		if (cli->ReadAllContent(sb, 16384, 10485760))
		{
			cli.Delete();
			return sb.IndexOf(containsText->ToCString()) != INVALID_INDEX;
		}
		else
		{
			cli.Delete();
			return false;
		}
	}
	else
	{
		cli.Delete();
		return false;
	}
}

Bool SSWR::ServerMonitor::URLMonitorClient::BuildTarget(NN<Text::StringBuilderUTF8> target)
{
	if (this->url->StartsWith(CSTR("http://")) || this->url->StartsWith(CSTR("https://")))
	{
		NN<Text::String> s;
		target->Append(this->url->ToCString());
		if (this->containsText.SetTo(s))
		{
			target->AppendUTF8Char('|');
			target->Append(s);
		}
		return true;
	}
	return false;
}
