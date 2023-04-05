#include "Stdafx.h"
#include "Net/WebServer/GCISNotifyHandler.h"
#include "Text/JSONBuilder.h"
#include "Text/MailCreator.h"

Bool __stdcall Net::WebServer::GCISNotifyHandler::NotifyFunc(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, WebServiceHandler *hdlr)
{
	GCISNotifyHandler *me = (GCISNotifyHandler*)hdlr;
	Crypto::Cert::X509Cert *cert = req->GetClientCert();
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sb2;
	{
		Text::JSONBuilder builder(&sb, Text::JSONBuilder::OT_OBJECT);
		if (cert == 0)
		{
			resp->SetStatusCode(Net::WebStatus::SC_INTERNAL_SERVER_ERROR);
			builder.ObjectAddStr(CSTR("description"), CSTR("Client Cert is missing"));
			builder.ObjectAddStr(CSTR("code"), CSTR("9999"));
		}
		else
		{
			Bool failed = false;
			UOSInt dataSize;
			const UInt8 *content = req->GetReqData(&dataSize);
			Text::JSONBase *json = Text::JSONBase::ParseJSONBytes(content, dataSize);
			Text::CString resultCd = CSTR("0000");
			Text::CString resultMsg = CSTR("Request processed successfully.");
			if (json == 0)
			{
				failed = true;
				resultCd = CSTR("0021");
				resultMsg = CSTR("Invalid content type");
			}
			else if (json->GetType() != Text::JSONType::Object)
			{
				failed = true;
				resultCd = CSTR("0021");
				resultMsg = CSTR("Invalid content type");
			}
			else
			{
				Text::MailCreator mail;
				cert->GetSubjectCN(&sb2);
				mail.SetFrom(0, sb2.ToCString());
				Text::JSONObject *msgObj = (Text::JSONObject*)json;
				Text::String *s;
				if ((s = msgObj->GetObjectString(CSTR("ChanType"))) != 0 && !s->Equals(UTF8STRC("EM")) && !s->Equals(UTF8STRC("BD")))
				{
					failed = true;
					resultCd = CSTR("0074");
					resultMsg = CSTR("Invalid Channel Type.");
				}
				if (!failed)
				{
					Text::JSONArray *recipientDetail = msgObj->GetObjectArray(CSTR("RecipientDetail"));
					if (recipientDetail == 0)
					{
						failed = true;
						resultCd = CSTR("0032");
						resultMsg = CSTR("No recipient found.");
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
								break;
							}
							else if ((s = recipient->GetObjectString(CSTR("ChanAddr"))) != 0)
							{
								mail.ToAdd(0, s);
							}
							else if ((s = recipient->GetObjectString(CSTR("CcAddr"))) != 0)
							{
								mail.CCAdd(0, s);
							}
							else if ((s = recipient->GetObjectString(CSTR("BccAddr"))) != 0)
							{
//								mail.BccAdd(0, s);
							}
							else
							{
								failed = true;
								resultCd = CSTR("0032");
								resultMsg = CSTR("No recipient found.");
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
					}
					else if ((s = contentDetail->GetObjectString(CSTR("ContentType"))) != 0)
					{
						content = contentDetail->GetObjectString(CSTR("Content"));
						if (content == 0)
						{
							failed = true;
							resultCd = CSTR("0024");
							resultMsg = CSTR("Content field is empty.");
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
						}
					}
					else
					{
						failed = true;
						resultCd = CSTR("0021");
						resultMsg = CSTR("Invalid content type.");
					}
					if (!failed)
					{
						if ((s = contentDetail->GetObjectString(CSTR("Subject"))) != 0)
						{
							mail.SetSubject(s);
						}
						else
						{
							failed = true;
							resultCd = CSTR("0021");
							resultMsg = CSTR("Invalid content type.");
						}
					}
				}
				if (!failed)
				{
					Text::MIMEObj::MailMessage *msg = mail.CreateMail();
					me->hdlr(me->hdlrObj, req->GetNetConn(), msg);
					DEL_CLASS(msg);
				}
				//////////////////////////////
			}

			builder.ObjectAddStr(CSTR("Status"), failed?CSTR("F"):CSTR("S"));
			builder.ObjectBeginObject(CSTR("NotificationStatus"));
			if (json)
			{
				if (json->GetType() == Text::JSONType::Object)
					builder.ObjectAdd((Text::JSONObject*)json);
				json->EndUse();
			}
			if (!failed)
			{
				sb2.ClearStr();
				cert->GetSubjectCN(&sb2);
				builder.ObjectAddStr(CSTR("Client"), sb2.ToCString());

				builder.ObjectAddStr(CSTR("ResultMesg"), resultMsg);
				builder.ObjectAddStr(CSTR("ResultCd"), resultCd);
			}
		}
	}
	resp->AddDefHeaders(req);
	resp->AddCacheControl(0);
	resp->AddContentType(CSTR("application/json"));
	resp->AddContentLength(sb.GetLength());
	resp->Write(sb.v, sb.leng);
	return true;
}

Net::WebServer::GCISNotifyHandler::GCISNotifyHandler(Text::CString notifyPath, MailHandler hdlr, void *userObj)
{
	this->hdlr = hdlr;
	this->hdlrObj = userObj;

	this->AddService(notifyPath, Net::WebUtil::RequestMethod::HTTP_POST, NotifyFunc);
}

Net::WebServer::GCISNotifyHandler::~GCISNotifyHandler()
{
}
