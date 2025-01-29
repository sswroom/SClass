#include "Stdafx.h"
#include "webdriverxx.h"
#include "IO/SeleniumIDERunner.h"
#include "Sync/ThreadUtil.h"

webdriverxx::By SeleniumIDERunner_ParseBy(Text::CStringNN target)
{
	if (target.StartsWith(CSTR("xpath=")))
	{
		return webdriverxx::ByXPath((const Char*)target.Substring(6).v.Ptr());
	}
	throw webdriverxx::WebDriverException(std::string("Unknown Target: ") + (const Char*)target.v.Ptr());
}

IO::SeleniumIDERunner::SeleniumIDERunner()
{
}

IO::SeleniumIDERunner::~SeleniumIDERunner()
{
}

Bool IO::SeleniumIDERunner::Run(NN<SeleniumTest> test)
{
	UOSInt currIndex = INVALID_INDEX;
	NN<IO::SeleniumCommand> command;
	NN<Text::String> s;
	UOSInt i;
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	Text::StringBuilderUTF8 sb;
	
	try
	{
		webdriverxx::ChromeOptions options;
		webdriverxx::Chrome chrome;
//		options.SetSpecCompliantProtocol(false);
		options.SetArgs({"start-maximized"});
		chrome.SetChromeOptions(options);
//		chrome.SetJavascriptEnabled(true);
		webdriverxx::WebDriver driver = webdriverxx::Start(chrome, "http://localhost:4444/wd/hub");
		currIndex = 0;
		while (test->GetCommand(currIndex).SetTo(command))
		{
			if (!command->GetCommand().SetTo(s))
			{
				printf("WebDriver error index: %d, unknown command\r\n", (Int32)(OSInt)currIndex);
				driver.DeleteSession();
				return false;
			}
			if (s->Equals(CSTR("open")))
			{
				driver.Navigate((const Char*)Text::String::OrEmpty(command->GetTarget())->v.Ptr());
			}
			else if (s->Equals(CSTR("setWindowSize")))
			{
				if (!command->GetTarget().SetTo(s) || s->leng >= 64)
				{
					printf("WebDriver error index: %d, setWindowsSize invalid target\r\n", (Int32)(OSInt)currIndex);
					driver.DeleteSession();
					return false;
				}
				sptr = s->ConcatTo(sbuff);
				i = s->IndexOf('x');
				if (i == INVALID_INDEX)
				{
					printf("WebDriver error index: %d, setWindowsSize invalid target: %s\r\n", (Int32)(OSInt)currIndex, s->v.Ptr());
					driver.DeleteSession();
					return false;
				}
//				sbuff[i] = 0;
//				sb.ClearStr();
//				sb.Append(CSTR("window.resizeTo("));
//				sb.AppendC(sbuff, i);
//				sb.AppendUTF8Char(',');
//				sb.AppendP(&sbuff[i + 1], sptr);
//				sb.AppendUTF8Char(')');
//				driver.Execute((const Char*)sb.v.Ptr());
//				driver.GetCurrentWindow().Maximize();
//				driver.GetCurrentWindow().SetSize(webdriverxx::Size(Text::StrToInt32(sbuff), Text::StrToInt32(&sbuff[i + 1])));
//				driver.GetCurrentWindow().SetRect(webdriverxx::Rect(0, 0, Text::StrToInt32(sbuff), Text::StrToInt32(&sbuff[i + 1])));
			}
			else if (s->Equals(CSTR("waitForElementVisible")))
			{
				webdriverxx::By by = SeleniumIDERunner_ParseBy(Text::String::OrEmpty(command->GetTarget())->ToCString());
				webdriverxx::Element ele = driver.FindElement(by);
				auto element_is_displayed = [&]{
					return ele.IsDisplayed();
				};
				webdriverxx::WaitUntil(element_is_displayed);
			}
			else
			{
				printf("WebDriver error index: %d, unsupported command %s\r\n", (Int32)(OSInt)currIndex, s->v.Ptr());
				driver.DeleteSession();
				Sync::ThreadUtil::SleepDur(1000);
				return false;
			}
			Sync::ThreadUtil::SleepDur(1000);
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
		printf("WebDriver error index: %d, %s\r\n", (Int32)(OSInt)currIndex, cmd.v.Ptr());
		printf("WebDriver error: %s\r\n", ex.what());
		return false;
	}
}
