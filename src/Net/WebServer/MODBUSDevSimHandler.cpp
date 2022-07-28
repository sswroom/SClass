#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/WebServer/MODBUSDevSimHandler.h"
#include "Net/WebServer/HTTPServerUtil.h"
#include "Text/XML.h"

Bool Net::WebServer::MODBUSDevSimHandler::ProcessRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq)
{
	if (subReq.Equals(UTF8STRC("/device")))
	{
		if (this->dev)
		{
			Text::String *s;
			if (req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_POST)
			{
				UInt16 toggleIndex;
				req->ParseHTTPForm();
				if ((s = req->GetHTTPFormStr(CSTR("action"))) != 0 && s->Equals(UTF8STRC("toggle")) && req->GetHTTPFormUInt16(CSTR("index"), &toggleIndex))
				{
					this->dev->ToggleValue(toggleIndex);
				}
			}
			Text::StringBuilderUTF8 sb;
			Text::StringBuilderUTF8 sb2;
			sb.AppendC(UTF8STRC("<html>"));
			sb.AppendC(UTF8STRC("<head><title>MODBUS Device Simulator - "));
			sb.Append(this->dev->GetName());
			sb.AppendC(UTF8STRC("</title>"));
			sb.AppendC(UTF8STRC("<script type=\"application/javascript\">\r\n"));
			sb.AppendC(UTF8STRC("function submitToggle(i){document.forms[0].index.value=i;document.forms[0].submit();}\r\n"));
			sb.AppendC(UTF8STRC("</script></head>\r\n"));
			sb.AppendC(UTF8STRC("<body><form method=\"POST\" action=\"device\">"));
			sb.AppendC(UTF8STRC("<input name=\"action\" type=\"hidden\" value=\"toggle\"/>"));
			sb.AppendC(UTF8STRC("<input name=\"index\" type=\"hidden\"/>"));
			sb.AppendC(UTF8STRC("</form>\r\n"));
			sb.AppendC(UTF8STRC("<table border=\"1\"><tr><td>Name</td><td>Value</td><td>Action</td></tr>\r\n"));
			UOSInt i = 0;
			UOSInt j = this->dev->GetValueCount();
			while (i < j)
			{
				sb.AppendC(UTF8STRC("<tr><td>"));
				s = Text::XML::ToNewHTMLText(this->dev->GetValueName(i).v);
				sb.Append(s);
				s->Release();
				sb.AppendC(UTF8STRC("</td><td>"));
				sb2.ClearStr();
				this->dev->GetValue(i, &sb2);
				s = Text::XML::ToNewHTMLText(sb2.ToString());
				sb.Append(s);
				s->Release();
				sb.AppendC(UTF8STRC("</td><td>"));
				sb.AppendC(UTF8STRC("<input type=\"button\" onclick=\"submitToggle("));
				sb.AppendUOSInt(i);
				sb.AppendC(UTF8STRC(");\" value=\"Toggle\" /></td></tr>\r\n"));

				i++;
			}
			sb.AppendC(UTF8STRC("</table></body></html>"));

			Text::CString mime = CSTR("text/html");
			resp->AddDefHeaders(req);
			resp->AddContentType(mime);
			Net::WebServer::HTTPServerUtil::SendContent(req, resp, mime, sb.GetLength(), sb.ToString());
			return true;

		}
		else
		{
			resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
			return true;
		}
	}

	return this->DoRequest(req, resp, subReq);
}

Net::WebServer::MODBUSDevSimHandler::MODBUSDevSimHandler(IO::MODBUSDevSim *dev)
{
	this->dev = dev;
}

Net::WebServer::MODBUSDevSimHandler::~MODBUSDevSimHandler()
{
}
