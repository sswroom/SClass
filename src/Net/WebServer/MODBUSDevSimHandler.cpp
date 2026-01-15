#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/WebServer/MODBUSDevSimHandler.h"
#include "Net/WebServer/HTTPServerUtil.h"
#include "Text/XML.h"

Bool Net::WebServer::MODBUSDevSimHandler::ProcessRequest(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq)
{
	if (subReq.Equals(UTF8STRC("/device")))
	{
		NN<IO::MODBUSDevSim> dev;
		if (this->dev.SetTo(dev))
		{
			NN<Text::String> s;
			if (req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_POST)
			{
				UInt32 delay;
				UInt16 toggleIndex;
				req->ParseHTTPForm();
				if (req->GetHTTPFormStr(CSTR("action")).SetTo(s))
				{
					if (s->Equals(UTF8STRC("toggle")) && req->GetHTTPFormUInt16(CSTR("index"), toggleIndex))
					{
						dev->ToggleValue(toggleIndex);
					}
					else if (s->Equals(UTF8STRC("delay")) && req->GetHTTPFormUInt32(CSTR("delay"), delay))
					{
						this->listener->SetDelay(delay);
					}
				}
			}
			Text::StringBuilderUTF8 sb;
			Text::StringBuilderUTF8 sb2;
			sb.AppendC(UTF8STRC("<html>"));
			sb.AppendC(UTF8STRC("<head><title>MODBUS Device Simulator - "));
			sb.Append(dev->GetName());
			sb.AppendC(UTF8STRC("</title>"));
			sb.AppendC(UTF8STRC("<script type=\"application/javascript\">\r\n"));
			sb.AppendC(UTF8STRC("function submitToggle(i){document.forms[0].index.value=i;document.forms[0].submit();}\r\n"));
			sb.AppendC(UTF8STRC("</script></head>\r\n"));
			sb.AppendC(UTF8STRC("<body><form method=\"POST\" action=\"device\">"));
			sb.AppendC(UTF8STRC("<input name=\"action\" type=\"hidden\" value=\"toggle\"/>"));
			sb.AppendC(UTF8STRC("<input name=\"index\" type=\"hidden\"/>"));
			sb.AppendC(UTF8STRC("</form>\r\n"));

			sb.AppendC(UTF8STRC("<body><form method=\"POST\" action=\"device\">"));
			sb.AppendC(UTF8STRC("<input name=\"action\" type=\"hidden\" value=\"delay\"/>"));
			sb.AppendC(UTF8STRC("Delay: <input name=\"delay\" type=\"text\" value=\""));
			sb.AppendU32(this->listener->GetDelay());
			sb.AppendC(UTF8STRC("\"/>"));
			sb.AppendC(UTF8STRC("<input type=\"submit\"/><br/>"));
			sb.AppendC(UTF8STRC("</form>\r\n"));

			sb.AppendC(UTF8STRC("<table border=\"1\"><tr><td>Name</td><td>Value</td><td>Action</td></tr>\r\n"));
			UIntOS i = 0;
			UIntOS j = dev->GetValueCount();
			while (i < j)
			{
				sb.AppendC(UTF8STRC("<tr><td>"));
				s = Text::XML::ToNewHTMLBodyText(dev->GetValueName(i).OrEmpty().v);
				sb.Append(s);
				s->Release();
				sb.AppendC(UTF8STRC("</td><td>"));
				sb2.ClearStr();
				dev->GetValue(i, sb2);
				s = Text::XML::ToNewHTMLBodyText(sb2.ToString());
				sb.Append(s);
				s->Release();
				sb.AppendC(UTF8STRC("</td><td>"));
				sb.AppendC(UTF8STRC("<input type=\"button\" onclick=\"submitToggle("));
				sb.AppendUIntOS(i);
				sb.AppendC(UTF8STRC(");\" value=\"Toggle\" /></td></tr>\r\n"));

				i++;
			}
			sb.AppendC(UTF8STRC("</table></body></html>"));

			Text::CStringNN mime = CSTR("text/html");
			this->AddResponseHeaders(req, resp);
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

Net::WebServer::MODBUSDevSimHandler::MODBUSDevSimHandler(NN<Net::MODBUSTCPListener> listener, Optional<IO::MODBUSDevSim> dev)
{
	this->listener = listener;
	this->dev = dev;
}

Net::WebServer::MODBUSDevSimHandler::~MODBUSDevSimHandler()
{
}
