#include "Stdafx.h"
#include "Net/HTTPClient.h"
#include "Net/JMeter/JMeterHTTPSamplerProxy.h"

Bool Net::JMeter::JMeterHTTPSamplerProxy::Step(NN<JMeterIteration> iter, NN<JMeterResult> result) const
{
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateClient(iter->GetTCPClientFactory(), iter->GetSSLEngine(), CSTR_NULL, this->useKeepalive, this->url->StartsWith(UTF8STRC("https://")));
	Double timeDNS;
	Double timeConn;
	Double timeReq;
	Double timeResp;
	UInt8 buff[2048];
	Bool succ = false;
	if (cli->Connect(url->ToCString(), this->method, timeDNS, timeConn, false))
	{
		if (this->enableGZip)
		{
			cli->AddHeaderC(CSTR("Accept-Encoding"), CSTR("gzip, deflate"));
		}
		if (this->useKeepalive)
		{
			cli->AddHeaderC(CSTR("Connection"), CSTR("keep-alive"));
		}
		else
		{
			cli->AddHeaderC(CSTR("Connection"), CSTR("close"));
		}
		iter->HTTPBegin(cli);
		NN<Text::String> contType;
		NN<Data::ByteBuffer> postData;
		if (this->method == Net::WebUtil::RequestMethod::HTTP_POST && this->postType.SetTo(contType) && this->postData.SetTo(postData))
		{
			cli->AddContentType(contType->ToCString());
			cli->AddContentLength(postData->GetSize());
			cli->WriteCont(postData->Arr(), postData->GetSize());
		}
		cli->EndRequest(timeReq, timeResp);
		if (timeResp >= 0)
		{
			UInt64 totalSize = 0;
			UOSInt readSize;
			while ((readSize = cli->Read(BYTEARR(buff))) != 0)
			{
				totalSize += readSize;
			}
			result->dataSize = totalSize;
			succ = cli->GetRespStatus() == 200;
		}
		iter->HTTPEnd(cli, succ);
	}
	cli.Delete();
	return succ;
}

void Net::JMeter::JMeterHTTPSamplerProxy::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	sb->Append(Net::WebUtil::RequestMethodGetName(this->method));
	sb->AppendUTF8Char(' ');
	sb->Append(this->url);
}
