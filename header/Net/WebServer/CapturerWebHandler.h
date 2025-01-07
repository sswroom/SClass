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
			Optional<IO::BTCapturer> btCapture;
			Optional<Net::WiFiCapturer> wifiCapture;
			IO::RadioSignalLogger *radioLogger;

			static Bool __stdcall IndexFunc(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svc);
			static Bool __stdcall BTCurrentFunc(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svc);
			static Bool __stdcall BTDetailFunc(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svc);
			static Bool __stdcall BTDetailPubFunc(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svc);
			static Bool __stdcall WiFiCurrentFunc(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svc);
			static Bool __stdcall WiFiDetailFunc(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svc);
			static Bool __stdcall WiFiDownloadFunc(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svc);

			static void AppendWiFiTable(NN<Text::StringBuilderUTF8> sb, NN<Net::WebServer::IWebRequest> req, NN<Data::ArrayListNN<Net::WiFiLogFile::LogFileEntry>> entryList, const Data::Timestamp &scanTime);
			static void AppendBTTable(NN<Text::StringBuilderUTF8> sb, NN<Net::WebServer::IWebRequest> req, NN<const Data::ReadingListNN<IO::BTScanLog::ScanRecord3>> entryList, Bool inRangeOnly);
			static OSInt __stdcall WiFiLogRSSICompare(NN<Net::WiFiLogFile::LogFileEntry> obj1, NN<Net::WiFiLogFile::LogFileEntry> obj2);
			static OSInt __stdcall BTLogRSSICompare(NN<IO::BTScanLog::ScanRecord3> obj1, NN<IO::BTScanLog::ScanRecord3> obj2);
		public:
			CapturerWebHandler(Optional<Net::WiFiCapturer> wifiCapture, Optional<IO::BTCapturer> btCapture, IO::RadioSignalLogger *radioLogger);
			virtual ~CapturerWebHandler();
		};
	}
}
#endif
