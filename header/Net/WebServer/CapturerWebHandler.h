#ifndef _SM_NET_WEBSERVER_CAPTURERWEBHANDLER
#define _SM_NET_WEBSERVER_CAPTURERWEBHANDLER
#include "IO/BTCapturer.h"
#include "Net/WiFiCapturer.h"
#include "Net/WebServer/WebServiceHandler.h"
#include "Text/StringBuilderUTF.h"

namespace Net
{
	namespace WebServer
	{
		class CaptuererWebHandler : public Net::WebServer::WebServiceHandler
		{
		private:
			IO::BTCapturer *btCapture;
			Net::WiFiCapturer *wifiCapture;

			static Bool __stdcall IndexFunc(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, WebServiceHandler *svc);
			static Bool __stdcall BTCurrentFunc(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, WebServiceHandler *svc);
			static Bool __stdcall BTDetailFunc(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, WebServiceHandler *svc);
			static Bool __stdcall WiFiCurrentFunc(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, WebServiceHandler *svc);
			static Bool __stdcall WiFiDetailFunc(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, WebServiceHandler *svc);
			static Bool __stdcall WiFiDownloadFunc(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, WebServiceHandler *svc);

			static void AppendWiFiTable(Text::StringBuilderUTF *sb, Data::ArrayList<Net::WiFiLogFile::LogFileEntry*> *entryList, Int64 scanTime);
			static void AppendBTTable(Text::StringBuilderUTF *sb, Data::ArrayList<IO::ProgCtrl::BluetoothCtlProgCtrl::DeviceInfo*> *entryList, Bool inRangeOnly);
		public:
			CaptuererWebHandler(Net::WiFiCapturer *wifiCapture, IO::BTCapturer *btCapture);
			virtual ~CaptuererWebHandler();
		};
	}
}
#endif
