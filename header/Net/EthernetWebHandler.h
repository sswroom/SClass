#ifndef _SM_NET_ETHERNETWEBHANDLER
#define _SM_NET_ETHERNETWEBHANDLER
#include "Data/FastStringMap.h"
#include "Net/EthernetAnalyzer.h"
#include "Net/WebServer/WebStandardHandler.h"

namespace Net
{
	class EthernetWebHandler : public Net::WebServer::WebStandardHandler
	{
	private:
		typedef Bool (__stdcall *RequestHandler)(EthernetWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp);

	private:
		Net::EthernetAnalyzer *analyzer;
		Data::FastStringMap<RequestHandler> *reqMap;

		static void AppendHeader(Text::StringBuilderUTF8 *sbOut);
		void AppendMenu(Text::StringBuilderUTF8 *sbOut);
		static void AppendFooter(Text::StringBuilderUTF8 *sbOut);

		static Bool __stdcall DeviceReq(EthernetWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp);
		static Bool __stdcall IPTransferReq(EthernetWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp);
		static Bool __stdcall DNSReqv4Req(EthernetWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp);
		static Bool __stdcall DNSReqv6Req(EthernetWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp);
		static Bool __stdcall DNSReqOthReq(EthernetWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp);
		static Bool __stdcall DNSTargetReq(EthernetWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp);
		static Bool __stdcall DNSClientReq(EthernetWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp);
		static Bool __stdcall DHCPReq(EthernetWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp);
		static Bool __stdcall IPLogReq(EthernetWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp);

	private:
		virtual Bool ProcessRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq);

	public:
		EthernetWebHandler(Net::EthernetAnalyzer *analyzer);
		virtual ~EthernetWebHandler();
	};
}
#endif
