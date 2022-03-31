#ifndef _SM_SSWR_SMONITOR_SMONITORWEBHANDLER
#define _SM_SSWR_SMONITOR_SMONITORWEBHANDLER
#include "Data/FastStringMap.h"
#include "IO/Writer.h"
#include "Net/WebServer/MemoryWebSessionManager.h"
#include "Net/WebServer/WebStandardHandler.h"
#include "SSWR/SMonitor/ISMonitorCore.h"

namespace SSWR
{
	namespace SMonitor
	{
		class SMonitorWebHandler : public Net::WebServer::WebStandardHandler
		{
		private:
			typedef Bool (__stdcall *RequestHandler)(SSWR::SMonitor::SMonitorWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp);
		private:
			SSWR::SMonitor::ISMonitorCore *core;
			Data::FastStringMap<RequestHandler> *reqMap;
			Net::WebServer::MemoryWebSessionManager *sessMgr;

			static Bool __stdcall OnSessDeleted(Net::WebServer::IWebSession* sess, void *userObj);
			static Bool __stdcall OnSessCheck(Net::WebServer::IWebSession* sess, void *userObj);

			static Bool __stdcall DefaultReq(SSWR::SMonitor::SMonitorWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp);
			static Bool __stdcall IndexReq(SSWR::SMonitor::SMonitorWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp);
			static Bool __stdcall LoginReq(SSWR::SMonitor::SMonitorWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp);
			static Bool __stdcall LogoutReq(SSWR::SMonitor::SMonitorWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp);
			static Bool __stdcall DeviceReq(SSWR::SMonitor::SMonitorWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp);
			static Bool __stdcall DeviceEditReq(SSWR::SMonitor::SMonitorWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp);
			static Bool __stdcall DeviceReadingReq(SSWR::SMonitor::SMonitorWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp);
			static Bool __stdcall DeviceDigitalsReq(SSWR::SMonitor::SMonitorWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp);
			static Bool __stdcall DeviceReadingImgReq(SSWR::SMonitor::SMonitorWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp);
			static Bool __stdcall DevicePastDataReq(SSWR::SMonitor::SMonitorWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp);
			static Bool __stdcall DevicePastDataImgReq(SSWR::SMonitor::SMonitorWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp);
			static Bool __stdcall UserPasswordReq(SSWR::SMonitor::SMonitorWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp);
			static Bool __stdcall UsersReq(SSWR::SMonitor::SMonitorWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp);
			static Bool __stdcall UserAddReq(SSWR::SMonitor::SMonitorWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp);
			static Bool __stdcall UserAssignReq(SSWR::SMonitor::SMonitorWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp);

			static void __stdcall WriteHeaderBegin(IO::Writer *writer);
			static void __stdcall WriteHeaderEnd(IO::Writer *writer);
			static void __stdcall WriteMenu(IO::Writer *writer, Net::WebServer::IWebSession *sess);
			static void __stdcall WriteHTMLText(IO::Writer *writer, const UTF8Char *txt);
			static void __stdcall WriteHTMLText(IO::Writer *writer, Text::String *txt);
			static void __stdcall WriteAttrText(IO::Writer *writer, const UTF8Char *txt);
			static void __stdcall WriteAttrText(IO::Writer *writer, Text::String *txt);
			static void __stdcall WriteJSText(IO::Writer *writer, const UTF8Char *txt);
			static void __stdcall WriteJSText(IO::Writer *writer, Text::String *txt);

			virtual Bool ProcessRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq);

		public:
			SMonitorWebHandler(SSWR::SMonitor::ISMonitorCore *core);
			virtual ~SMonitorWebHandler();
		};
	}
}

#endif
