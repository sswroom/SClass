#include "Stdafx.h"
#include "IO/MemoryReadingStream.h"
#include "Net/HTTPClient.h"
#include "Net/SMSGatewaySmartone.h"
#include "Text/StringTool.h"
#include "Text/XML.h"
#include "Text/XMLReader.h"

//#define VERBOSE

Net::SMSGatewaySmartone::SMSGatewaySmartone(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Optional<IO::LogTool> log, Text::CStringNN fqdn, Text::CStringNN accountId, Text::CStringNN password)
{
	this->clif = clif;
	this->ssl = ssl;
	this->log = log;
	this->fqdn = Text::String::New(fqdn);
	this->accountId = Text::String::New(accountId);
	this->password = Text::String::New(password);
	this->lastReqError = RequestStatus::NetworkError;
}

Net::SMSGatewaySmartone::~SMSGatewaySmartone()
{
	this->fqdn->Release();
	this->accountId->Release();
	this->password->Release();
}

Bool Net::SMSGatewaySmartone::IsTargetValid(Text::CStringNN targetNum)
{
	return targetNum.StartsWith('+') && Text::StringTool::IsUInteger(targetNum.v + 1);
}

Bool Net::SMSGatewaySmartone::SendSMS(Text::CStringNN targetNum, Text::CStringNN msg)
{
	if (!IsTargetValid(targetNum) || msg.leng == 0)
	{
		this->lastReqError = RequestStatus::InvalidParam;
		return false;
	}
	Text::StringBuilderUTF8 url;
	Text::StringBuilderUTF8 data;
	NN<Text::String> s;
	url.Append(CSTR("https://"))->Append(this->fqdn)->Append(CSTR("/appprod/servlet/nGenSendSM"));

	data.Append(CSTR("<Request>\r\n"));
	s = Text::XML::ToNewXMLText(this->accountId->v);
	data.Append(CSTR("<acc>"))->Append(s)->Append(CSTR("</acc>\r\n"));
	s->Release();
	s = Text::XML::ToNewXMLText(this->password->v);
	data.Append(CSTR("<pwd>"))->Append(s)->Append(CSTR("</pwd>\r\n"));
	s->Release();
	if (targetNum.StartsWith(CSTR("+852")))
	{
		data.Append(CSTR("<msisdn>"))->Append(targetNum.Substring(1))->Append(CSTR("</msisdn>\r\n"));
	}
	else
	{
		data.Append(CSTR("<msisdn>"))->Append(targetNum)->Append(CSTR("</msisdn>\r\n"));
	}
	data.Append(CSTR("<suffix>0</suffix>\r\n"));
	s = Text::XML::ToNewXMLText(msg.v);
	data.Append(CSTR("<msg>"))->Append(s)->Append(CSTR("</msg>\r\n"));
	s->Release();
	data.Append(CSTR("<req_sms_status>n</req_sms_status>\r\n"));
	data.Append(CSTR("<xmlResp>y</xmlResp>\r\n"));
//	data.Append(CSTR("<user_msg_id/>\r\n"));
//	data.Append(CSTR("<opt_out/>\r\n"));
	data.Append(CSTR("</Request>"));
#if defined(VERBOSE)
	printf("%s\r\n", data.v.Ptr());
#endif
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(clif, ssl, url.ToCString(), Net::WebUtil::RequestMethod::HTTP_POST, false);
	cli->AddContentType(CSTR("text/xml; charset=utf-8"));
	cli->AddContentLength(data.GetLength());
	cli->WriteCont(data.v, data.GetLength());
	Net::WebStatus::StatusCode code = cli->GetRespStatus();
	if (code == Net::WebStatus::SC_OK)
	{
		data.ClearStr();
		cli->ReadAllContent(data, 4096, 1048576);
#if defined(VERBOSE)
		printf("%s\r\n", data.v.Ptr());
#endif
		url.ClearStr();
		IO::MemoryReadingStream mstm(data.ToByteArray());
		Text::XMLReader reader(0, mstm, Text::XMLReader::PM_XML);
		if (reader.NextElementName().SetTo(s) && s->Equals(CSTR("Response")))
		{
			if (reader.NextElementName().SetTo(s) && s->Equals(CSTR("RequestStatus")))
			{
				reader.ReadNodeText(url);
			}
		}
		UInt32 istatus;
		if (url.leng > 0 && url.ToUInt32(istatus))
		{
			this->lastReqError = (RequestStatus)istatus;
		}
		else
		{
			this->lastReqError = RequestStatus::NetworkError;
		}
	}
	else
	{
		this->lastReqError = RequestStatus::NetworkError;
	}
	cli.Delete();
	return this->lastReqError == RequestStatus::Success;
}

Net::SMSGatewaySmartone::RequestStatus Net::SMSGatewaySmartone::GetLastReqError() const
{
	return this->lastReqError;
}

Text::CStringNN Net::SMSGatewaySmartone::RequestStatusGetName(RequestStatus val)
{
	switch (val)
	{
	case RequestStatus::Success:
		return CSTR("Success");
	case RequestStatus::MissingParam:
		return CSTR("MissingParam");
	case RequestStatus::IPNotAllow:
		return CSTR("IPNotAllow");
	case RequestStatus::InvalidParam:
		return CSTR("InvalidParam");
	case RequestStatus::SendError:
		return CSTR("SendError");
	case RequestStatus::UEMOOptOutNotAllowed:
		return CSTR("UEMOOptOutNotAllowed");
	case RequestStatus::UniqueMessageDuplicated:
		return CSTR("UniqueMessageDuplicated");
	case RequestStatus::ServerError:
		return CSTR("ServerError");
	case RequestStatus::UserMessageIDError:
		return CSTR("UserMessageIDError");
	case RequestStatus::MessageQueueLimitExceed:
		return CSTR("MessageQueueLimitExceed");
	case RequestStatus::RequestsPerSecondExceed:
		return CSTR("RequestsPerSecondExceed");
	case RequestStatus::NetworkError:
		return CSTR("NetworkError");
	default:
		return CSTR("Unknown");
	}
}
