#include "Stdafx.h"
#include "IO/SeleniumIDERunner.h"
#include "Net/WebDriver.h"
#include "Sync/ThreadUtil.h"

enum class CondType
{
	Times
};

Bool IO::SeleniumIDERunner::ErrorSess(NN<Net::WebDriverSession> sess, UOSInt currIndex)
{
	OPTSTR_DEL(this->lastErrorMsg);
	this->lastErrorIndex = currIndex;
	Text::StringBuilderUTF8 sb;
	sb.Append(CSTR("Response Code: "));
	sb.AppendUOSInt((UOSInt)sess->GetLastErrorCode());
	sb.Append(CSTR("\r\nError: "));
	sb.AppendOpt(sess->GetLastError());
	sb.Append(CSTR("\r\nMessage: "));
	sb.AppendOpt(sess->GetLastErrorMessage());
	NN<Text::String> s;
	if (sess->GetLastErrorStacktrace().SetTo(s))
	{
		sb.Append(CSTR("\r\nStacktrace: "));
		sb.Append(s);
	}
	this->lastErrorMsg = Text::String::New(sb.ToCString());
	return false;
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

Bool IO::SeleniumIDERunner::Run(NN<SeleniumTest> test, Text::CString mobileDevice, Optional<GPSPosition> location, StepStatusHandler statusHdlr, AnyType userObj)
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
					succ = this->ErrorSess(sess, currIndex);
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
							succ = this->ErrorSess(sess, currIndex);
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
						succ = this->ErrorSess(sess, currIndex);
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
							succ = this->ErrorSess(sess, currIndex);
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
						succ = this->ErrorSess(sess, currIndex);
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
							succ = this->ErrorSess(sess, currIndex);
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
							succ = this->ErrorSess(sess, currIndex);
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
						succ = this->ErrorSess(sess, currIndex);
					}
					else
					{
						if (!sess->ElementClick(eleId->ToCString()))
						{
							succ = this->ErrorSess(sess, currIndex);
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
						succ = this->ErrorSess(sess, currIndex);
					}
					else
					{
						if (!sess->ElementClear(eleId->ToCString()))
						{
							succ = this->ErrorSess(sess, currIndex);
						}
						if (succ && command->GetValue().SetTo(s) && s->leng > 0)
						{
							if (!sess->ElementSendKeys(eleId->ToCString(), s->ToCString()))
							{
								succ = this->ErrorSess(sess, currIndex);
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
						succ = this->ErrorSess(sess, currIndex);
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
						succ = this->ErrorSess(sess, currIndex);
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
}
