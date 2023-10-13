#include "Stdafx.h"
#include "Net/WebServer/GCISNotifyHandler.h"
#include "Text/JSONBuilder.h"
#include "Text/MailCreator.h"

#include <stdio.h>

Bool __stdcall Net::WebServer::GCISNotifyHandler::NotifyFunc(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, WebServiceHandler *hdlr)
{
	GCISNotifyHandler *me = (GCISNotifyHandler*)hdlr;
	Crypto::Cert::X509Cert *cert = req->GetClientCert();
	Text::StringBuilderUTF8 sb2;
	Text::JSONBuilder builder(Text::JSONBuilder::OT_OBJECT);
	if (cert == 0)
	{
		resp->SetStatusCode(Net::WebStatus::SC_INTERNAL_SERVER_ERROR);
		builder.ObjectAddStr(CSTR("description"), CSTR("Client Cert is missing"));
		builder.ObjectAddStr(CSTR("code"), CSTR("9999"));
		me->log->LogMessage(CSTR("Client Cert is missing"), IO::LogHandler::LogLevel::Error);
	}
	else
	{
		Bool failed = false;
		UOSInt dataSize;
		const UInt8 *content = req->GetReqData(dataSize);
		Text::JSONBase *json = Text::JSONBase::ParseJSONBytes(content, dataSize);
		Text::CString resultCd = CSTR("0000");
		Text::CString resultMsg = CSTR("Request processed successfully.");
		if (json == 0)
		{
			failed = true;
			resultCd = CSTR("0021");
			resultMsg = CSTR("Invalid content type");
			me->log->LogMessage(CSTR("Cannot parse JSON"), IO::LogHandler::LogLevel::Error);
		}
		else if (json->GetType() != Text::JSONType::Object)
		{
			failed = true;
			resultCd = CSTR("0021");
			resultMsg = CSTR("Invalid content type");
			me->log->LogMessage(CSTR("JSON is not object"), IO::LogHandler::LogLevel::Error);
		}
		else
		{
			Text::MailCreator mail;
			cert->GetSubjectCN(sb2);
			mail.SetFrom(0, sb2.ToCString());
			Text::JSONObject *msgObj = (Text::JSONObject*)json;
			NotNullPtr<Text::String> s;
			if (s.Set(msgObj->GetObjectString(CSTR("ChanType"))) && !s->Equals(UTF8STRC("EM")) && !s->Equals(UTF8STRC("BD")))
			{
				failed = true;
				resultCd = CSTR("0074");
				resultMsg = CSTR("Invalid Channel Type.");
				me->log->LogMessage(CSTR("Invalid Channel Type"), IO::LogHandler::LogLevel::Error);
			}
			if (!failed)
			{
				Text::JSONArray *recipientDetail = msgObj->GetObjectArray(CSTR("RecipientDetail"));
				if (recipientDetail == 0)
				{
					failed = true;
					resultCd = CSTR("0032");
					resultMsg = CSTR("No recipient found.");
					me->log->LogMessage(CSTR("No recipient found 1"), IO::LogHandler::LogLevel::Error);
				}
				else
				{
					Text::JSONObject *recipient;
					UOSInt i = 0;
					UOSInt j = recipientDetail->GetArrayLength();
					while (i < j)
					{
						recipient = recipientDetail->GetArrayObject(i);
						if (recipient == 0)
						{
							failed = true;
							resultCd = CSTR("0032");
							resultMsg = CSTR("No recipient found.");
							me->log->LogMessage(CSTR("No recipient found 2"), IO::LogHandler::LogLevel::Error);
							break;
						}
						else if (s.Set(recipient->GetObjectString(CSTR("ChanAddr"))))
						{
							mail.ToAdd(0, s);
						}
						else if (s.Set(recipient->GetObjectString(CSTR("CcAddr"))))
						{
							mail.CCAdd(0, s);
						}
						else if (s.Set(recipient->GetObjectString(CSTR("BccAddr"))))
						{
//								mail.BccAdd(0, s);
						}
						else
						{
							failed = true;
							resultCd = CSTR("0032");
							resultMsg = CSTR("No recipient found.");
							me->log->LogMessage(CSTR("No recipient found 3"), IO::LogHandler::LogLevel::Error);
							break;
						}

						i++;
					}
				}
			}
			if (!failed)
			{
				Text::JSONObject *contentDetail = msgObj->GetObjectObject(CSTR("ContentDetail"));
				Text::String *content;
				if (contentDetail == 0)
				{
					failed = true;
					resultCd = CSTR("0024");
					resultMsg = CSTR("Content field is empty.");
					me->log->LogMessage(CSTR("Content field is empty"), IO::LogHandler::LogLevel::Error);
				}
				else if (s.Set(contentDetail->GetObjectString(CSTR("ContentType"))))
				{
					content = contentDetail->GetObjectString(CSTR("Content"));
					if (content == 0)
					{
						failed = true;
						resultCd = CSTR("0024");
						resultMsg = CSTR("Content field is empty.");
						me->log->LogMessage(CSTR("Content field is empty"), IO::LogHandler::LogLevel::Error);
					}
					else if (s->Equals(UTF8STRC("text/html")))
					{
						mail.SetContentHTML(content, CSTR("."));
					}
					else if (s->Equals(UTF8STRC("test/plain")))
					{
						mail.SetContentText(content);
					}
					else
					{
						failed = true;
						resultCd = CSTR("0021");
						resultMsg = CSTR("Invalid content type.");
						me->log->LogMessage(CSTR("Invalid content type 1"), IO::LogHandler::LogLevel::Error);
					}
				}
				else
				{
					failed = true;
					resultCd = CSTR("0021");
					resultMsg = CSTR("Invalid content type.");
					me->log->LogMessage(CSTR("Invalid content type 2"), IO::LogHandler::LogLevel::Error);
				}
				if (!failed)
				{
					if (s.Set(contentDetail->GetObjectString(CSTR("Subject"))))
					{
						mail.SetSubject(s);
					}
					else
					{
						failed = true;
						resultCd = CSTR("0021");
						resultMsg = CSTR("Invalid content type.");
						me->log->LogMessage(CSTR("Invalid content type 3"), IO::LogHandler::LogLevel::Error);
					}
				}
			}
			if (!failed)
			{
				NotNullPtr<Text::MIMEObj::MailMessage> msg = mail.CreateMail();
				me->hdlr(me->hdlrObj, req->GetNetConn(), msg);
				msg.Delete();
				me->log->LogMessage(CSTR("Message created"), IO::LogHandler::LogLevel::Action);
			}
			//////////////////////////////
		}

		builder.ObjectAddStr(CSTR("Status"), failed?CSTR("F"):CSTR("S"));
		builder.ObjectBeginObject(CSTR("NotificationStatus"));
		NotNullPtr<Text::JSONBase> nnjson;
		if (nnjson.Set(json))
		{
			if (nnjson->GetType() == Text::JSONType::Object)
				builder.ObjectAdd(NotNullPtr<Text::JSONObject>::ConvertFrom(nnjson));
			nnjson->EndUse();
		}
		if (!failed)
		{
			sb2.ClearStr();
			cert->GetSubjectCN(sb2);
			builder.ObjectAddStr(CSTR("Client"), sb2.ToCString());

			builder.ObjectAddStr(CSTR("ResultMesg"), resultMsg);
			builder.ObjectAddStr(CSTR("ResultCd"), resultCd);
		}
		else
		{
			builder.ObjectAddStr(CSTR("ResultMesg"), resultMsg);
			builder.ObjectAddStr(CSTR("ResultCd"), resultCd);
		}
	}
	return resp->ResponseJSONStr(req, 0, builder.Build());
}

Bool __stdcall Net::WebServer::GCISNotifyHandler::BatchUplFunc(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, WebServiceHandler *me)
{
	UOSInt size;
	const UInt8 *data = req->GetReqData(size);
	Text::StringBuilderUTF8 sb;
	sb.AppendC(data, size);
	printf("%s\r\n", sb.v);

	UTF8Char sbuff[128];
	UTF8Char *sptr;
	Text::JSONBuilder builder(Text::JSONBuilder::OT_OBJECT);
	sptr = Text::StrInt64(sbuff, Data::DateTimeUtil::GetCurrTimeMillis());
	builder.ObjectAddStr(CSTR("UploadRefNum"), CSTRP(sbuff, sptr));
	builder.ObjectAddStr(CSTR("ResultCd"), CSTR("0000"));
	return resp->ResponseJSONStr(req, 0, builder.Build());
}

Net::WebServer::GCISNotifyHandler::GCISNotifyHandler(Text::CStringNN notifyPath, Text::CStringNN batchUplPath, MailHandler hdlr, void *userObj, NotNullPtr<IO::LogTool> log)
{
	this->hdlr = hdlr;
	this->hdlrObj = userObj;
	this->log = log;

	this->AddService(notifyPath, Net::WebUtil::RequestMethod::HTTP_POST, NotifyFunc);
	this->AddService(batchUplPath, Net::WebUtil::RequestMethod::HTTP_POST, BatchUplFunc);
}

Net::WebServer::GCISNotifyHandler::~GCISNotifyHandler()
{
}
