#ifndef _SM_SSWR_SMONITOR_SMONITORWEBHANDLER
#define _SM_SSWR_SMONITOR_SMONITORWEBHANDLER
#include "Data/FastStringMap.hpp"
#include "IO/Writer.h"
#include "Net/WebServer/MemoryWebSessionManager.h"
#include "Net/WebServer/WebStandardHandler.h"
#include "SSWR/SMonitor/SMonitorCore.h"

namespace SSWR
{
	namespace SMonitor
	{
		class SMonitorWebHandler : public Net::WebServer::WebStandardHandler
		{
		private:
			typedef Bool (CALLBACKFUNC RequestHandler)(NN<SSWR::SMonitor::SMonitorWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp);
		private:
			SSWR::SMonitor::SMonitorCore *core;
			Data::FastStringMap<RequestHandler> *reqMap;
			Net::WebServer::MemoryWebSessionManager *sessMgr;

			static Bool __stdcall OnSessDeleted(NN<Net::WebServer::WebSession> sess, AnyType userObj);
			static Bool __stdcall OnSessCheck(NN<Net::WebServer::WebSession> sess, AnyType userObj);

			static Bool __stdcall DefaultReq(NN<SSWR::SMonitor::SMonitorWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp);
			static Bool __stdcall IndexReq(NN<SSWR::SMonitor::SMonitorWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp);
			static Bool __stdcall LoginReq(NN<SSWR::SMonitor::SMonitorWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp);
			static Bool __stdcall LogoutReq(NN<SSWR::SMonitor::SMonitorWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp);
			static Bool __stdcall DeviceReq(NN<SSWR::SMonitor::SMonitorWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp);
			static Bool __stdcall DeviceEditReq(NN<SSWR::SMonitor::SMonitorWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp);
			static Bool __stdcall DeviceReadingReq(NN<SSWR::SMonitor::SMonitorWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp);
			static Bool __stdcall DeviceDigitalsReq(NN<SSWR::SMonitor::SMonitorWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp);
			static Bool __stdcall DeviceReadingImgReq(NN<SSWR::SMonitor::SMonitorWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp);
			static Bool __stdcall DevicePastDataReq(NN<SSWR::SMonitor::SMonitorWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp);
			static Bool __stdcall DevicePastDataImgReq(NN<SSWR::SMonitor::SMonitorWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp);
			static Bool __stdcall UserPasswordReq(NN<SSWR::SMonitor::SMonitorWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp);
			static Bool __stdcall UsersReq(NN<SSWR::SMonitor::SMonitorWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp);
			static Bool __stdcall UserAddReq(NN<SSWR::SMonitor::SMonitorWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp);
			static Bool __stdcall UserAssignReq(NN<SSWR::SMonitor::SMonitorWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp);

			static void __stdcall WriteHeaderBegin(IO::Writer *writer);
			static void __stdcall WriteHeaderEnd(IO::Writer *writer);
			static void __stdcall WriteMenu(IO::Writer *writer, Optional<Net::WebServer::WebSession> sess);
			static void __stdcall WriteHTMLText(IO::Writer *writer, UnsafeArray<const UTF8Char> txt);
			static void __stdcall WriteHTMLText(IO::Writer *writer, NN<Text::String> txt);
			static void __stdcall WriteHTMLText(IO::Writer *writer, Text::CString txt);
			static void __stdcall WriteAttrText(IO::Writer *writer, UnsafeArray<const UTF8Char> txt);
			static void __stdcall WriteAttrText(IO::Writer *writer, Text::String *txt);
			static void __stdcall WriteAttrText(IO::Writer *writer, NN<Text::String> txt);
			static void __stdcall WriteJSText(IO::Writer *writer, UnsafeArray<const UTF8Char> txt);
			static void __stdcall WriteJSText(IO::Writer *writer, Text::String *txt);
			static void __stdcall WriteJSText(IO::Writer *writer, NN<Text::String> txt);

			virtual Bool ProcessRequest(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq);

		public:
			SMonitorWebHandler(SSWR::SMonitor::SMonitorCore *core);
			virtual ~SMonitorWebHandler();
		};
	}
}

#endif
