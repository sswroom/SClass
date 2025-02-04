#include "Stdafx.h"
#include "IO/SeleniumIDERunner.h"
#include "Net/WebDriver.h"
#include "Sync/ThreadUtil.h"

enum class CondType
{
	Times
};

Bool IO::SeleniumIDERunner::ErrorClient(NN<Net::WebDriverClient> cli, UOSInt currIndex)
{
	OPTSTR_DEL(this->lastErrorMsg);
	this->lastErrorIndex = currIndex;
	Text::StringBuilderUTF8 sb;
	sb.Append(CSTR("Response Code: "));
	sb.AppendUOSInt((UOSInt)cli->GetLastErrorCode());
	sb.Append(CSTR("\r\nError: "));
	sb.AppendOpt(cli->GetLastError());
	sb.Append(CSTR("\r\nMessage: "));
	sb.AppendOpt(cli->GetLastErrorMessage());
	NN<Text::String> s;
	if (cli->GetLastErrorStacktrace().SetTo(s))
	{
		sb.Append(CSTR("\r\nStacktrace: "));
		sb.Append(s);
	}
	this->lastErrorMsg = Text::String::New(sb.ToCString());
	return false;
}

NN<Net::WebDriverBrowserOptions> IO::SeleniumIDERunner::CreateChromeOptions(Text::CString mobileDevice)
{
	Text::CStringNN cstr;
	NN<Net::WebDriverChromeOptions> browser;
	NN<Net::WebDriverTimeouts> timeouts;
	NEW_CLASSNN(browser, Net::WebDriverChromeOptions());
	NEW_CLASSNN(timeouts, Net::WebDriverTimeouts());
	if (mobileDevice.SetTo(cstr))
		browser->SetMobileDeviceName(cstr);
	browser->SetPageLoadStrategy(CSTR("normal"));
	//browser->SetPlatformName(CSTR("ANY"));
	browser->SetTimeouts(timeouts->SetScript(30000)->SetPageLoad(30000)->SetImplicit(5000));
	return browser;
}

NN<Net::WebDriverBrowserOptions> IO::SeleniumIDERunner::CreateMSEdgeOptions(Text::CString mobileDevice)
{
	Text::CStringNN cstr;
	NN<Net::WebDriverMSEdgeOptions> browser;
	NN<Net::WebDriverTimeouts> timeouts;
	NEW_CLASSNN(browser, Net::WebDriverMSEdgeOptions());
	NEW_CLASSNN(timeouts, Net::WebDriverTimeouts());
	if (mobileDevice.SetTo(cstr))
		browser->SetMobileDeviceName(cstr);
	browser->SetPageLoadStrategy(CSTR("normal"));
	browser->SetTimeouts(timeouts->SetScript(30000)->SetPageLoad(30000)->SetImplicit(5000));
	return browser;
}

NN<Net::WebDriverBrowserOptions> IO::SeleniumIDERunner::CreateFirefoxOptions(Text::CString mobileDevice)
{
	NN<Net::WebDriverFirefoxOptions> browser;
	NN<Net::WebDriverTimeouts> timeouts;
	NEW_CLASSNN(browser, Net::WebDriverFirefoxOptions());
	NEW_CLASSNN(timeouts, Net::WebDriverTimeouts());
	browser->SetPageLoadStrategy(CSTR("normal"));
	browser->SetTimeouts(timeouts->SetScript(30000)->SetPageLoad(30000)->SetImplicit(5000));
	return browser;
}

NN<Net::WebDriverBrowserOptions> IO::SeleniumIDERunner::CreateWebKitGTKOptions(Text::CString mobileDevice)
{
	NN<Net::WebDriverWebKitGTKOptions> browser;
	NN<Net::WebDriverTimeouts> timeouts;
	NEW_CLASSNN(browser, Net::WebDriverWebKitGTKOptions());
	NEW_CLASSNN(timeouts, Net::WebDriverTimeouts());
	browser->SetPageLoadStrategy(CSTR("normal"));
	browser->SetTimeouts(timeouts->SetScript(30000)->SetPageLoad(30000)->SetImplicit(5000));
	return browser;
}

NN<Net::WebDriverBrowserOptions> IO::SeleniumIDERunner::CreateOtherOptions(Text::CStringNN browserName)
{
	Text::CStringNN cstr;
	NN<Net::WebDriverW3CBrowserOptions> browser;
	NN<Net::WebDriverTimeouts> timeouts;
	NEW_CLASSNN(browser, Net::WebDriverW3CBrowserOptions(browserName));
	NEW_CLASSNN(timeouts, Net::WebDriverTimeouts());
	browser->SetPageLoadStrategy(CSTR("normal"));
	browser->SetTimeouts(timeouts->SetScript(30000)->SetPageLoad(30000)->SetImplicit(5000));
	return browser;
}

NN<Net::WebDriverBrowserOptions> IO::SeleniumIDERunner::CreateBrowserOptions(BrowserType browserType, Text::CString mobileDevice)
{
	switch (browserType)
	{
	case BrowserType::Chrome:
	default:
		return CreateChromeOptions(mobileDevice);
	case BrowserType::MSEdge:
		return CreateMSEdgeOptions(mobileDevice);
		break;
	case BrowserType::Firefox:
		return CreateFirefoxOptions(mobileDevice);
	case BrowserType::HtmlUnit:
		return CreateOtherOptions(CSTR("htmlunit"));
	case BrowserType::InternetExplorer:
		return CreateOtherOptions(CSTR("internet explorer"));
	case BrowserType::IPad:
		return CreateOtherOptions(CSTR("iPad"));
	case BrowserType::IPhone:
		return CreateOtherOptions(CSTR("iPhone"));
	case BrowserType::Opera:
		return CreateOtherOptions(CSTR("opera"));
	case BrowserType::Safari:
		return CreateOtherOptions(CSTR("safari"));
	case BrowserType::WebKitGTK:
		return CreateWebKitGTKOptions(mobileDevice);
	case BrowserType::Mock:
		return CreateOtherOptions(CSTR("mock"));
	case BrowserType::PhantomJS:
		return CreateOtherOptions(CSTR("phantomjs"));
	}
}

IO::SeleniumIDERunner::SeleniumIDERunner(NN<Net::TCPClientFactory> clif, UInt16 port)
{
	this->clif = clif;
	this->lastErrorIndex = INVALID_INDEX;
	this->lastErrorMsg = 0;
	Text::StringBuilderUTF8 sb;
	sb.Append(CSTR("http://localhost:"));
	sb.AppendU16(port);
	this->url = Text::String::New(sb.ToCString());
}

IO::SeleniumIDERunner::~SeleniumIDERunner()
{
	OPTSTR_DEL(this->lastErrorMsg);
	this->url->Release();
}

Bool IO::SeleniumIDERunner::Run(NN<SeleniumTest> test, BrowserType browserType, Text::CString mobileDevice, Optional<GPSPosition> location, StepStatusHandler statusHdlr, AnyType userObj)
{
	NN<Net::WebDriverBrowserOptions> browser = CreateBrowserOptions(browserType, mobileDevice);
	Net::WebDriverStartSession param(browser);
	NN<Net::WebDriverSession> sess;
	NN<GPSPosition> nnlocation;
	Bool succ = false;
	UOSInt currIndex;
	UOSInt i;
	NN<IO::SeleniumCommand> command;
	NN<Text::String> s;
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	struct {
		CondType type;
		UOSInt index;
		UOSInt param;
	} cond[16];
	UOSInt condCnt = 0;
	Net::WebDriver driver(this->clif, 0, this->url->ToCString());
	if (driver.NewSession(param).SetTo(sess))
	{
		if (location.SetTo(nnlocation))
		{
			sess->ExecuteCdpCommand(CSTR("Emulation.setGeolocationOverride"), Text::JSONObject::New()
				->SetObjectDouble(CSTR("latitude"), nnlocation->latitude)
				->SetObjectDouble(CSTR("longitude"), nnlocation->longitude)
				->SetObjectDouble(CSTR("accuracy"), nnlocation->accuracy));
		}

		Data::Timestamp lastTime = Data::Timestamp::UtcNow();
		Data::Timestamp thisTime;
		Bool skip = false;
		currIndex = 0;
		succ = true;
		while (succ && test->GetCommand(currIndex).SetTo(command))
		{
			if (!command->GetCommand().SetTo(s))
			{
				OPTSTR_DEL(this->lastErrorMsg);
				this->lastErrorMsg = Text::String::New(UTF8STRC("Unknown command"));
				this->lastErrorIndex = currIndex;
				succ = false;
			}
			else if (s->Equals(CSTR("end")))
			{
				if (condCnt == 0)
				{
					OPTSTR_DEL(this->lastErrorMsg);
					this->lastErrorMsg = Text::String::New(UTF8STRC("End found without condition"));
					this->lastErrorIndex = currIndex;
					succ = false;
				}
				else
				{
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
						succ = false;
					}
				}
			}
			else if (skip)
			{

			}
			else if (s->Equals(CSTR("open")))
			{
				if (!sess->NavigateTo(Text::String::OrEmpty(command->GetTarget())->ToCString()))
				{
					succ = this->ErrorClient(sess, currIndex);
				}
				
			}
			else if (s->Equals(CSTR("setWindowSize")))
			{
				if (!command->GetTarget().SetTo(s) || s->leng >= 64)
				{
					OPTSTR_DEL(this->lastErrorMsg);
					this->lastErrorMsg = Text::String::New(UTF8STRC("setWindowsSize invalid target"));
					this->lastErrorIndex = currIndex;
					succ = false;
				}
				else
				{
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
						succ = false;
					}
					else
					{
						sbuff[i] = 0;
						if (!sess->SetWindowRect(Math::RectArea<Int64>(0, 0, Text::StrToInt64(sbuff), Text::StrToInt64(&sbuff[i + 1]))))
						{
							succ = this->ErrorClient(sess, currIndex);
						}
					}
				}
			}
			else if (s->Equals(CSTR("waitForElementVisible")))
			{
				NN<Net::WebDriverBy> by;
				if (!this->ParseBy(Text::String::OrEmpty(command->GetTarget())->ToCString(), currIndex).SetTo(by))
				{
					succ = false;
				}
				else
				{
					NN<Text::String> eleId;
					if (!sess->FindElement(by).SetTo(eleId))
					{
						succ = this->ErrorClient(sess, currIndex);
					}
					else
					{
						auto element_is_displayed = [&]{
							return sess->IsElementDisplayed(eleId->ToCString());
						};
						UInt32 v = 5000;
						if (command->GetValue().SetTo(s))
						{
							s->ToUInt32(v);
						}
						if (!sess->WaitUntil(element_is_displayed, v))
						{
							succ = this->ErrorClient(sess, currIndex);
						}
						eleId->Release();
					}
					by.Delete();
				}
			}
			else if (s->Equals(CSTR("waitForElementNotVisible")))
			{
				NN<Net::WebDriverBy> by;
				if (!this->ParseBy(Text::String::OrEmpty(command->GetTarget())->ToCString(), currIndex).SetTo(by))
				{
					succ = false;
				}
				else
				{
					NN<Text::String> eleId;
					if (!sess->FindElement(by).SetTo(eleId))
					{
						succ = this->ErrorClient(sess, currIndex);
					}
					else
					{
						auto element_is_displayed = [&]{
							return !sess->IsElementDisplayed(eleId->ToCString());
						};
						UInt32 v = 5000;
						if (command->GetValue().SetTo(s))
						{
							s->ToUInt32(v);
						}
						if (!sess->WaitUntil(element_is_displayed, v))
						{
							succ = this->ErrorClient(sess, currIndex);
						}
						eleId->Release();
					}
					by.Delete();
				}
			}
			else if (s->Equals(CSTR("waitForElementPresent")))
			{
				NN<Net::WebDriverBy> by;
				if (!this->ParseBy(Text::String::OrEmpty(command->GetTarget())->ToCString(), currIndex).SetTo(by))
				{
					succ = false;
				}
				else
				{
					Int64 dur = 5000;
					if (command->GetValue().SetTo(s))
						s->ToInt64(dur);
					Data::Timestamp startTime = Data::Timestamp::UtcNow();
					NN<Text::String> eleId;
					while (true)
					{
						if (sess->FindElement(by).SetTo(eleId))
						{
							eleId->Release();
							break;
						}
						else if (Data::Timestamp::UtcNow().DiffMS(startTime) > dur)
						{
							succ = this->ErrorClient(sess, currIndex);
							break;
						}
						Sync::ThreadUtil::SleepDur(50);
					}
					by.Delete();
				}
			}
			else if (s->Equals(CSTR("executeScript")))
			{
				succ = sess->ExecuteScript(Text::String::OrEmpty(command->GetTarget())->ToCString());
			}
			else if (s->Equals(CSTR("click")))
			{
				NN<Net::WebDriverBy> by;
				if (!this->ParseBy(Text::String::OrEmpty(command->GetTarget())->ToCString(), currIndex).SetTo(by))
				{
					succ = false;
				}
				else
				{
					NN<Text::String> eleId;
					if (!sess->FindElement(by).SetTo(eleId))
					{
						succ = this->ErrorClient(sess, currIndex);
					}
					else
					{
						if (!sess->ElementClick(eleId->ToCString()))
						{
							succ = this->ErrorClient(sess, currIndex);
						}
						eleId->Release();
					}
					by.Delete();
				}
			}
			else if (s->Equals(CSTR("type")))
			{
				NN<Net::WebDriverBy> by;
				if (!this->ParseBy(Text::String::OrEmpty(command->GetTarget())->ToCString(), currIndex).SetTo(by))
				{
					succ = false;
				}
				else
				{
					NN<Text::String> eleId;
					if (!sess->FindElement(by).SetTo(eleId))
					{
						succ = this->ErrorClient(sess, currIndex);
					}
					else
					{
						if (!sess->ElementClear(eleId->ToCString()))
						{
							succ = this->ErrorClient(sess, currIndex);
						}
						if (succ && command->GetValue().SetTo(s) && s->leng > 0)
						{
							if (!sess->ElementSendKeys(eleId->ToCString(), s->ToCString()))
							{
								succ = this->ErrorClient(sess, currIndex);
							}
						}
						eleId->Release();
					}
					by.Delete();
				}
			}
			else if (s->Equals(CSTR("mouseOver")))
			{
				NN<Net::WebDriverBy> by;
				if (!this->ParseBy(Text::String::OrEmpty(command->GetTarget())->ToCString(), currIndex).SetTo(by))
				{
					succ = false;
				}
				else
				{
					NN<Text::String> eleId;
					if (!sess->FindElement(by).SetTo(eleId))
					{
						succ = this->ErrorClient(sess, currIndex);
					}
					else
					{
//						sess->MoveToCenterOf(eleId);
						eleId->Release();
					}
					by.Delete();
				}
			}
			else if (s->Equals(CSTR("mouseOut")))
			{
				NN<Net::WebDriverBy> by;
				if (!this->ParseBy(Text::String::OrEmpty(command->GetTarget())->ToCString(), currIndex).SetTo(by))
				{
					succ = false;
				}
				else
				{
					NN<Text::String> eleId;
					if (!sess->FindElement(by).SetTo(eleId))
					{
						succ = this->ErrorClient(sess, currIndex);
					}
					else
					{
						eleId->Release();
					}
					by.Delete();
				}
			}
			else if (s->Equals(CSTR("pause")))
			{
				Int64 dur;
				if (!command->GetTarget().SetTo(s) || !s->ToInt64(dur))
				{
					OPTSTR_DEL(this->lastErrorMsg);
					this->lastErrorMsg = Text::String::New(UTF8STRC("pause invalid target"));
					this->lastErrorIndex = currIndex;
					succ = false;
				}
				else
				{
					Sync::ThreadUtil::SleepDur(dur);
				}
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
				succ = false;
			}
			if (succ && statusHdlr != 0)
			{
				thisTime = Data::Timestamp::UtcNow();
				statusHdlr(userObj, currIndex, thisTime - lastTime);
				lastTime = thisTime;
			}
			//Sync::ThreadUtil::SleepDur(1000);
			currIndex++;
		}

		sess.Delete();
	}
	else
	{
		this->ErrorClient(driver, INVALID_INDEX);
	}
	return succ;
}

Optional<Net::WebDriverBy> IO::SeleniumIDERunner::ParseBy(Text::CStringNN by, UOSInt currIndex)
{
	NN<Net::WebDriverBy> ret;
	if (Net::WebDriver::ParseBy(by).SetTo(ret))
	{
		return ret;
	}
	else
	{
		OPTSTR_DEL(this->lastErrorMsg);
		Text::StringBuilderUTF8 sb;
		sb.Append(CSTR("Unknown target: "));
		sb.Append(by);
		this->lastErrorMsg = Text::String::New(sb.ToCString());
		this->lastErrorIndex = currIndex;
		return 0;
	}
}

void IO::SeleniumIDERunner::FillMobileItemSelector(NN<UI::ItemSelector> selector)
{
	selector->AddItem(CSTR("BlackBerry Z30"), 0);
	selector->AddItem(CSTR("Blackberry PlayBook"), 0);
	selector->AddItem(CSTR("Galaxy Note 3"), 0);
	selector->AddItem(CSTR("Galaxy Note II"), 0);
	selector->AddItem(CSTR("Galaxy S III"), 0);
	selector->AddItem(CSTR("Galaxy S8"), 0);
	selector->AddItem(CSTR("Galaxy S9+"), 0);
	selector->AddItem(CSTR("Galaxy Tab S4"), 0);
	selector->AddItem(CSTR("Kindle Fire HDX"), 0);
	selector->AddItem(CSTR("LG Optimus L70"), 0);
	selector->AddItem(CSTR("Laptop with HiDPI screen"), 0);
	selector->AddItem(CSTR("Laptop with MDPI screen"), 0);
	selector->AddItem(CSTR("Laptop with touch"), 0);
	selector->AddItem(CSTR("Microsoft Lumia 550"), 0);
	selector->AddItem(CSTR("Microsoft Lumia 950"), 0);
	selector->AddItem(CSTR("Moto G Power"), 0);
	selector->AddItem(CSTR("Moto G4"), 0);
	selector->AddItem(CSTR("Nexus 10"), 0);
	selector->AddItem(CSTR("Nexus 4"), 0);
	selector->AddItem(CSTR("Nexus 5"), 0);
	selector->AddItem(CSTR("Nexus 5X"), 0);
	selector->AddItem(CSTR("Nexus 6"), 0);
	selector->AddItem(CSTR("Nexus 6P"), 0);
	selector->AddItem(CSTR("Nexus 7"), 0);
	selector->AddItem(CSTR("Nokia Lumia 520"), 0);
	selector->AddItem(CSTR("Nokia N9"), 0);
	selector->AddItem(CSTR("Pixel 3"), 0);
	selector->AddItem(CSTR("Pixel 4"), 0);
	selector->AddItem(CSTR("JioPhone 2"), 0);
	selector->AddItem(CSTR("iPhone SE"), 0);
	selector->AddItem(CSTR("iPhone XR"), 0);
	selector->AddItem(CSTR("iPhone 12 Pro"), 0);
	selector->AddItem(CSTR("iPhone 14 Pro Max"), 0);
	selector->AddItem(CSTR("Pixel 3 XL"), 0);
	selector->AddItem(CSTR("Pixel 7"), 0);
	selector->AddItem(CSTR("Samsung Galaxy S8+"), 0);
	selector->AddItem(CSTR("Samsung Galaxy S20 Ultra"), 0);
	selector->AddItem(CSTR("iPad Mini"), 0);
	selector->AddItem(CSTR("iPad Air"), 0);
	selector->AddItem(CSTR("iPad Pro"), 0);
	selector->AddItem(CSTR("Surface Pro 7"), 0);
	selector->AddItem(CSTR("Surface Duo"), 0);
	selector->AddItem(CSTR("Galaxy Z Fold 5"), 0);
	selector->AddItem(CSTR("Asus Zenbook Fold"), 0);
	selector->AddItem(CSTR("Samsung Galaxy A51/71"), 0);
	selector->AddItem(CSTR("Nest Hub"), 0);
	selector->AddItem(CSTR("Nest Hub Max"), 0);
	selector->AddItem(CSTR("Galaxy S5"), 0);
	selector->AddItem(CSTR("Pixel 2"), 0);
	selector->AddItem(CSTR("Pixel 2 XL"), 0);
	selector->AddItem(CSTR("iPhone 4"), 0);
	selector->AddItem(CSTR("iPhone 5/SE"), 0);
	selector->AddItem(CSTR("iPhone 6/7/8"), 0);
	selector->AddItem(CSTR("iPhone 6/7/8 Plus"), 0);
	selector->AddItem(CSTR("iPhone X"), 0);
	selector->AddItem(CSTR("iPad"), 0);
	selector->AddItem(CSTR("iPad Pro"), 0);
	selector->AddItem(CSTR("Galaxy Fold"), 0);
	selector->AddItem(CSTR("Facebook on Android"), 0);
}

Text::CStringNN IO::SeleniumIDERunner::BrowserTypeGetName(BrowserType val)
{
	switch (val)
	{
	case BrowserType::Chrome:
		return CSTR("Chrome");
	case BrowserType::MSEdge:
		return CSTR("MSEdge");
	case BrowserType::Firefox:
		return CSTR("Firefox");
	case BrowserType::HtmlUnit:
		return CSTR("HtmlUnit");
	case BrowserType::InternetExplorer:
		return CSTR("InternetExplorer");
	case BrowserType::IPad:
		return CSTR("IPad");
	case BrowserType::IPhone:
		return CSTR("IPhone");
	case BrowserType::Opera:
		return CSTR("Opera");
	case BrowserType::Safari:
		return CSTR("Safari");
	case BrowserType::WebKitGTK:
		return CSTR("WebKitGTK");
	case BrowserType::Mock:
		return CSTR("Mock");
	case BrowserType::PhantomJS:
		return CSTR("PhantomJS");
	default:
		return CSTR("Unknown");
	}
}
