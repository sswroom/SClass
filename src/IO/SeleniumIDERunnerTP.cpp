#include "Stdafx.h"
#include "webdriverxx.h"
#include "IO/SeleniumIDERunner.h"
#include "Sync/ThreadUtil.h"

enum class CondType
{
	Times
};
webdriverxx::By SeleniumIDERunner_ParseBy(Text::CStringNN target)
{
	if (target.StartsWith(CSTR("xpath=")))
	{
		return webdriverxx::ByXPath((const Char*)target.Substring(6).v.Ptr());
	}
	else if (target.StartsWith(CSTR("css=")))
	{
		return webdriverxx::ByCss((const Char*)target.Substring(4).v.Ptr());
	}
	else if (target.StartsWith(CSTR("id=")))
	{
		return webdriverxx::ById((const Char*)target.Substring(3).v.Ptr());
	}
	else if (target.StartsWith(CSTR("linkText=")))
	{
		return webdriverxx::ByLinkText((const Char*)target.Substring(9).v.Ptr());
	}
	throw webdriverxx::WebDriverException(std::string("Unknown Target: ") + (const Char*)target.v.Ptr());
}

webdriverxx::Element SeleniumIDERunner_WaitForElement(webdriverxx::WebDriver &driver, webdriverxx::By &by, UInt32 dur)
{
	auto elementFinder = [&]{
		return driver.FindElement(by);
	};
	return webdriverxx::WaitForValue(elementFinder, dur);
}

IO::SeleniumIDERunner::SeleniumIDERunner(NN<Net::TCPClientFactory> clif)
{
	this->clif = clif;
	this->lastErrorIndex = INVALID_INDEX;
	this->lastErrorMsg = 0;
	this->url = Text::String::New(UTF8STRC("http://localhost:4444/wd/hub"));
}

IO::SeleniumIDERunner::~SeleniumIDERunner()
{
	OPTSTR_DEL(this->lastErrorMsg);
	this->url->Release();
}

Bool IO::SeleniumIDERunner::Run(NN<SeleniumTest> test, Text::CString mobileDevice, Optional<GPSPosition> location, StepStatusHandler statusHdlr, AnyType userObj)
{
	UOSInt currIndex = INVALID_INDEX;
	NN<IO::SeleniumCommand> command;
	NN<Text::String> s;
	UOSInt i;
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	struct {
		CondType type;
		UOSInt index;
		UOSInt param;
	} cond[16];
	UOSInt condCnt = 0;
	NN<GPSPosition> nnlocation;
	try
	{
		webdriverxx::ChromeOptions options;
		webdriverxx::Chrome chrome;
		webdriverxx::Timeouts to;
		Text::CStringNN nnmobileDevice;
		if (mobileDevice.SetTo(nnmobileDevice))
		{
			webdriverxx::chrome::MobileEmulation me;
			me.SetDeviceName((const Char*)nnmobileDevice.v.Ptr());
			options.SetMobileEmulation(me);
		}
		to.SetImplicitTimeout(5000);
		options.SetArgs({"start-maximized"});
		chrome.SetChromeOptions(options);
		chrome.SetTimeouts(to);
		webdriverxx::WebDriver driver = webdriverxx::Start(chrome, (const Char*)this->url->v.Ptr());
		if (location.SetTo(nnlocation))
		{
			driver.ExecuteCdpCommand("Emulation.setGeolocationOverride", webdriverxx::JsonObject()
			.Set("latitude", nnlocation->latitude)
			.Set("longitude", nnlocation->longitude)
			.Set("accuracy", nnlocation->accuracy));
		}
		Data::Timestamp lastTime = Data::Timestamp::UtcNow();
		Data::Timestamp thisTime;
		Bool skip = false;
		currIndex = 0;
		while (test->GetCommand(currIndex).SetTo(command))
		{
			if (!command->GetCommand().SetTo(s))
			{
				OPTSTR_DEL(this->lastErrorMsg);
				this->lastErrorMsg = Text::String::New(UTF8STRC("Unknown command"));
				this->lastErrorIndex = currIndex;
				return false;
			}
			if (s->Equals(CSTR("end")))
			{
				if (condCnt == 0)
				{
					OPTSTR_DEL(this->lastErrorMsg);
					this->lastErrorMsg = Text::String::New(UTF8STRC("End found without condition"));
					this->lastErrorIndex = currIndex;
					return false;
				}
				condCnt--;
				if (cond[condCnt].type == CondType::Times)
				{
					cond[condCnt].param--;
					if (cond[condCnt].param > 0)
					{
						currIndex = cond[condCnt].index;
						condCnt++;
					}
					else
					{
						skip = false;
					}
				}
				else
				{
					OPTSTR_DEL(this->lastErrorMsg);
					this->lastErrorMsg = Text::String::New(UTF8STRC("Unknown condition"));
					this->lastErrorIndex = currIndex;
					return false;
				}
			}
			else if (skip)
			{

			}
			else if (s->Equals(CSTR("open")))
			{
				driver.Navigate((const Char*)Text::String::OrEmpty(command->GetTarget())->v.Ptr());
			}
			else if (s->Equals(CSTR("setWindowSize")))
			{
				if (!command->GetTarget().SetTo(s) || s->leng >= 64)
				{
					OPTSTR_DEL(this->lastErrorMsg);
					this->lastErrorMsg = Text::String::New(UTF8STRC("setWindowsSize invalid target"));
					this->lastErrorIndex = currIndex;
					return false;
				}
				sptr = s->ConcatTo(sbuff);
				i = s->IndexOf('x');
				if (i == INVALID_INDEX)
				{
					OPTSTR_DEL(this->lastErrorMsg);
					Text::StringBuilderUTF8 sb;
					sb.Append(CSTR("setWindowsSize invalid target: "));
					sb.Append(s);
					this->lastErrorMsg = Text::String::New(sb.ToCString());
					this->lastErrorIndex = currIndex;
					return false;
				}
				sbuff[i] = 0;
				driver.GetWindow().SetRect(webdriverxx::Rect(0, 0, Text::StrToInt32(sbuff), Text::StrToInt32(&sbuff[i + 1])));
			}
			else if (s->Equals(CSTR("waitForElementVisible")))
			{
				webdriverxx::By by = SeleniumIDERunner_ParseBy(Text::String::OrEmpty(command->GetTarget())->ToCString());
				webdriverxx::Element ele = driver.FindElement(by);
				auto element_is_displayed = [&]{
					return ele.IsDisplayed();
				};
				webdriverxx::Duration dur = 5000;
				if (command->GetValue().SetTo(s))
					s->ToUInt32(dur);
				webdriverxx::WaitUntil(element_is_displayed, dur);
			}
			else if (s->Equals(CSTR("waitForElementNotVisible")))
			{
				webdriverxx::By by = SeleniumIDERunner_ParseBy(Text::String::OrEmpty(command->GetTarget())->ToCString());
				webdriverxx::Element ele = driver.FindElement(by);
				auto element_is_displayed = [&]{
					return !ele.IsDisplayed();
				};
				webdriverxx::Duration dur = 5000;
				if (command->GetValue().SetTo(s))
					s->ToUInt32(dur);
				webdriverxx::WaitUntil(element_is_displayed, dur);
			}
			else if (s->Equals(CSTR("waitForElementPresent")))
			{
				webdriverxx::By by = SeleniumIDERunner_ParseBy(Text::String::OrEmpty(command->GetTarget())->ToCString());
				UInt32 dur = 5000;
				if (command->GetValue().SetTo(s))
					s->ToUInt32(dur);
				SeleniumIDERunner_WaitForElement(driver, by, dur);
			}
			else if (s->Equals(CSTR("executeScript")))
			{
				driver.Execute((const Char*)Text::String::OrEmpty(command->GetTarget())->v.Ptr());
			}
			else if (s->Equals(CSTR("click")))
			{
				webdriverxx::By by = SeleniumIDERunner_ParseBy(Text::String::OrEmpty(command->GetTarget())->ToCString());
				webdriverxx::Element ele = driver.FindElement(by);
				ele.Click();
			}
			else if (s->Equals(CSTR("type")))
			{
				webdriverxx::By by = SeleniumIDERunner_ParseBy(Text::String::OrEmpty(command->GetTarget())->ToCString());
				webdriverxx::Element ele = driver.FindElement(by);
				ele.Clear();
				if (command->GetValue().SetTo(s) && s->leng > 0)
				{
					ele.SendKeys((const Char*)s->v.Ptr());
				}
			}
			else if (s->Equals(CSTR("mouseOver")))
			{
				webdriverxx::By by = SeleniumIDERunner_ParseBy(Text::String::OrEmpty(command->GetTarget())->ToCString());
				webdriverxx::Element ele = driver.FindElement(by);
//				driver.MoveToCenterOf(ele);
			}
			else if (s->Equals(CSTR("mouseOut")))
			{
				webdriverxx::By by = SeleniumIDERunner_ParseBy(Text::String::OrEmpty(command->GetTarget())->ToCString());
				webdriverxx::Element ele = driver.FindElement(by);
			}
			else if (s->Equals(CSTR("pause")))
			{
				Int64 dur;
				if (!command->GetTarget().SetTo(s) || !s->ToInt64(dur))
				{
					OPTSTR_DEL(this->lastErrorMsg);
					this->lastErrorMsg = Text::String::New(UTF8STRC("pause invalid target"));
					this->lastErrorIndex = currIndex;
					return false;
				}
				Sync::ThreadUtil::SleepDur(dur);
			}
			else if (s->Equals(CSTR("times")))
			{
				cond[condCnt].type = CondType::Times;
				cond[condCnt].index = currIndex;
				cond[condCnt].param = Text::String::OrEmpty(command->GetTarget())->ToUOSInt();
				skip = cond[condCnt].param == 0;
				condCnt++;
			}
			else
			{
				OPTSTR_DEL(this->lastErrorMsg);
				Text::StringBuilderUTF8 sb;
				sb.Append(CSTR("Unsupported command: "));
				sb.Append(s);
				this->lastErrorMsg = Text::String::New(sb.ToCString());
				this->lastErrorIndex = currIndex;
				return false;
			}
			if (statusHdlr != 0)
			{
				thisTime = Data::Timestamp::UtcNow();
				statusHdlr(userObj, currIndex, thisTime - lastTime);
				lastTime = thisTime;
			}
			currIndex++;
		}
		return true;
	}
	catch (webdriverxx::WebDriverException &ex)
	{
		Text::CStringNN cmd = CSTR("init");
		if (test->GetCommand(currIndex).SetTo(command) && command->GetCommand().SetTo(s))
		{
			cmd = s->ToCString();
		}
		OPTSTR_DEL(this->lastErrorMsg);
		Text::StringBuilderUTF8 sb;
		sb.Append(CSTR("Exception occurred on command "));
		sb.Append(cmd);
		sb.Append(CSTR(": "));
		sb.AppendSlow((const UTF8Char*)ex.what());
		this->lastErrorMsg = Text::String::New(sb.ToCString());
		this->lastErrorIndex = currIndex;
		return false;
	}
}

void IO::SeleniumIDERunner::FillMobileItemSelector(NN<UI::ItemSelector> selector)
{
	auto curr = webdriverxx::chrome::device::deviceList.begin();
	auto end = webdriverxx::chrome::device::deviceList.end();
	while (curr != end)
	{
		std::string s = *curr;
		Text::CStringNN cs = Text::CStringNN((const UTF8Char*)s.c_str(), s.length());
		selector->AddItem(cs, 0);
		curr++;
	}
}
