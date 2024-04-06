#ifndef _SM_NET_WEBSERVER_CAPTURERWEBHANDLER
#define _SM_NET_WEBSERVER_CAPTURERWEBHANDLER
#include "IO/BTCapturer.h"
#include "IO/RadioSignalLogger.h"
#include "Net/WiFiCapturer.h"
#include "Net/WebServer/WebServiceHandler.h"
#include "Text/StringBuilderUTF8.h"

namespace Net
{
	namespace WebServer
	{
		class CapturerWebHandler : public Net::WebServer::WebServiceHandler
		{
		private:
			IO::BTCapturer *btCapture;
			Net::WiFiCapturer *wifiCapture;
			IO::RadioSignalLogger *radioLogger;

			static Bool __stdcall IndexFunc(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NotNullPtr<WebServiceHandler> svc);
			static Bool __stdcall BTCurrentFunc(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NotNullPtr<WebServiceHandler> svc);
			static Bool __stdcall BTDetailFunc(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NotNullPtr<WebServiceHandler> svc);
			static Bool __stdcall BTDetailPubFunc(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NotNullPtr<WebServiceHandler> svc);
			static Bool __stdcall WiFiCurrentFunc(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NotNullPtr<WebServiceHandler> svc);
			static Bool __stdcall WiFiDetailFunc(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NotNullPtr<WebServiceHandler> svc);
			static Bool __stdcall WiFiDownloadFunc(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NotNullPtr<WebServiceHandler> svc);

			static void AppendWiFiTable(NotNullPtr<Text::StringBuilderUTF8> sb, NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Data::ArrayList<Net::WiFiLogFile::LogFileEntry*>> entryList, const Data::Timestamp &scanTime);
			static void AppendBTTable(NotNullPtr<Text::StringBuilderUTF8> sb, NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<const Data::ReadingList<IO::BTScanLog::ScanRecord3*>> entryList, Bool inRangeOnly);
			static OSInt __stdcall WiFiLogRSSICompare(Net::WiFiLogFile::LogFileEntry *obj1, Net::WiFiLogFile::LogFileEntry *obj2);
			static OSInt __stdcall BTLogRSSICompare(IO::BTScanLog::ScanRecord3 *obj1, IO::BTScanLog::ScanRecord3 *obj2);
		public:
			CapturerWebHandler(Net::WiFiCapturer *wifiCapture, IO::BTCapturer *btCapture, IO::RadioSignalLogger *radioLogger);
			virtual ~CapturerWebHandler();
		};
	}
}
#endif
