#include "Stdafx.h"
#include "Net/WebDriver.h"
#include "Text/TextBinEnc/Base64Enc.h"

//https://www.w3.org/TR/webdriver2/

void Net::WebDriverParam::WriteParam(NN<Net::HTTPClient> cli) const
{
	Text::JSONBuilder builder(this->GetObjectType());
	this->BuildJSON(builder);
	Text::CStringNN json = builder.Build();
	cli->AddContentType(CSTR("application/json;charset=UTF-8"));
	cli->AddContentLength(json.leng);
	cli->Write(json.ToByteArray());
}

void Net::WebDriverOSInfo::BuildJSON(NN<Text::JSONBuilder> builder) const
{
	builder->ObjectAddStr(CSTR("arch"), this->arch);
	builder->ObjectAddStr(CSTR("name"), this->name);
	builder->ObjectAddStr(CSTR("version"), this->version);
}

Optional<Net::WebDriverOSInfo> Net::WebDriverOSInfo::Parse(NN<Text::JSONObject> obj)
{
	NN<Text::String> arch;
	NN<Text::String> name;
	NN<Text::String> version;
	if (obj->GetObjectString(CSTR("arch")).SetTo(arch) && obj->GetObjectString(CSTR("name")).SetTo(name) && obj->GetObjectString(CSTR("version")).SetTo(version))
	{
		NN<Net::WebDriverOSInfo> me;
		NEW_CLASSNN(me, Net::WebDriverOSInfo(arch->Clone(), name->Clone(), version->Clone()));
		return me;
	}
	return 0;
}

Net::WebDriverSlot::WebDriverSlot()
{
}

Net::WebDriverSlot::~WebDriverSlot()
{
}

Optional<Net::WebDriverSlot> Net::WebDriverSlot::Parse(NN<Text::JSONObject> obj)
{
	/////////////////////////////
	return 0;
}

Net::WebDriverNode::WebDriverNode()
{
}

Net::WebDriverNode::~WebDriverNode()
{

}

Net::WebDriverTimeouts::WebDriverTimeouts()
{
	this->script = -1;
	this->pageLoad = -1;
	this->implicit = -1;
}

Net::WebDriverTimeouts::~WebDriverTimeouts()
{

}

NN<Net::WebDriverTimeouts> Net::WebDriverTimeouts::SetScript(Int64 script)
{
	this->script = script;
	return *this;
}

NN<Net::WebDriverTimeouts> Net::WebDriverTimeouts::SetPageLoad(Int64 pageLoad)
{
	this->pageLoad = pageLoad;
	return *this;
}

NN<Net::WebDriverTimeouts> Net::WebDriverTimeouts::SetImplicit(Int64 implicit)
{
	this->implicit = implicit;
	return *this;
}

void Net::WebDriverTimeouts::BuildJSON(NN<Text::JSONBuilder> builder) const
{
	if (this->script >= 0) builder->ObjectAddInt64(CSTR("script"), this->script);
	if (this->pageLoad >= 0) builder->ObjectAddInt64(CSTR("pageLoad"), this->pageLoad);
	if (this->implicit >= 0) builder->ObjectAddInt64(CSTR("implicit"), this->implicit);
}

Optional<Net::WebDriverTimeouts> Net::WebDriverTimeouts::Parse(NN<Text::JSONObject> obj)
{
	Int64 v;
	NN<Net::WebDriverTimeouts> timeouts;
	NEW_CLASSNN(timeouts, Net::WebDriverTimeouts());
	if (obj->GetValueAsInt64(CSTR("script"), v)) timeouts->SetScript(v);
	if (obj->GetValueAsInt64(CSTR("pageLoad"), v)) timeouts->SetPageLoad(v);
	if (obj->GetValueAsInt64(CSTR("implicit"), v)) timeouts->SetImplicit(v);
	return timeouts;
}

void Net::WebDriverBrowserOptions::BuildW3CJSON(NN<Text::JSONBuilder> builder) const
{
	NN<Text::String> s;
	NN<WebDriverTimeouts> timeouts;
	builder->ObjectAddStr(CSTR("browserName"), this->browserName);
	if (this->pageLoadStrategy.SetTo(s)) builder->ObjectAddStr(CSTR("pageLoadStrategy"), s);
	if (this->platformName.SetTo(s)) builder->ObjectAddStr(CSTR("platformName"), s);
	if (this->timeouts.SetTo(timeouts))
	{
		builder->ObjectBeginObject(CSTR("timeouts"));
		timeouts->BuildJSON(builder);
		builder->ObjectEnd();
	}
}

Net::WebDriverBrowserOptions::WebDriverBrowserOptions(Text::CStringNN browserName)
{
	this->browserName = Text::String::New(browserName);
	this->pageLoadStrategy = 0;
	this->platformName = 0;
	this->timeouts = 0;
}

Net::WebDriverBrowserOptions::~WebDriverBrowserOptions()
{
	this->browserName->Release();
	OPTSTR_DEL(this->pageLoadStrategy);
	OPTSTR_DEL(this->platformName);
	this->timeouts.Delete();
}

void Net::WebDriverBrowserOptions::SetPageLoadStrategy(Text::CString pageLoadStrategy)
{
	OPTSTR_DEL(this->pageLoadStrategy);
	this->pageLoadStrategy = Text::String::NewOrNull(pageLoadStrategy);
}

void Net::WebDriverBrowserOptions::SetPlatformName(Text::CString platformName)
{
	OPTSTR_DEL(this->platformName);
	this->platformName = Text::String::NewOrNull(platformName);
}

void Net::WebDriverBrowserOptions::SetTimeouts(Optional<WebDriverTimeouts> timeouts)
{
	this->timeouts.Delete();
	this->timeouts = timeouts;
}

Net::WebDriverChromeOptions::WebDriverChromeOptions() : WebDriverBrowserOptions(CSTR("chrome"))
{
	this->mobileDeviceName = 0;
}

Net::WebDriverChromeOptions::~WebDriverChromeOptions()
{
	this->args.FreeAll();
	OPTSTR_DEL(this->mobileDeviceName);
}

void Net::WebDriverChromeOptions::AddArgs(Text::CStringNN arg)
{
	this->args.Add(Text::String::New(arg));
}

void Net::WebDriverChromeOptions::SetMobileDeviceName(Text::CStringNN mobileDeviceName)
{
	OPTSTR_DEL(this->mobileDeviceName);
	this->mobileDeviceName = Text::String::New(mobileDeviceName);
}

void Net::WebDriverChromeOptions::BuildJSON(NN<Text::JSONBuilder> builder) const
{
	NN<Text::String> s;
	UOSInt i;
	UOSInt j;
	this->BuildW3CJSON(builder);
	if (this->args.GetCount() > 0 || this->mobileDeviceName.NotNull())
	{
		builder->ObjectBeginObject(CSTR("goog:chromeOptions"));
		if (this->args.GetCount() > 0)
		{
			builder->ObjectBeginArray(CSTR("args"));
			i = 0;
			j = this->args.GetCount();
			while (i < j)
			{
				builder->ArrayAddStr(this->args.GetItemNoCheck(i));
				i++;
			}
			builder->ArrayEnd();
		}
		if (this->mobileDeviceName.SetTo(s))
		{
			builder->ObjectBeginObject(CSTR("mobileEmulation"));
			builder->ObjectAddStr(CSTR("deviceName"), s);
			builder->ObjectEnd();
		}
		builder->ObjectEnd();
	}
}

Net::WebDriverMSEdgeOptions::WebDriverMSEdgeOptions() : WebDriverBrowserOptions(CSTR("MicrosoftEdge"))
{
	this->mobileDeviceName = 0;
}

Net::WebDriverMSEdgeOptions::~WebDriverMSEdgeOptions()
{
	this->args.FreeAll();
	OPTSTR_DEL(this->mobileDeviceName);
}

void Net::WebDriverMSEdgeOptions::AddArgs(Text::CStringNN arg)
{
	this->args.Add(Text::String::New(arg));
}

void Net::WebDriverMSEdgeOptions::SetMobileDeviceName(Text::CStringNN mobileDeviceName)
{
	OPTSTR_DEL(this->mobileDeviceName);
	this->mobileDeviceName = Text::String::New(mobileDeviceName);
}

void Net::WebDriverMSEdgeOptions::BuildJSON(NN<Text::JSONBuilder> builder) const
{
	NN<Text::String> s;
	UOSInt i;
	UOSInt j;
	this->BuildW3CJSON(builder);
	if (this->args.GetCount() > 0 || this->mobileDeviceName.NotNull())
	{
		builder->ObjectBeginObject(CSTR("ms:edgeOptions"));
		if (this->args.GetCount() > 0)
		{
			builder->ObjectBeginArray(CSTR("args"));
			i = 0;
			j = this->args.GetCount();
			while (i < j)
			{
				builder->ArrayAddStr(this->args.GetItemNoCheck(i));
				i++;
			}
			builder->ArrayEnd();
		}
		if (this->mobileDeviceName.SetTo(s))
		{
			builder->ObjectBeginObject(CSTR("mobileEmulation"));
			builder->ObjectAddStr(CSTR("deviceName"), s);
			builder->ObjectEnd();
		}
		builder->ObjectEnd();
	}
}

Net::WebDriverFirefoxOptions::WebDriverFirefoxOptions() : WebDriverBrowserOptions(CSTR("firefox"))
{
}

Net::WebDriverFirefoxOptions::~WebDriverFirefoxOptions()
{
	this->args.FreeAll();
}

void Net::WebDriverFirefoxOptions::AddArgs(Text::CStringNN arg)
{
	this->args.Add(Text::String::New(arg));
}

void Net::WebDriverFirefoxOptions::BuildJSON(NN<Text::JSONBuilder> builder) const
{
	UOSInt i;
	UOSInt j;
	this->BuildW3CJSON(builder);
	if (this->args.GetCount() > 0)
	{
		builder->ObjectBeginObject(CSTR("moz:firefoxOptions"));
		if (this->args.GetCount() > 0)
		{
			builder->ObjectBeginArray(CSTR("args"));
			i = 0;
			j = this->args.GetCount();
			while (i < j)
			{
				builder->ArrayAddStr(this->args.GetItemNoCheck(i));
				i++;
			}
			builder->ArrayEnd();
		}
		builder->ObjectEnd();
	}
}

Net::WebDriverWebKitGTKOptions::WebDriverWebKitGTKOptions() : WebDriverBrowserOptions(CSTR("MiniBrowser"))
{
}

Net::WebDriverWebKitGTKOptions::~WebDriverWebKitGTKOptions()
{
	this->args.FreeAll();
}

void Net::WebDriverWebKitGTKOptions::AddArgs(Text::CStringNN arg)
{
	this->args.Add(Text::String::New(arg));
}

void Net::WebDriverWebKitGTKOptions::BuildJSON(NN<Text::JSONBuilder> builder) const
{
	UOSInt i;
	UOSInt j;
	this->BuildW3CJSON(builder);
	if (this->args.GetCount() > 0)
	{
		builder->ObjectBeginObject(CSTR("webkitgtk:browserOptions"));
		if (this->args.GetCount() > 0)
		{
			builder->ObjectBeginArray(CSTR("args"));
			i = 0;
			j = this->args.GetCount();
			while (i < j)
			{
				builder->ArrayAddStr(this->args.GetItemNoCheck(i));
				i++;
			}
			builder->ArrayEnd();
		}
		builder->ObjectEnd();
	}
}

Net::WebDriverW3CBrowserOptions::WebDriverW3CBrowserOptions(Text::CStringNN browserName) : WebDriverBrowserOptions(browserName)
{
}

Net::WebDriverW3CBrowserOptions::~WebDriverW3CBrowserOptions()
{
}

void Net::WebDriverW3CBrowserOptions::BuildJSON(NN<Text::JSONBuilder> builder) const
{
	this->BuildW3CJSON(builder);
}

Net::WebDriverCapabilities::WebDriverCapabilities(NN<WebDriverBrowserOptions> browser)
{
	this->firstMatch.Add(browser);
}

Net::WebDriverCapabilities::~WebDriverCapabilities()
{
	this->firstMatch.DeleteAll();
}

void Net::WebDriverCapabilities::BuildJSON(NN<Text::JSONBuilder> builder) const
{
	NN<WebDriverBrowserOptions> browser;
	builder->ObjectBeginArray(CSTR("firstMatch"));
	UOSInt i = 0;
	UOSInt j = this->firstMatch.GetCount();
	while (i < j)
	{
		browser = this->firstMatch.GetItemNoCheck(i);
		builder->ArrayBeginObject();
		browser->BuildJSON(builder);
		builder->ObjectEnd();
		i++;
	}
	builder->ArrayEnd();
}

Optional<Net::WebDriverNode> Net::WebDriverNode::Parse(NN<Text::JSONObject> obj)
{
	NN<Text::String> id;
	NN<Text::String> uri;
	Int64 maxSessions;
	Int64 sessionTimeout;
	NN<WebDriverOSInfo> osinfo;
	Int64 heartbeatPeriod;
	NN<Text::String> availability;
	NN<Text::String> version;
	NN<Text::JSONObject> osinfoObj;
	if (!obj->GetValueString(CSTR("id")).SetTo(id)) return 0;
	if (!obj->GetValueString(CSTR("uri")).SetTo(uri)) return 0;
	if (!obj->GetValueAsInt64(CSTR("maxSessions"), maxSessions)) return 0;
	if (!obj->GetValueAsInt64(CSTR("sessionTimeout"), sessionTimeout)) return 0;
	if (!obj->GetValueAsInt64(CSTR("heartbeatPeriod"), heartbeatPeriod)) return 0;
	if (!obj->GetValueString(CSTR("availability")).SetTo(availability)) return 0;
	if (!obj->GetValueString(CSTR("version")).SetTo(version)) return 0;
	if (!obj->GetValueObject(CSTR("osinfo")).SetTo(osinfoObj) || !WebDriverOSInfo::Parse(osinfoObj).SetTo(osinfo)) return 0;
	NN<WebDriverSlot> slot;
	NN<WebDriverNode> node;
	NEW_CLASSNN(node, WebDriverNode());
	node->id = id->Clone();
	node->uri = uri->Clone();
	node->maxSessions = (UOSInt)maxSessions;
	node->sessionTimeout = (UOSInt)sessionTimeout;
	node->osinfo = osinfo;
	node->heartbeatPeriod = (UOSInt)heartbeatPeriod;
	node->availability = availability->Clone();
	node->version = version->Clone();
	NN<Text::JSONArray> slots;
	NN<Text::JSONObject> slotObj;
	if (obj->GetValueArray(CSTR("slots")).SetTo(slots))
	{
		UOSInt i = 0;
		UOSInt j = slots->GetArrayLength();
		while (i < j)
		{
			if (slots->GetArrayObject(i).SetTo(slotObj) && WebDriverSlot::Parse(slotObj).SetTo(slot))
			{
				node->slots.Add(slot);
			}
			i++;
		}
	}
	return node;
}

Net::WebDriverStartSession::WebDriverStartSession(NN<WebDriverBrowserOptions> browser) : capabilities(browser)
{
	this->user = 0;
	this->password = 0;
}

Net::WebDriverStartSession::~WebDriverStartSession()
{
	OPTSTR_DEL(this->user);
	OPTSTR_DEL(this->password);
}

void Net::WebDriverStartSession::BuildJSON(NN<Text::JSONBuilder> builder) const
{
	NN<Text::String> s;
	if (this->user.SetTo(s)) builder->ObjectAddStr(CSTR("user"), s);
	if (this->password.SetTo(s)) builder->ObjectAddStr(CSTR("password"), s);
	builder->ObjectBeginObject(CSTR("capabilities"));
	this->capabilities.BuildJSON(builder);
	builder->ObjectEnd();
}

Optional<Text::JSONBase> Net::WebDriverClient::ParseResponse(Net::WebStatus::StatusCode code, NN<IO::MemoryStream> mstm, Text::CStringNN command, Optional<WebDriverParam> param)
{
	NN<Text::JSONObject> obj;
	NN<Text::JSONBase> objBase;
	NN<Text::String> error;
	NN<Text::String> message;
	NN<Text::String> s;
	if (code == Net::WebStatus::SC_OK && WebDriver::ParseJSON(mstm).SetTo(obj))
	{
		if (obj->GetValue(CSTR("value")).SetTo(objBase))
		{
			if (objBase->GetValueString(CSTR("error")).SetTo(error) && objBase->GetValueString(CSTR("message")).SetTo(message))
			{
				this->lastErrorCode = code;
				OPTSTR_DEL(this->lastError);
				OPTSTR_DEL(this->lastErrorMessage);
				OPTSTR_DEL(this->lastErrorStacktrace);
				this->lastError = error->Clone();
				this->lastErrorMessage = message->Clone();
				if (objBase->GetValueString(CSTR("stacktrace")).SetTo(s))
				{
					this->lastErrorStacktrace = s->Clone();
				}
				obj->EndUse();
				return 0;
			}
			objBase->BeginUse();
			obj->EndUse();
			return objBase;
		}
		else
		{
			obj->EndUse();
			this->ErrorInvalidResponse(code, mstm, command, param);
			return 0;
		}
	}
	else if (WebDriver::ParseJSON(mstm).SetTo(obj))
	{
		if (obj->GetValue(CSTR("value")).SetTo(objBase))
		{
			if (objBase->GetValueString(CSTR("error")).SetTo(error) && objBase->GetValueString(CSTR("message")).SetTo(message))
			{
				this->lastErrorCode = code;
				OPTSTR_DEL(this->lastError);
				OPTSTR_DEL(this->lastErrorMessage);
				OPTSTR_DEL(this->lastErrorStacktrace);
				this->lastError = error->Clone();
				this->lastErrorMessage = message->Clone();
				if (objBase->GetValueString(CSTR("stacktrace")).SetTo(s))
				{
					this->lastErrorStacktrace = s->Clone();
				}
				obj->EndUse();
				return 0;
			}
		}
		obj->EndUse();
	}
	this->ErrorInvalidResponse(code, mstm, command, param);
	return 0;
}

void Net::WebDriverClient::ErrorInvalidResponse(Net::WebStatus::StatusCode code, NN<IO::MemoryStream> mstm, Text::CStringNN command, Optional<WebDriverParam> param)
{
	this->lastErrorCode = code;
	OPTSTR_DEL(this->lastError);
	OPTSTR_DEL(this->lastErrorMessage);
	OPTSTR_DEL(this->lastErrorStacktrace);
	this->lastError = Text::String::New(CSTR("Invalid response"));
	Text::StringBuilderUTF8 sb;
	NN<WebDriverParam> nnparam;
	sb.Append(CSTR("Command: "));
	sb.Append(command);
	if (param.SetTo(nnparam))
	{
		sb.Append(CSTR("\r\nParam: "));
		Text::JSONBuilder json(Text::JSONBuilder::OT_OBJECT);
		nnparam->BuildJSON(json);
		sb.Append(json.Build());
	}
	sb.Append(CSTR("\r\nResponse: "));
	sb.Append(mstm->ToCString());
	this->lastErrorMessage = Text::String::New(sb.ToCString());
}

void Net::WebDriverClient::ErrorInvalidValue(NN<Text::JSONBase> json, Text::CStringNN command, Optional<WebDriverParam> param)
{
	this->lastErrorCode = Net::WebStatus::SC_OK;
	OPTSTR_DEL(this->lastError);
	OPTSTR_DEL(this->lastErrorMessage);
	OPTSTR_DEL(this->lastErrorStacktrace);
	this->lastError = Text::String::New(CSTR("Invalid response"));
	Text::StringBuilderUTF8 sb;
	NN<WebDriverParam> nnparam;
	sb.Append(CSTR("Command: "));
	sb.Append(command);
	if (param.SetTo(nnparam))
	{
		sb.Append(CSTR("\r\nParam: "));
		Text::JSONBuilder json(Text::JSONBuilder::OT_OBJECT);
		nnparam->BuildJSON(json);
		sb.Append(json.Build());
	}
	sb.Append(CSTR("\r\nResponse Value: "));
	json->ToJSONString(sb);
	this->lastErrorMessage = Text::String::New(sb.ToCString());
}

Bool Net::WebDriverClient::ResponseExists(Optional<Text::JSONBase> value)
{
	NN<Text::JSONBase> json;
	if (value.SetTo(json))
	{
		json->EndUse();
		return true;
	}
	return false;
}

Optional<Text::String> Net::WebDriverClient::ResponseString(Optional<Text::JSONBase> value, Text::CStringNN command, Optional<WebDriverParam> param)
{
	NN<Text::JSONBase> json;
	if (value.SetTo(json))
	{
		Optional<Text::String> ret = 0;
		if (json->GetType() == Text::JSONType::String)
		{
			ret = NN<Text::JSONString>::ConvertFrom(json)->GetValue()->Clone();
		}
		else
		{
			this->ErrorInvalidValue(json, command, param);
		}
		json->EndUse();
		return ret;
	}
	return 0;
}

Bool Net::WebDriverClient::ResponseBool(Optional<Text::JSONBase> value, OutParam<Bool> outVal, Text::CStringNN command, Optional<WebDriverParam> param)
{
	NN<Text::JSONBase> json;
	if (value.SetTo(json))
	{
		outVal.Set(json->GetAsBool());
		json->EndUse();
		return true;
	}
	return false;
}

Optional<Text::JSONBase> Net::WebDriverClient::DoGet(Text::CStringNN command)
{
	IO::MemoryStream mstm;
	return ParseResponse(this->resource->Get(command, mstm), mstm, command, 0);
}

Optional<Text::JSONBase> Net::WebDriverClient::DoPost(Text::CStringNN command, NN<WebDriverParam> param)
{
	IO::MemoryStream mstm;
	return ParseResponse(this->resource->Post(command, param, mstm), mstm, command, param);
}

Optional<Text::JSONBase> Net::WebDriverClient::DoPut(Text::CStringNN command, NN<WebDriverParam> param)
{
	IO::MemoryStream mstm;
	return ParseResponse(this->resource->Put(command, param, mstm), mstm, command, param);
}

Optional<Text::JSONBase> Net::WebDriverClient::DoDelete(Text::CStringNN command, Optional<WebDriverParam> param)
{
	IO::MemoryStream mstm;
	return ParseResponse(this->resource->Delete(command, param, mstm), mstm, command, param);
}

Net::WebDriverClient::WebDriverClient(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Text::CStringNN url)
{
	NEW_CLASSNN(this->resource, Net::RESTResource(clif, ssl, url));
	this->lastErrorCode = Net::WebStatus::SC_UNKNOWN;
	this->lastError = 0;
	this->lastErrorMessage = 0;
	this->lastErrorStacktrace = 0;
}

Net::WebDriverClient::WebDriverClient(NN<Net::RESTResource> resource)
{
	this->resource = resource;
	this->lastErrorCode = Net::WebStatus::SC_UNKNOWN;
	this->lastError = 0;
	this->lastErrorMessage = 0;
	this->lastErrorStacktrace = 0;
}

Net::WebDriverClient::~WebDriverClient()
{
	this->resource.Delete();
	OPTSTR_DEL(this->lastError);
	OPTSTR_DEL(this->lastErrorMessage);
	OPTSTR_DEL(this->lastErrorStacktrace);
}

Net::WebDriverSession::WebDriverSession(NN<Net::RESTResource> resource) : WebDriverClient(resource)
{
}

Net::WebDriverSession::~WebDriverSession()
{
	this->ResponseExists(this->DoDelete(CSTR(""), 0));
}

Optional<Net::WebDriverTimeouts> Net::WebDriverSession::GetTimeouts()
{
	Optional<WebDriverTimeouts> timeouts = 0;
	NN<Text::JSONBase> obj;
	if (this->DoGet(CSTR("timeouts")).SetTo(obj))
	{
		if (obj->GetType() == Text::JSONType::Object)
		{
			timeouts = WebDriverTimeouts::Parse(NN<Text::JSONObject>::ConvertFrom(obj));
		}
		else
		{
			this->ErrorInvalidValue(obj, CSTR("timeouts"), 0);
		}
		obj->EndUse();
	}
	return timeouts;
}

Bool Net::WebDriverSession::SetTimeouts(NN<WebDriverTimeouts> timeouts)
{
	return ResponseExists(this->DoPost(CSTR("timeouts"), timeouts));
}

Bool Net::WebDriverSession::ExecuteCdpCommand(Text::CStringNN cmd, NN<Text::JSONObject> params)
{
	WebDriverExecuteCdpParam cdpParam(cmd, params);
	return ResponseExists(this->DoPost(CSTR("goog/cdp/execute"), cdpParam));
}

Bool Net::WebDriverSession::NavigateTo(Text::CStringNN url)
{
	WebDriverJSONParam param(Text::JSONObject::New()->SetObjectString(CSTR("url"), url));
	return ResponseExists(this->DoPost(CSTR("url"), param));
}

Optional<Text::String> Net::WebDriverSession::GetCurrentURL()
{
	return ResponseString(this->DoGet(CSTR("url")), CSTR("url"), 0);
}

Bool Net::WebDriverSession::Back()
{
	WebDriverJSONParam param(Text::JSONObject::New());
	return ResponseExists(this->DoPost(CSTR("back"), param));
}

Bool Net::WebDriverSession::Forward()
{
	WebDriverJSONParam param(Text::JSONObject::New());
	return ResponseExists(this->DoPost(CSTR("forward"), param));
}

Bool Net::WebDriverSession::Refresh()
{
	WebDriverJSONParam param(Text::JSONObject::New());
	return ResponseExists(this->DoPost(CSTR("refresh"), param));
}

Optional<Text::String> Net::WebDriverSession::GetTitle()
{
	return ResponseString(this->DoGet(CSTR("title")), CSTR("title"), 0);
}

Optional<Text::String> Net::WebDriverSession::GetWindowHandle()
{
	return ResponseString(this->DoGet(CSTR("window")), CSTR("window"), 0);
}

Bool Net::WebDriverSession::GetWindowRect(OutParam<Math::RectArea<Int64>> rect)
{
	Bool ret = false;
	NN<Text::JSONBase> obj;
	if (this->DoGet(CSTR("window/rect")).SetTo(obj))
	{
		NN<Text::JSONObject> value;
		if (obj->GetValueObject(CSTR("value")).SetTo(value))
		{
			Int64 x;
			Int64 y;
			Int64 width;
			Int64 height;
			if (value->GetObjectInt64(CSTR("x"), x) && value->GetObjectInt64(CSTR("y"), y) && value->GetObjectInt64(CSTR("width"), width) && value->GetObjectInt64(CSTR("height"), height))
			{
				ret = true;
				rect.Set(Math::RectArea<Int64>(x, y, width, height));
			}
			else
			{
				this->ErrorInvalidValue(obj, CSTR("window/rect"), 0);
			}
		}
		else
		{
			this->ErrorInvalidValue(obj, CSTR("window/rect"), 0);
		}
		obj->EndUse();
	}
	return ret;
}

Bool Net::WebDriverSession::SetWindowRect(Math::RectArea<Int64> rect)
{
	WebDriverJSONParam param(Text::JSONObject::New()
		->SetObjectInt64(CSTR("x"), rect.min.x)
		->SetObjectInt64(CSTR("y"), rect.min.y)
		->SetObjectInt64(CSTR("width"), rect.GetWidth())
		->SetObjectInt64(CSTR("height"), rect.GetHeight()));
	return ResponseExists(this->DoPost(CSTR("window/rect"), param));
}

Bool Net::WebDriverSession::MaximizeWindow()
{
	WebDriverJSONParam param(Text::JSONObject::New());
	return ResponseExists(this->DoPost(CSTR("window/maximize"), param));
}

Bool Net::WebDriverSession::MinimizeWindow()
{
	WebDriverJSONParam param(Text::JSONObject::New());
	return ResponseExists(this->DoPost(CSTR("window/minimize"), param));
}

Bool Net::WebDriverSession::FullscreenWindow()
{
	WebDriverJSONParam param(Text::JSONObject::New());
	return ResponseExists(this->DoPost(CSTR("window/fullscreen"), param));
}

Optional<Text::String> Net::WebDriverSession::FindElement(NN<WebDriverBy> by)
{
	Optional<Text::String> ret = 0;
	NN<Text::JSONBase> obj;
	NN<Text::JSONObject> valueObj;
	if (this->DoPost(CSTR("element"), by).SetTo(obj))
	{
		if (obj->GetType() == Text::JSONType::Object)
		{
			valueObj = NN<Text::JSONObject>::ConvertFrom(obj);
			Data::ArrayListStringNN names;
			valueObj->GetObjectNames(names);
			if (names.GetCount() > 0)
			{
				ret = valueObj->GetObjectNewString(names.GetItemNoCheck(0)->ToCString());
			}
		}
		obj->EndUse();
		if (ret.IsNull())
		{
			this->ErrorInvalidValue(obj, CSTR("element"), by);
		}
	}
	return ret;
}

Bool Net::WebDriverSession::FindElements(NN<WebDriverBy> by, NN<Data::ArrayListStringNN> ids)
{
	Bool ret = false;
	NN<Text::JSONBase> obj;
	NN<Text::JSONArray> valueObj;
	NN<Text::String> s;
	if (this->DoPost(CSTR("elements"), by).SetTo(obj))
	{
		if (obj->GetType() == Text::JSONType::Array)
		{
			valueObj = NN<Text::JSONArray>::ConvertFrom(obj);
			UOSInt i = 0;
			UOSInt j = valueObj->GetArrayLength();
			while (i < j)
			{
				if (valueObj->GetArrayString(i).SetTo(s))
				{
					ids->Add(s->Clone());
				}
				i++;
			}
			ret = true;
		}
		else
		{
			this->ErrorInvalidValue(obj, CSTR("elements"), by);
		}
		obj->EndUse();
	}
	return ret;
}

Optional<Text::String> Net::WebDriverSession::FindElementFromElement(Text::CStringNN elementId, NN<WebDriverBy> by)
{
	Optional<Text::String> ret = 0;
	NN<Text::JSONBase> obj;
	NN<Text::JSONObject> valueObj;
	Text::StringBuilderUTF8 sb;
	sb.Append(CSTR("element/"));
	sb.Append(elementId);
	sb.Append(CSTR("/element"));
	if (this->DoPost(sb.ToCString(), by).SetTo(obj))
	{
		if (obj->GetType() == Text::JSONType::Object)
		{
			valueObj = NN<Text::JSONObject>::ConvertFrom(obj);
			Data::ArrayListStringNN names;
			valueObj->GetObjectNames(names);
			if (names.GetCount() > 0)
			{
				ret = valueObj->GetObjectNewString(names.GetItemNoCheck(0)->ToCString());
			}
		}
		obj->EndUse();
		if (ret.IsNull())
		{
			this->ErrorInvalidValue(obj, sb.ToCString(), by);
		}
	}
	return ret;
}

Bool Net::WebDriverSession::FindElementsFromElement(Text::CStringNN elementId, NN<WebDriverBy> by, NN<Data::ArrayListStringNN> ids)
{
	Bool ret = false;
	NN<Text::JSONBase> obj;
	NN<Text::JSONArray> valueObj;
	NN<Text::String> s;
	Text::StringBuilderUTF8 sb;
	sb.Append(CSTR("element/"));
	sb.Append(elementId);
	sb.Append(CSTR("/elements"));
	if (this->DoPost(sb.ToCString(), by).SetTo(obj))
	{
		if (obj->GetType() == Text::JSONType::Array)
		{
			valueObj = NN<Text::JSONArray>::ConvertFrom(obj);
			UOSInt i = 0;
			UOSInt j = valueObj->GetArrayLength();
			while (i < j)
			{
				if (valueObj->GetArrayString(i).SetTo(s))
				{
					ids->Add(s->Clone());
				}
				i++;
			}
			ret = true;
		}
		else
		{
			this->ErrorInvalidValue(obj, sb.ToCString(), by);
		}
		obj->EndUse();
	}
	return ret;
}

Bool Net::WebDriverSession::IsElementDisplayed(Text::CStringNN elementId, OutParam<Bool> displayed)
{
	Text::StringBuilderUTF8 sb;
	sb.Append(CSTR("element/"));
	sb.Append(elementId);
	sb.Append(CSTR("/displayed"));
	return ResponseBool(DoGet(sb.ToCString()), displayed, sb.ToCString(), 0);
}

Bool Net::WebDriverSession::IsElementDisplayed(Text::CStringNN elementId)
{
	Bool ret;
	return this->IsElementDisplayed(elementId, ret) && ret;
}

Bool Net::WebDriverSession::IsElementSelected(Text::CStringNN elementId, OutParam<Bool> selected)
{
	Text::StringBuilderUTF8 sb;
	sb.Append(CSTR("element/"));
	sb.Append(elementId);
	sb.Append(CSTR("/selected"));
	return ResponseBool(DoGet(sb.ToCString()), selected, sb.ToCString(), 0);
}

Bool Net::WebDriverSession::IsElementSelected(Text::CStringNN elementId)
{
	Bool ret;
	return this->IsElementSelected(elementId, ret) && ret;
}

Bool Net::WebDriverSession::IsElementEnabled(Text::CStringNN elementId, OutParam<Bool> enabled)
{
	Text::StringBuilderUTF8 sb;
	sb.Append(CSTR("element/"));
	sb.Append(elementId);
	sb.Append(CSTR("/enabled"));
	return ResponseBool(DoGet(sb.ToCString()), enabled, sb.ToCString(), 0);
}

Bool Net::WebDriverSession::IsElementEnabled(Text::CStringNN elementId)
{
	Bool ret;
	return this->IsElementEnabled(elementId, ret) && ret;
}

Bool Net::WebDriverSession::ElementClick(Text::CStringNN elementId)
{
	WebDriverJSONParam param(Text::JSONObject::New());
	Text::StringBuilderUTF8 sb;
	sb.Append(CSTR("element/"));
	sb.Append(elementId);
	sb.Append(CSTR("/click"));
	return ResponseExists(this->DoPost(sb.ToCString(), param));
}

Bool Net::WebDriverSession::ElementClear(Text::CStringNN elementId)
{
	WebDriverJSONParam param(Text::JSONObject::New());
	Text::StringBuilderUTF8 sb;
	sb.Append(CSTR("element/"));
	sb.Append(elementId);
	sb.Append(CSTR("/clear"));
	return ResponseExists(this->DoPost(sb.ToCString(), param));

}

Bool Net::WebDriverSession::ElementSendKeys(Text::CStringNN elementId, Text::CStringNN keys)
{
	WebDriverJSONParam param(Text::JSONObject::New()
		->SetObjectString(CSTR("text"), keys)
		->SetObjectValueAndRelease(CSTR("value"), Text::JSONArray::New()->AddArrayString(keys)));
	Text::StringBuilderUTF8 sb;
	sb.Append(CSTR("element/"));
	sb.Append(elementId);
	sb.Append(CSTR("/value"));
	return ResponseExists(this->DoPost(sb.ToCString(), param));
}

Bool Net::WebDriverSession::ExecuteScript(Text::CStringNN script)
{
	Net::WebDriverJSONParam param(Text::JSONObject::New()->SetObjectString(CSTR("script"), script)->SetObjectValueAndRelease(CSTR("args"), Text::JSONArray::New()));
	return ResponseExists(this->DoPost(CSTR("execute/sync"), param));
}

Optional<IO::MemoryStream> Net::WebDriverSession::TakeScreenshot()
{
	NN<Text::String> b64;
	if (!ResponseString(this->DoGet(CSTR("screenshot")), CSTR("screenshot"), 0).SetTo(b64))
	{
		return 0;
	}
	Text::TextBinEnc::Base64Enc b64Enc;
	UnsafeArray<UInt8> tmpBuff = MemAllocArr(UInt8, b64->leng);
	UOSInt buffLen;
	buffLen = b64Enc.DecodeBin(b64->ToCString(), tmpBuff);
	NN<IO::MemoryStream> mstm;
	NEW_CLASSNN(mstm, IO::MemoryStream(buffLen));
	mstm->Write(Data::ByteArrayR(tmpBuff, buffLen));
	MemFreeArr(tmpBuff);
	b64->Release();
	return mstm;
}

Net::WebDriver::WebDriver(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Text::CStringNN url, Data::Duration cmdTimeout) : WebDriverClient(clif, ssl, url)
{
	this->SetTimeout(cmdTimeout);
	this->ready = false;
	this->message = 0;
	this->UpdateStatus();
}

Net::WebDriver::~WebDriver()
{
	OPTSTR_DEL(this->message);
	this->nodes.DeleteAll();
}

Bool Net::WebDriver::UpdateStatus()
{
	NN<Text::JSONBase> obj;
	if (this->DoGet(CSTR("status")).SetTo(obj))
	{
		NN<WebDriverNode> node;
		OPTSTR_DEL(this->message);
		this->nodes.DeleteAll();
		this->ready = obj->GetValueAsBool(CSTR("ready"));
		this->message = obj->GetValueNewString(CSTR("message"));
		NN<Text::JSONArray> nodesArr;
		if (obj->GetValueArray(CSTR("nodes")).SetTo(nodesArr))
		{
			NN<Text::JSONObject> nodeObj;
			UOSInt i = 0;
			UOSInt j = nodesArr->GetArrayLength();
			while (i < j)
			{
				if (nodesArr->GetArrayObject(i).SetTo(nodeObj) && WebDriverNode::Parse(nodeObj).SetTo(node))
				{
					this->nodes.Add(node);
				}
				i++;
			}
		}

		obj->EndUse();
		return true;
	}
	return false;
}

Optional<Net::WebDriverSession> Net::WebDriver::NewSession(NN<WebDriverStartSession> param)
{
	NN<Text::JSONBase> response;
	if (this->DoPost(CSTR("session"), param).SetTo(response))
	{
		NN<Text::String> sessId;
		if (response->GetValueString(CSTR("sessionId")).SetTo(sessId))
		{
			Text::StringBuilderUTF8 sb;
			sb.Append(CSTR("session/"));
			sb.Append(sessId);
			NN<WebDriverSession> sess;
			NEW_CLASSNN(sess, WebDriverSession(this->CreateSubResource(sb.ToCString())));
			response->EndUse();
			return sess;
		}
		response->EndUse();
	}
	return 0;
}

Optional<Text::JSONObject> Net::WebDriver::ParseJSON(NN<IO::MemoryStream> mstm)
{
	NN<Text::JSONBase> json;
	if (Text::JSONBase::ParseJSONStr(mstm->ToCString()).SetTo(json))
	{
		if (json->GetType() == Text::JSONType::Object)
		{
			return NN<Text::JSONObject>::ConvertFrom(json);
		}
		json->EndUse();
	}
	return 0;
}


Optional<Net::WebDriverBy> Net::WebDriver::ParseBy(Text::CStringNN by)
{
	Optional<Net::WebDriverBy> ret;
	if (by.StartsWith(CSTR("xpath=")))
	{
		NEW_CLASSOPT(ret, Net::WebDriverBy(CSTR("xpath"), by.Substring(6)));
	}
	else if (by.StartsWith(CSTR("css=")))
	{
		NEW_CLASSOPT(ret, Net::WebDriverBy(CSTR("css selector"), by.Substring(4)));
	}
	else if (by.StartsWith(CSTR("id=")))
	{
		Text::StringBuilderUTF8 sb;
		sb.Append(CSTR("*[id=\""));
		sb.Append(by.Substring(3));
		sb.Append(CSTR("\"]"));
		NEW_CLASSOPT(ret, Net::WebDriverBy(CSTR("css selector"), sb.ToCString()));
	}
	else if (by.StartsWith(CSTR("linkText=")))
	{
		NEW_CLASSOPT(ret, Net::WebDriverBy(CSTR("link text"), by.Substring(9)));
	}
	else
	{
		ret = 0;
	}
	return ret;
}
