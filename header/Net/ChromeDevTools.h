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

		JSONRESP_BEGIN(ChromeProtocolVersion)
		JSONRESP_STR("major",false,false)
		JSONRESP_STR("minor",false,false)
		JSONRESP_SEC_GET(ChromeProtocolVersion)
		JSONRESP_GETSTR("major",GetMajor)
		JSONRESP_GETSTR("minor",GetMinor)
		JSONRESP_END

		JSONRESP_BEGIN(ChromeReturnItem)
		JSONRESP_STR("$ref",true,false)
		JSONRESP_STR("type",true,false)
		JSONRESP_SEC_GET(ChromeReturnItem)
		JSONRESP_GETSTROPT("$ref",GetRef)
		JSONRESP_GETSTROPT("type",GetType)
		JSONRESP_END

		JSONRESP_BEGIN(ChromeParameter)
		JSONRESP_STR("$ref",true,false)
		JSONRESP_STR("description",true,false)
		JSONRESP_ARRAY_STR("enum",true,false)
		JSONRESP_BOOL("experimental",true,false)
		JSONRESP_STR("name",false,false)
		JSONRESP_BOOL("optional",true,false)
		JSONRESP_STR("type",true,false)
		JSONRESP_BOOL("deprecated",true,false)
		JSONRESP_OBJ("items",true,false,ChromeReturnItem)
		JSONRESP_SEC_GET(ChromeParameter)
		JSONRESP_GETSTROPT("$ref",GetRef)
		JSONRESP_GETSTROPT("description",GetDescription)
		JSONRESP_GETARRAY_STR("enum",GetEnum)
		JSONRESP_GETBOOL("experimental",IsExperimental)
		JSONRESP_GETSTR("name",GetName)
		JSONRESP_GETBOOL("optional",IsOptional)
		JSONRESP_GETSTROPT("type",GetType)
		JSONRESP_GETBOOL("deprecated",IsDeprecated)
		JSONRESP_GETOBJ("items",GetItems,ChromeReturnItem)
		JSONRESP_END

		JSONRESP_BEGIN(ChromeCommand)
		JSONRESP_STR("description",true,false)
		JSONRESP_STR("name",false,false)
		JSONRESP_ARRAY_OBJ("returns",true,false,ChromeParameter)
		JSONRESP_BOOL("experimental",true,false)
		JSONRESP_BOOL("deprecated",true,false)
		JSONRESP_ARRAY_OBJ("parameters",true,false,ChromeParameter)
		JSONRESP_STR("redirect",true,false)
		JSONRESP_SEC_GET(ChromeCommand)
		JSONRESP_GETSTROPT("description",GetDescription)
		JSONRESP_GETSTR("name",GetName)
		JSONRESP_GETARRAY_OBJ("returns",GetReturns,ChromeParameter)
		JSONRESP_GETBOOL("experimental",IsExperimental)
		JSONRESP_GETBOOL("deprecated",IsDeprecated)
		JSONRESP_GETARRAY_OBJ("parameters",GetParameters,ChromeParameter)
		JSONRESP_GETSTROPT("redirect",GetRedirect)
		JSONRESP_END

		JSONRESP_BEGIN(ChromeEvent)
		JSONRESP_STR("description",true,false)
		JSONRESP_STR("name",false,false)
		JSONRESP_BOOL("experimental",true,false)
		JSONRESP_BOOL("deprecated",true,false)
		JSONRESP_ARRAY_OBJ("parameters",true,false,ChromeParameter)
		JSONRESP_SEC_GET(ChromeEvent)
		JSONRESP_GETSTR("description",GetDescription)
		JSONRESP_GETSTR("name",GetName)
		JSONRESP_GETBOOL("experimental",IsExperimental)
		JSONRESP_GETBOOL("deprecated",IsDeprecated)
		JSONRESP_GETARRAY_OBJ("parameters",GetParameters,ChromeParameter)
		JSONRESP_END

		JSONRESP_BEGIN(ChromeType)
		JSONRESP_STR("description",true,false)
		JSONRESP_BOOL("experimental",true,false)
		JSONRESP_STR("id",false,false)
		JSONRESP_ARRAY_OBJ("properties",true,false,ChromeParameter)
		JSONRESP_STR("type",false,false)
		JSONRESP_BOOL("deprecated",true,false)
		JSONRESP_ARRAY_STR("enum",true,false)
		JSONRESP_OBJ("items",true,false,ChromeReturnItem)
		JSONRESP_SEC_GET(ChromeType)
		JSONRESP_GETSTROPT("description",GetDescription)
		JSONRESP_GETBOOL("experimental",IsExperimental)
		JSONRESP_GETSTR("id",GetId)
		JSONRESP_GETARRAY_OBJ("properties",GetProperties,ChromeParameter)
		JSONRESP_GETSTR("type",GetType)
		JSONRESP_GETBOOL("deprecated",IsDeprecated)
		JSONRESP_GETARRAY_STR("enum",GetEnum)
		JSONRESP_GETOBJ("items",GetItems,ChromeReturnItem)
		JSONRESP_END

		JSONRESP_BEGIN(ChromeDomain)
		JSONRESP_STR("domain",false,false)
		JSONRESP_STR("description",true,false)
		JSONRESP_BOOL("experimental",true,false)
		JSONRESP_ARRAY_OBJ("commands",false,false,ChromeCommand)
		JSONRESP_ARRAY_STR("dependencies",true,false)
		JSONRESP_ARRAY_OBJ("events",true,false,ChromeEvent)
		JSONRESP_ARRAY_OBJ("types",true,false,ChromeType)
		JSONRESP_BOOL("deprecated",true,false)
		JSONRESP_SEC_GET(ChromeDomain)
		JSONRESP_GETSTR("domain",GetDomain)
		JSONRESP_GETSTROPT("description",GetDescription)
		JSONRESP_GETBOOL("experimental",IsExperimental)
		JSONRESP_GETARRAY_OBJ("commands",GetCommands,ChromeCommand)
		JSONRESP_GETARRAY_STR("dependencies",GetDependencies)
		JSONRESP_GETARRAY_OBJ("events",GetEvents,ChromeEvent)
		JSONRESP_GETARRAY_OBJ("types",GetTypes,ChromeType)
		JSONRESP_GETBOOL("deprecated",IsDeprecated)
		JSONRESP_END

		JSONRESP_BEGIN(ChromeProtocol)
		JSONRESP_OBJ("version",false,false,ChromeProtocolVersion)
		JSONRESP_ARRAY_OBJ("domains",false,false,ChromeDomain)
		JSONRESP_SEC_GET(ChromeProtocol)
		JSONRESP_GETOBJ("version",GetVersion,ChromeProtocolVersion)
		JSONRESP_GETARRAY_OBJ("domains",GetDomains,ChromeDomain)
		JSONRESP_END

	private:
		NN<Net::TCPClientFactory> clif;
		Optional<Net::SSLEngine> ssl;
		UInt16 port;

	public:
		ChromeDevTools(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, UInt16 port);
		~ChromeDevTools();

		Optional<ChromeVersion> GetVersion() const;
		Optional<ChromeTargets> GetTargets() const;
		Optional<ChromeProtocol> GetProtocol() const;
	};
}
#endif
