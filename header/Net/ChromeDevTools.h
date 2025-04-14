#ifndef _SM_NET_CHROMEDEVTOOLS
#define _SM_NET_CHROMEDEVTOOLS
#include "Net/SSLEngine.h"
#include "Net/JSONResponse.h"

namespace Net
{
	class ChromeDevTools
	{
	public:
		JSONRESP_BEGIN(ChromeVersion)
		JSONRESP_STR("Browser",false,false)
		JSONRESP_STR("Protocol-Version",false,false)
		JSONRESP_STR("User-Agent",false,false)
		JSONRESP_STR("V8-Version",false,false)
		JSONRESP_STR("WebKit-Version",false,false)
		JSONRESP_STR("webSocketDebuggerUrl",false,false)
		JSONRESP_SEC_GET(ChromeVersion)
		JSONRESP_GETSTR("Browser",GetBrowser)
		JSONRESP_GETSTR("Protocol-Version",GetProtocolVersion)
		JSONRESP_GETSTR("User-Agent",GetUserAgent)
		JSONRESP_GETSTR("V8-Version",GetV8Version)
		JSONRESP_GETSTR("WebKit-Version",GetWebKitVersion)
		JSONRESP_GETSTR("webSocketDebuggerUrl",GetWebSocketDebuggerUrl)
		JSONRESP_END

		JSONRESP_BEGIN(ChromeTarget)
		JSONRESP_STR("description",false,false)
		JSONRESP_STR("devtoolsFrontendUrl",false,false)
		JSONRESP_STR("faviconUrl",true,false)
		JSONRESP_STR("id",false,false)
		JSONRESP_STR("title",false,false)
		JSONRESP_STR("type",false,false)
		JSONRESP_STR("url",false,false)
		JSONRESP_STR("webSocketDebuggerUrl",false,false)
		JSONRESP_SEC_GET(ChromeTarget)
		JSONRESP_GETSTR("description",GetDescription)
		JSONRESP_GETSTR("devtoolsFrontendUrl",GetDevToolsFrontendUrl)
		JSONRESP_GETSTROPT("faviconUrl",GetFaviconUrl)
		JSONRESP_GETSTR("id",GetId)
		JSONRESP_GETSTR("title",GetTitle)
		JSONRESP_GETSTR("type",GetType)
		JSONRESP_GETSTR("url",GetURL)
		JSONRESP_GETSTR("webSocketDebuggerUrl",GetWebSocketDebuggerUrl)
		JSONRESP_END

		JSONRESP_BEGINARRAY(ChromeTargets, ChromeTarget)
		JSONRESP_ENDARRAY

	private:
		NN<Net::TCPClientFactory> clif;
		Optional<Net::SSLEngine> ssl;
		UInt16 port;

	public:
		ChromeDevTools(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, UInt16 port);
		~ChromeDevTools();

		Optional<ChromeVersion> GetVersion() const;
		Optional<ChromeTargets> GetTargets() const;
	};
}
#endif
