#ifndef _SM_NET_WEBDRIVER
#define _SM_NET_WEBDRIVER
#include "IO/MemoryStream.h"
#include "Math/RectArea.h"
#include "Net/RESTResource.h"
#include "Sync/ThreadUtil.h"
#include "Text/JSONBuilder.h"

namespace Net
{
	class WebDriverParam : public Net::RESTParam
	{
	public:
		virtual ~WebDriverParam() {};

		virtual Text::JSONBuilder::ObjectType GetObjectType() const { return Text::JSONBuilder::OT_OBJECT; }
		virtual void BuildJSON(NN<Text::JSONBuilder> builder) const = 0;
		virtual void WriteParam(NN<Net::HTTPClient> cli) const;
	};

	class WebDriverOSInfo : public WebDriverParam
	{
	private:
		NN<Text::String> arch;
		NN<Text::String> name;
		NN<Text::String> version;

		WebDriverOSInfo(NN<Text::String> arch, NN<Text::String> name, NN<Text::String> version) { this->arch = arch; this->name = name; this->version = version; }
	public:
		virtual ~WebDriverOSInfo() {this->arch->Release(); this->name->Release(); this->version->Release(); }

		NN<Text::String> GetArch() const { return this->arch; }
		NN<Text::String> GetName() const { return this->name; }
		NN<Text::String> GetVersion() const { return this->version; }
		virtual void BuildJSON(NN<Text::JSONBuilder> builder) const;

		static Optional<WebDriverOSInfo> Parse(NN<Text::JSONObject> obj);
	};

	class WebDriverSlot
	{
	private:
		WebDriverSlot();
	public:
		~WebDriverSlot();

		static Optional<WebDriverSlot> Parse(NN<Text::JSONObject> obj);
	};

	class WebDriverNode
	{
	private:
		NN<Text::String> id;
		NN<Text::String> uri;
		UOSInt maxSessions;
		UOSInt sessionTimeout;
		NN<WebDriverOSInfo> osinfo;
		UOSInt heartbeatPeriod;
		NN<Text::String> availability;
		NN<Text::String> version;
		Data::ArrayListNN<WebDriverSlot> slots;

		WebDriverNode();
	public:
		~WebDriverNode();

		static Optional<WebDriverNode> Parse(NN<Text::JSONObject> obj);
	};

	class WebDriverTimeouts : public WebDriverParam
	{
	private:
		Int64 script;
		Int64 pageLoad;
		Int64 implicit;
		
	public:
		WebDriverTimeouts();
		virtual ~WebDriverTimeouts();

		NN<WebDriverTimeouts> SetScript(Int64 script);
		NN<WebDriverTimeouts> SetPageLoad(Int64 pageLoad);
		NN<WebDriverTimeouts> SetImplicit(Int64 implicit);

		virtual void BuildJSON(NN<Text::JSONBuilder> builder) const;
		static Optional<WebDriverTimeouts> Parse(NN<Text::JSONObject> obj);
	};

	class WebDriverBy : public WebDriverParam
	{
	private:
		NN<Text::String> usingType;
		NN<Text::String> value;
		
	public:
		WebDriverBy(Text::CStringNN usingType, Text::CStringNN value) { this->usingType = Text::String::New(usingType); this->value = Text::String::New(value); }
		virtual ~WebDriverBy() { this->usingType->Release(); this->value->Release(); }

		virtual void BuildJSON(NN<Text::JSONBuilder> builder) const { builder->ObjectAddStr(CSTR("using"), this->usingType); builder->ObjectAddStr(CSTR("value"), this->value); }
	};

	class WebDriverJSONParam : public WebDriverParam
	{
	private:
		NN<Text::JSONObject> obj;
		
	public:
		WebDriverJSONParam(NN<Text::JSONObject> obj) { this->obj = obj; }
		virtual ~WebDriverJSONParam() { this->obj->EndUse(); }

		virtual void BuildJSON(NN<Text::JSONBuilder> builder) const { builder->ObjectAdd(this->obj); }
	};

	class WebDriverExecuteCdpParam : public WebDriverParam
	{
	private:
		Text::CStringNN cmd;
		NN<Text::JSONObject> params;
	public:
		WebDriverExecuteCdpParam(Text::CStringNN cmd, NN<Text::JSONObject> params) { this->cmd = cmd; this->params = params; }
		virtual ~WebDriverExecuteCdpParam() { params->EndUse(); }

		virtual void BuildJSON(NN<Text::JSONBuilder> builder) const { builder->ObjectAddStr(CSTR("cmd"), cmd); builder->ObjectBeginObject(CSTR("params")); builder->ObjectAdd(params); builder->ObjectEnd(); }
	};

	class WebDriverBrowserOptions : public WebDriverParam
	{
	protected:
		NN<Text::String> browserName;
		Optional<Text::String> pageLoadStrategy;
		Optional<Text::String> platformName;
		Optional<WebDriverTimeouts> timeouts;

		void BuildW3CJSON(NN<Text::JSONBuilder> builder) const;
		WebDriverBrowserOptions(Text::CStringNN browserName);
	public:
		virtual ~WebDriverBrowserOptions();

		void SetPageLoadStrategy(Text::CString pageLoadStrategy);
		void SetPlatformName(Text::CString platformName);
		void SetTimeouts(Optional<WebDriverTimeouts> timeouts);
	};

	class WebDriverChromeOptions : public WebDriverBrowserOptions
	{
	private:
		Data::ArrayListStringNN args;
		Optional<Text::String> mobileDeviceName;

	public:
		WebDriverChromeOptions();
		virtual ~WebDriverChromeOptions();

		void AddArgs(Text::CStringNN arg);
		void SetMobileDeviceName(Text::CStringNN mobileDeviceName);

		virtual void BuildJSON(NN<Text::JSONBuilder> builder) const;
	};

	class WebDriverMSEdgeOptions : public WebDriverBrowserOptions
	{
	private:
		Data::ArrayListStringNN args;
		Optional<Text::String> mobileDeviceName;

	public:
		WebDriverMSEdgeOptions();
		virtual ~WebDriverMSEdgeOptions();

		void AddArgs(Text::CStringNN arg);
		void SetMobileDeviceName(Text::CStringNN mobileDeviceName);

		virtual void BuildJSON(NN<Text::JSONBuilder> builder) const;
	};

	class WebDriverFirefoxOptions : public WebDriverBrowserOptions
	{
	private:
		Data::ArrayListStringNN args;

	public:
		WebDriverFirefoxOptions();
		virtual ~WebDriverFirefoxOptions();

		void AddArgs(Text::CStringNN arg);

		virtual void BuildJSON(NN<Text::JSONBuilder> builder) const;
	};

	class WebDriverWebKitGTKOptions : public WebDriverBrowserOptions
	{
	private:
		Data::ArrayListStringNN args;

	public:
		WebDriverWebKitGTKOptions();
		virtual ~WebDriverWebKitGTKOptions();

		void AddArgs(Text::CStringNN arg);

		virtual void BuildJSON(NN<Text::JSONBuilder> builder) const;
	};

	class WebDriverW3CBrowserOptions : public WebDriverBrowserOptions
	{
	public:
		WebDriverW3CBrowserOptions(Text::CStringNN browserName);
		virtual ~WebDriverW3CBrowserOptions();

		virtual void BuildJSON(NN<Text::JSONBuilder> builder) const;
	};

	class WebDriverCapabilities : public WebDriverParam
	{
	private:
		Data::ArrayListNN<WebDriverBrowserOptions> firstMatch;
	public:
		WebDriverCapabilities(NN<WebDriverBrowserOptions> browser);
		virtual ~WebDriverCapabilities();	

		virtual void BuildJSON(NN<Text::JSONBuilder> builder) const;
	};

	class WebDriverStartSession : public WebDriverParam
	{
	private:
		Optional<Text::String> user;
		Optional<Text::String> password;
		WebDriverCapabilities capabilities;
	public:
		WebDriverStartSession(NN<WebDriverBrowserOptions> browser);
		virtual ~WebDriverStartSession();

		virtual void BuildJSON(NN<Text::JSONBuilder> builder) const;
	};

	class WebDriverClient
	{
	private:
		NN<Net::RESTResource> resource;
		Net::WebStatus::StatusCode lastErrorCode;
		Optional<Text::String> lastError;
		Optional<Text::String> lastErrorMessage;
		Optional<Text::String> lastErrorStacktrace;

	protected:
		Optional<Text::JSONBase> ParseResponse(Net::WebStatus::StatusCode code, NN<IO::MemoryStream> mstm, Text::CStringNN command, Optional<WebDriverParam> param);
		void ErrorInvalidResponse(Net::WebStatus::StatusCode code, NN<IO::MemoryStream> mstm, Text::CStringNN command, Optional<WebDriverParam> param);
		void ErrorInvalidValue(NN<Text::JSONBase> json, Text::CStringNN command, Optional<WebDriverParam> param);
		Bool ResponseExists(Optional<Text::JSONBase> value);
		Optional<Text::String> ResponseString(Optional<Text::JSONBase> value, Text::CStringNN command, Optional<WebDriverParam> param);
		Bool ResponseBool(Optional<Text::JSONBase> value, OutParam<Bool> outVal, Text::CStringNN command, Optional<WebDriverParam> param);
		Optional<Text::JSONBase> DoGet(Text::CStringNN command);
		Optional<Text::JSONBase> DoPost(Text::CStringNN command, NN<WebDriverParam> param);
		Optional<Text::JSONBase> DoPut(Text::CStringNN command, NN<WebDriverParam> param);
		Optional<Text::JSONBase> DoDelete(Text::CStringNN command, Optional<WebDriverParam> param);

		WebDriverClient(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Text::CStringNN url);
		WebDriverClient(NN<Net::RESTResource> resource);
	public:
		virtual ~WebDriverClient();

		void SetTimeout(Data::Duration dur) { this->resource->SetTimeout(dur); }
		NN<Net::RESTResource> CreateSubResource(Text::CStringNN subPath) { return this->resource->CreateSubResource(subPath); }
		Net::WebStatus::StatusCode GetLastErrorCode() const { return this->lastErrorCode; }
		Optional<Text::String> GetLastError() const { return this->lastError; }
		Optional<Text::String> GetLastErrorMessage() const { return this->lastErrorMessage; }
		Optional<Text::String> GetLastErrorStacktrace() const { return this->lastErrorStacktrace; }
	};

	class WebDriverSession : public WebDriverClient
	{
	public:
		WebDriverSession(NN<Net::RESTResource> resource);
		virtual ~WebDriverSession();

		Optional<WebDriverTimeouts> GetTimeouts();
		Bool SetTimeouts(NN<WebDriverTimeouts> timeouts);
		Bool ExecuteCdpCommand(Text::CStringNN cmd, NN<Text::JSONObject> params);
		Bool NavigateTo(Text::CStringNN url);
		Optional<Text::String> GetCurrentURL();
		Bool Back();
		Bool Forward();
		Bool Refresh();
		Optional<Text::String> GetTitle();
		Optional<Text::String> GetWindowHandle();
		//Bool CloseWindow();
		//Bool SwitchToWindow(Text::CStringNN handle);
		//Bool GetWindowHandles(NN<Data::ArrayListStringNN> handles);
		//Bool NewWindow(Text::CStringNN type);
		//Bool SwitchToFrame(Text::CString id);
		//Bool SwitchToParentFrame();
		Bool GetWindowRect(OutParam<Math::RectArea<Int64>> rect);
		Bool SetWindowRect(Math::RectArea<Int64> rect);
		Bool MaximizeWindow();
		Bool MinimizeWindow();
		Bool FullscreenWindow();
		//Optional<Text::String> GetActiveElement();
		//Optional<Text::String> GetElementShadowRoot(Text::CStringNN elementId);
		Optional<Text::String> FindElement(NN<WebDriverBy> by);
		Bool FindElements(NN<WebDriverBy> by, NN<Data::ArrayListStringNN> ids);
		Optional<Text::String> FindElementFromElement(Text::CStringNN elementId, NN<WebDriverBy> by);
		Bool FindElementsFromElement(Text::CStringNN elementId, NN<WebDriverBy> by, NN<Data::ArrayListStringNN> ids);
		Bool IsElementDisplayed(Text::CStringNN elementId, OutParam<Bool> displayed);
		Bool IsElementDisplayed(Text::CStringNN elementId);
		Bool IsElementSelected(Text::CStringNN elementId, OutParam<Bool> selected);
		Bool IsElementSelected(Text::CStringNN elementId);
		Bool IsElementEnabled(Text::CStringNN elementId, OutParam<Bool> enabled);
		Bool IsElementEnabled(Text::CStringNN elementId);
		Bool ElementClick(Text::CStringNN elementId);
		Bool ElementClear(Text::CStringNN elementId);
		Bool ElementSendKeys(Text::CStringNN elementId, Text::CStringNN keys);
		Bool ExecuteScript(Text::CStringNN elementId);

		template<typename Getter> Bool WaitUntil(Getter getter, Data::Duration dur)
		{
			Data::Timestamp startTime = Data::Timestamp::UtcNow();
			Bool succ = false;
			while (!(succ = getter()) && Data::Timestamp::UtcNow().Diff(startTime) < dur)
				Sync::ThreadUtil::SleepDur(50);
			return succ;
		}
	};

	class WebDriver : public WebDriverClient
	{
	public:
		Bool ready;
		Optional<Text::String> message;
		Data::ArrayListNN<WebDriverNode> nodes;

	public:
		WebDriver(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Text::CStringNN url);
		~WebDriver();

		Bool UpdateStatus();
		Optional<WebDriverSession> NewSession(NN<WebDriverStartSession> param);

		Bool IsReady() const { return this->ready; }
		Optional<Text::String> GetMessage() const { return this->message; }

		static Optional<Text::JSONObject> ParseJSON(NN<IO::MemoryStream> mstm);
		static Optional<WebDriverBy> ParseBy(Text::CStringNN by);
	};
}
#endif
