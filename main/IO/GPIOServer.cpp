#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/GPIOControl.h"
#include "Manage/CPUInfo.h"
#include "Net/OSSocketFactory.h"
#include "Net/WebServer/WebListener.h"
#include "Net/WebServer/WebStandardHandler.h"
#include "Sync/Thread.h"

#define PORT 6080

class GPIOWebHandler : public Net::WebServer::WebStandardHandler
{
private:
	IO::GPIOControl *gpio;

public:
	GPIOWebHandler(IO::GPIOControl *gpio)
	{
		this->gpio = gpio;
	}

	virtual ~GPIOWebHandler()
	{

	}

	virtual Bool ProcessRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, UOSInt subReqLen)
	{
		if (this->DoRequest(req, resp, subReq, subReqLen))
		{
			return true;
		}

		UInt32 pin;
		UInt32 outVal;
		if (req->GetQueryValueU32(UTF8STRC("pin"), &pin))
		{
			if (req->GetQueryValueU32(UTF8STRC("output"), &outVal))
			{
				this->gpio->SetPinState(pin, outVal != 0);
			}
		}

		Text::StringBuilderUTF8 sb;
		UTF8Char sbuff[256];
		UTF8Char *sptr;
		UOSInt i;
		UOSInt j;
		
		sb.AppendC(UTF8STRC("<html><head><title>GPIO Status</title></head>\r\n"));
		sb.AppendC(UTF8STRC("<body>"));

		Manage::CPUInfo cpu;
		if ((sptr = cpu.GetCPUName(sbuff)) != 0)
		{
			sb.AppendC(UTF8STRC("<h2>CPU: "));
			sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
			sb.AppendC(UTF8STRC("</h2>\r\n"));
		}
		if (this->gpio->IsError())
		{
			sb.AppendC(UTF8STRC("<h2 color=\"red\">GPIO Error</h2>\r\n"));
		}
		else
		{
			sb.AppendC(UTF8STRC("<table border=\"1\">\r\n"));
			sb.AppendC(UTF8STRC("<tr><td>Pin Num</td><td>Status</td><td>Direction</td><td>Mode</td><td>Action</td></tr>\r\n"));
			i = 0;
			j = this->gpio->GetPinCount();
			while (i < j)
			{
				sb.AppendC(UTF8STRC("<tr><td>Pin "));
				sb.AppendUOSInt(i);
				sb.AppendC(UTF8STRC("</td>"));
				if (this->gpio->IsPinHigh(i))
				{
					sb.AppendC(UTF8STRC("<td style=\"background-color: #40FF40\">1</td>"));
				}
				else
				{
					sb.AppendC(UTF8STRC("<td style=\"background-color: #FF4040\">0</td>"));
				}
				if (this->gpio->IsPinOutput(i))
				{
					sb.AppendC(UTF8STRC("<td>Output</td>"));
					outVal = 1;
				}
				else
				{
					sb.AppendC(UTF8STRC("<td>Input</td>"));
					outVal = 0;
				}
				sb.AppendC(UTF8STRC("<td>"));
				sb.Append(IO::GPIOControl::PinModeGetName(i, this->gpio->GetPinMode(i)));
				sb.AppendC(UTF8STRC("</td><td>"));
				if (outVal == 1)
				{
					sb.AppendC(UTF8STRC("<a href=\"/?pin="));
					sb.AppendUOSInt(i);
					sb.AppendC(UTF8STRC("&output=1\">High</a>"));

					sb.AppendC(UTF8STRC(" <a href=\"/?pin="));
					sb.AppendUOSInt(i);
					sb.AppendC(UTF8STRC("&output=0\">Low</a>"));
				}
				sb.AppendC(UTF8STRC("</td></tr>"));
				
				i++;
			}
			sb.AppendC(UTF8STRC("</table>\r\n"));
		}
		sb.AppendC(UTF8STRC("</body></html>"));

		resp->AddDefHeaders(req);
		resp->AddContentLength(sb.GetLength());
		resp->AddContentType(UTF8STRC("text/html"));
		resp->AddCacheControl(0);
		resp->Write(sb.ToString(), sb.GetLength());
		return true;
	}
};

Int32 MyMain(Core::IProgControl *progCtrl)
{
	Net::SocketFactory *sockf;
	Net::WebServer::WebListener *listener;
	GPIOWebHandler *webHdlr;
	IO::GPIOControl *gpio;
	NEW_CLASS(sockf, Net::OSSocketFactory(false));
	NEW_CLASS(gpio, IO::GPIOControl());
	NEW_CLASS(webHdlr, GPIOWebHandler(gpio));
	NEW_CLASS(listener, Net::WebServer::WebListener(sockf, 0, webHdlr, PORT, 120, Sync::Thread::GetThreadCnt(), CSTR("GPIO/1.0"), false, true));
	progCtrl->WaitForExit(progCtrl);
	DEL_CLASS(listener);
	webHdlr->Release();
	DEL_CLASS(gpio);
	DEL_CLASS(sockf);
	return 0;
}