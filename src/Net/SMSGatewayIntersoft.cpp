#include "Stdafx.h"
#include "Net/HTTPClient.h"
#include "Net/SMSGatewayIntersoft.h"
#include "Text/XMLDOM.h"
#include "Text/TextBinEnc/FormEncoding.h"

#define VERBOSE
#if defined(VERBOSE)
#include <stdio.h>
#endif

Net::SMSGatewayIntersoft::SMSGatewayIntersoft(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, NN<Text::EncodingFactory> encFact, Optional<IO::LogTool> log)
{
	this->sockf = sockf;
	this->ssl = ssl;
	this->log = log;
	this->encFact = encFact;
	this->userName = 0;
	this->password = 0;
}

Net::SMSGatewayIntersoft::~SMSGatewayIntersoft()
{
	OPTSTR_DEL(this->userName);
	OPTSTR_DEL(this->password);
}

Bool Net::SMSGatewayIntersoft::IsTargetValid(Text::CStringNN targetNum)
{
	if (!targetNum.StartsWith(UTF8STRC("+852")))
	{
		return false;
	}
	if (targetNum.leng <= 4)
	{
		return false;
	}
	return true;

}

Bool Net::SMSGatewayIntersoft::SendSMS(Text::CStringNN targetNum, Text::CStringNN msg)
{
	NN<Text::String> userName;
	NN<Text::String> password;
	if (!this->userName.SetTo(userName) || !this->password.SetTo(password))
	{
		return false;
	}
	return this->SendSMS(userName->ToCString(), password->ToCString(), targetNum, msg);
}

void Net::SMSGatewayIntersoft::SetAccount(Text::CStringNN userName, Text::CStringNN password)
{
	if (userName.leng > 0 && password.leng > 0)
	{
		OPTSTR_DEL(this->userName);
		OPTSTR_DEL(this->password);
		this->userName = Text::String::New(userName);
		this->password = Text::String::New(password);
	}
}

Bool Net::SMSGatewayIntersoft::SendSMS(Text::CStringNN userName, Text::CStringNN password, Text::CStringNN targetNum, Text::CStringNN msg)
{
	UTF8Char sbuff[2048];
	UnsafeArray<UTF8Char> sptr;
	if (!targetNum.StartsWith(UTF8STRC("+852")))
	{
		return false;
	}
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("https://login.speedfax.net/SMSWS/sms.asmx/AddMessage?UserName="));
	sptr = Text::TextBinEnc::FormEncoding::FormEncode(sbuff, userName.v);
	sb.AppendC(sbuff, (UIntOS)(sptr - sbuff));
	sb.AppendC(UTF8STRC("&Password="));
	sptr = Text::TextBinEnc::FormEncoding::FormEncode(sbuff, password.v);
	sb.AppendC(sbuff, (UIntOS)(sptr - sbuff));
	sb.AppendC(UTF8STRC("&MobileNumber="));
	sptr = Text::TextBinEnc::FormEncoding::FormEncode(sbuff, &targetNum.v[4]);
	sb.AppendC(sbuff, (UIntOS)(sptr - sbuff));
	sb.AppendC(UTF8STRC("&Message="));
	sptr = Text::TextBinEnc::FormEncoding::FormEncode(sbuff, msg.v);
	sb.AppendC(sbuff, (UIntOS)(sptr - sbuff));
	sb.AppendC(UTF8STRC("&ScheduleTime="));

	NN<IO::LogTool> log;
	UInt8 dataBuff[2048];
	UIntOS readSize;
	Text::StringBuilderUTF8 sbMsg;
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(sockf, ssl, sb.ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, true);
	if (!cli->IsError())
	{
		Int32 status = (Int32)cli->GetRespStatus();
		IO::MemoryStream mstm;
		while ((readSize = cli->Read(Data::ByteArray(dataBuff, 2048))) > 0)
		{
			mstm.Write(Data::ByteArrayR(dataBuff, readSize));
		}
		cli.Delete();

#if defined(VERBOSE)
		sbMsg.ClearStr();
		sbMsg.AppendC(UTF8STRC("Requesting to "));
		sbMsg.AppendC(sb.ToString(), sb.GetLength());
		sbMsg.AppendC(UTF8STRC(" ("));
		sbMsg.AppendI32(status);
		sbMsg.AppendUTF8Char(')');
		printf("%s\r\n", sbMsg.ToString());
#endif

		Bool ret = false;
		UnsafeArray<UInt8> memBuff = mstm.GetBuff(readSize);
		Text::XMLDocument doc;
		if (doc.ParseBuff(this->encFact, memBuff, readSize))
		{
			NN<Text::XMLNode> node;
			if (!doc.SearchFirstNode(CSTR("/MessageResult/Accept")).SetTo(node))
			{
				if (this->log.SetTo(log))
				{
					log->LogMessage(CSTR("SMSGatewayIntersoft: Response error, Accept not found"), IO::LogHandler::LogLevel::Error);
				}
			}
			else
			{
				sbMsg.ClearStr();
				node->GetInnerText(sbMsg);
				if (this->log.SetTo(log))
				{
					sb.ClearStr();
					sb.AppendC(UTF8STRC("SMSGatewayIntersoft: Response Accept = "));
					sb.Append(sbMsg);
					log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);
				}
				if (sbMsg.Equals(UTF8STRC("true")))
				{
					ret = true;
				}

				if (this->log.SetTo(log))
				{
					if (ret)
					{
						if (!doc.SearchFirstNode(CSTR("/MessageResult/MessageID")).SetTo(node))
						{
							log->LogMessage(CSTR("SMSGatewayIntersoft: Response MessageID = not found"), IO::LogHandler::LogLevel::Error);
						}
						else
						{
							sbMsg.ClearStr();
							sb.AppendC(UTF8STRC("SMSGatewayIntersoft: Response MessageID = "));
							node->GetInnerText(sbMsg);
							log->LogMessage(sbMsg.ToCString(), IO::LogHandler::LogLevel::Command);
						}
					}
					else
					{
						if (!doc.SearchFirstNode(CSTR("/MessageResult/FailReason")).SetTo(node))
						{
							log->LogMessage(CSTR("SMSGatewayIntersoft: Response FailReason = not found"), IO::LogHandler::LogLevel::Error);
						}
						else
						{
							sbMsg.ClearStr();
							sb.AppendC(UTF8STRC("SMSGatewayIntersoft: Response FailReason = "));
							node->GetInnerText(sbMsg);
							log->LogMessage(sbMsg.ToCString(), IO::LogHandler::LogLevel::Error);
						}
					}
				}
			}
		}
		else
		{
			if (this->log.SetTo(log))
			{
				log->LogMessage(CSTR("SMSGatewayIntersoft: Error in parsing response"), IO::LogHandler::LogLevel::Error);
			}
		}
		return ret;
	}
	else
	{
		cli.Delete();
		if (this->log.SetTo(log))
		{
			sbMsg.ClearStr();
			sbMsg.AppendC(UTF8STRC("Error in connecting to URL: "));
			sbMsg.AppendC(sb.ToString(), sb.GetLength());
			log->LogMessage(sbMsg.ToCString(), IO::LogHandler::LogLevel::Error);
			return false;
		}
	}
}
