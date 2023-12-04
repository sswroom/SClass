#include "Stdafx.h"
#include "Net/HTTPClient.h"
#include "Net/SMSGatewayIntersoft.h"
#include "Text/XMLDOM.h"
#include "Text/TextBinEnc/FormEncoding.h"

#define VERBOSE
#if defined(VERBOSE)
#include <stdio.h>
#endif

Net::SMSGatewayIntersoft::SMSGatewayIntersoft(NotNullPtr<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Text::EncodingFactory *encFact, IO::LogTool *log)
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
	SDEL_STRING(this->userName);
	SDEL_STRING(this->password);
}

Bool Net::SMSGatewayIntersoft::IsTargetValid(Text::CString targetNum)
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

Bool Net::SMSGatewayIntersoft::SendSMS(Text::CString targetNum, Text::CString msg)
{
	if (this->userName == 0 || this->password == 0)
	{
		return false;
	}
	return this->SendSMS(this->userName->ToCString(), this->password->ToCString(), targetNum, msg);
}

void Net::SMSGatewayIntersoft::SetAccount(Text::CString userName, Text::CString password)
{
	if (userName.leng > 0 && password.leng > 0)
	{
		SDEL_STRING(this->userName);
		SDEL_STRING(this->password);
		this->userName = Text::String::New(userName);
		this->password = Text::String::New(password);
	}
}

Bool Net::SMSGatewayIntersoft::SendSMS(Text::CString userName, Text::CString password, Text::CString targetNum, Text::CString msg)
{
	UTF8Char sbuff[2048];
	UTF8Char *sptr;
	if (!targetNum.StartsWith(UTF8STRC("+852")))
	{
		return false;
	}
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("https://login.speedfax.net/SMSWS/sms.asmx/AddMessage?UserName="));
	sptr = Text::TextBinEnc::FormEncoding::FormEncode(sbuff, userName.v);
	sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
	sb.AppendC(UTF8STRC("&Password="));
	sptr = Text::TextBinEnc::FormEncoding::FormEncode(sbuff, password.v);
	sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
	sb.AppendC(UTF8STRC("&MobileNumber="));
	sptr = Text::TextBinEnc::FormEncoding::FormEncode(sbuff, &targetNum.v[4]);
	sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
	sb.AppendC(UTF8STRC("&Message="));
	sptr = Text::TextBinEnc::FormEncoding::FormEncode(sbuff, msg.v);
	sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
	sb.AppendC(UTF8STRC("&ScheduleTime="));

	UInt8 dataBuff[2048];
	UOSInt readSize;
	Text::StringBuilderUTF8 sbMsg;
	Net::HTTPClient *cli = Net::HTTPClient::CreateConnect(sockf, ssl, sb.ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, true);
	if (cli)
	{
		Int32 status = (Int32)cli->GetRespStatus();
		IO::MemoryStream mstm(UTF8STRC("Net.SMSGatewayIntersoft.SendSMS.mstm"));
		while ((readSize = cli->Read(dataBuff, 2048)) > 0)
		{
			mstm.Write(dataBuff, readSize);
		}
		DEL_CLASS(cli);

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
		UInt8 *memBuff = mstm.GetBuff(&readSize);
		Text::XMLDocument doc;
		if (doc.ParseBuff(this->encFact, memBuff, readSize))
		{
			Text::XMLNode *node = doc.SearchFirstNode(CSTR("/MessageResult/Accept"));
			if (node == 0)
			{
				if (this->log)
				{
					this->log->LogMessage(CSTR("SMSGatewayIntersoft: Response error, Accept not found"), IO::LogHandler::LogLevel::Error);
				}
			}
			else
			{
				sbMsg.ClearStr();
				node->GetInnerText(sbMsg);
				if (this->log)
				{
					sb.ClearStr();
					sb.AppendC(UTF8STRC("SMSGatewayIntersoft: Response Accept = "));
					sb.Append(sbMsg);
					this->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);
				}
				if (sbMsg.Equals(UTF8STRC("true")))
				{
					ret = true;
				}

				if (this->log)
				{
					if (ret)
					{
						node = doc.SearchFirstNode(CSTR("/MessageResult/MessageID"));
						if (node == 0)
						{
							this->log->LogMessage(CSTR("SMSGatewayIntersoft: Response MessageID = not found"), IO::LogHandler::LogLevel::Error);
						}
						else
						{
							sbMsg.ClearStr();
							sb.AppendC(UTF8STRC("SMSGatewayIntersoft: Response MessageID = "));
							node->GetInnerText(sbMsg);
							this->log->LogMessage(sbMsg.ToCString(), IO::LogHandler::LogLevel::Command);
						}
					}
					else
					{
						node = doc.SearchFirstNode(CSTR("/MessageResult/FailReason"));
						if (node == 0)
						{
							this->log->LogMessage(CSTR("SMSGatewayIntersoft: Response FailReason = not found"), IO::LogHandler::LogLevel::Error);
						}
						else
						{
							sbMsg.ClearStr();
							sb.AppendC(UTF8STRC("SMSGatewayIntersoft: Response FailReason = "));
							node->GetInnerText(sbMsg);
							this->log->LogMessage(sbMsg.ToCString(), IO::LogHandler::LogLevel::Error);
						}
					}
				}
			}
		}
		else
		{
			if (this->log)
			{
				this->log->LogMessage(CSTR("SMSGatewayIntersoft: Error in parsing response"), IO::LogHandler::LogLevel::Error);
			}
		}
		return ret;
	}
	else
	{
		if (this->log)
		{
			sbMsg.ClearStr();
			sbMsg.AppendC(UTF8STRC("Error in connecting to URL: "));
			sbMsg.AppendC(sb.ToString(), sb.GetLength());
			this->log->LogMessage(sbMsg.ToCString(), IO::LogHandler::LogLevel::Error);
			return false;
		}
	}
}
