#include "Stdafx.h"
#include "Net/WebServer/GCISNotifyHandler.h"
#include "Text/JSONBuilder.h"
#include "Text/MailCreator.h"

#include <stdio.h>

Bool __stdcall Net::WebServer::GCISNotifyHandler::NotifyFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> hdlr)
{
	NN<GCISNotifyHandler> me = NN<GCISNotifyHandler>::ConvertFrom(hdlr);
	NN<Crypto::Cert::X509Cert> cert;
	Text::StringBuilderUTF8 sb2;
	Text::JSONBuilder builder(Text::JSONBuilder::OT_OBJECT);
	if (!req->GetClientCert().SetTo(cert))
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
		UnsafeArray<const UInt8> content = req->GetReqData(dataSize).Or((const UInt8*)&dataSize);
		Optional<Text::JSONBase> optjson = 0;
		NN<Text::JSONBase> json;
		Text::CStringNN resultCd = CSTR("0000");
		Text::CStringNN resultMsg = CSTR("Request processed successfully.");
		if (!Text::JSONBase::ParseJSONBytes(content, dataSize).SetTo(json))
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
			optjson = json;
		}
		else
		{
			optjson = json;
			Text::MailCreator mail;
			cert->GetSubjectCN(sb2);
			mail.SetFrom(0, sb2.ToCString());
			NN<Text::JSONObject> msgObj = NN<Text::JSONObject>::ConvertFrom(json);
			NN<Text::String> s;
			if (msgObj->GetObjectString(CSTR("ChanType")).SetTo(s) && !s->Equals(UTF8STRC("EM")) && !s->Equals(UTF8STRC("BD")))
			{
				failed = true;
				resultCd = CSTR("0074");
				resultMsg = CSTR("Invalid Channel Type.");
				me->log->LogMessage(CSTR("Invalid Channel Type"), IO::LogHandler::LogLevel::Error);
			}
			if (!failed)
			{
				NN<Text::JSONArray> recipientDetail;
				if (!msgObj->GetObjectArray(CSTR("RecipientDetail")).SetTo(recipientDetail))
				{
					failed = true;
					resultCd = CSTR("0032");
					resultMsg = CSTR("No recipient found.");
					me->log->LogMessage(CSTR("No recipient found 1"), IO::LogHandler::LogLevel::Error);
				}
				else
				{
					NN<Text::JSONObject> recipient;
					UOSInt i = 0;
					UOSInt j = recipientDetail->GetArrayLength();
					while (i < j)
					{
						if (!recipientDetail->GetArrayObject(i).SetTo(recipient))
						{
							failed = true;
							resultCd = CSTR("0032");
							resultMsg = CSTR("No recipient found.");
							me->log->LogMessage(CSTR("No recipient found 2"), IO::LogHandler::LogLevel::Error);
							break;
						}
						else if (recipient->GetObjectString(CSTR("ChanAddr")).SetTo(s))
						{
							mail.ToAdd(0, s);
						}
						else if (recipient->GetObjectString(CSTR("CcAddr")).SetTo(s))
						{
							mail.CCAdd(0, s);
						}
						else if (recipient->GetObjectString(CSTR("BccAddr")).SetTo(s))
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
				NN<Text::JSONObject> contentDetail;
				NN<Text::String> content;
				if (!msgObj->GetObjectObject(CSTR("ContentDetail")).SetTo(contentDetail))
				{
					failed = true;
					resultCd = CSTR("0024");
					resultMsg = CSTR("Content field is empty.");
					me->log->LogMessage(CSTR("Content field is empty"), IO::LogHandler::LogLevel::Error);
				}
				else if (contentDetail->GetObjectString(CSTR("ContentType")).SetTo(s))
				{
					if (!contentDetail->GetObjectString(CSTR("Content")).SetTo(content))
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
					if (contentDetail->GetObjectString(CSTR("Subject")).SetTo(s))
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
				NN<Text::MIMEObj::MailMessage> msg = mail.CreateMail();
				me->hdlr(me->hdlrObj, req->GetNetConn(), msg);
				msg.Delete();
				me->log->LogMessage(CSTR("Message created"), IO::LogHandler::LogLevel::Action);
			}
			//////////////////////////////
		}

		builder.ObjectAddStr(CSTR("Status"), failed?CSTR("F"):CSTR("S"));
		builder.ObjectBeginObject(CSTR("NotificationStatus"));
		if (optjson.SetTo(json))
		{
			if (json->GetType() == Text::JSONType::Object)
				builder.ObjectAdd(NN<Text::JSONObject>::ConvertFrom(json));
			json->EndUse();
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
	return me->ResponseJSONStr(req, resp, 0, builder.Build());
}

Bool __stdcall Net::WebServer::GCISNotifyHandler::BatchUplFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr)
{
	NN<GCISNotifyHandler> me = NN<GCISNotifyHandler>::ConvertFrom(svcHdlr);
	UOSInt size;
	UnsafeArray<const UInt8> data = req->GetReqData(size).Or((const UInt8*)&size);
	Text::StringBuilderUTF8 sb;
	sb.AppendC(data, size);
	printf("%s\r\n", sb.v.Ptr());

	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	Text::JSONBuilder builder(Text::JSONBuilder::OT_OBJECT);
	sptr = Text::StrInt64(sbuff, Data::DateTimeUtil::GetCurrTimeMillis());
	builder.ObjectAddStr(CSTR("UploadRefNum"), CSTRP(sbuff, sptr));
	builder.ObjectAddStr(CSTR("ResultCd"), CSTR("0000"));
	return me->ResponseJSONStr(req, resp, 0, builder.Build());
}

Net::WebServer::GCISNotifyHandler::GCISNotifyHandler(Text::CStringNN notifyPath, Text::CStringNN batchUplPath, MailHandler hdlr, AnyType userObj, NN<IO::LogTool> log)
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
