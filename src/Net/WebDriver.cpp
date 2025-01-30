#include "Stdafx.h"
#include "Net/WebDriver.h"

Net::WebDriver::WebDriver(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Text::CStringNN url, NN<WebDriverParam> param)
{
	this->clif = clif;
	this->ssl = ssl;
	if (url.EndsWith('/'))
	{
		this->url = Text::String::New(url);
	}
	else
	{
		this->url = Text::String::New(url.leng + 1);
		UnsafeArray<UTF8Char> sptr = url.ConcatTo(this->url->v);
		*sptr = '/';
		sptr[1] = 0;
	}
	this->sessId = 0;
}

Net::WebDriver::~WebDriver()
{
	this->url->Release();
	OPTSTR_DEL(this->sessId);
}

Bool Net::WebDriver::HasSession() const
{
	return false;
}
