#include "Stdafx.h"
#include "webdriverxx.h"
#include "IO/SeleniumIDERunner.h"
#include "Sync/ThreadUtil.h"

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
//		options = options.SetSpecCompliantProtocol(false);
		chrome.SetChromeOptions(options);
		webdriverxx::WebDriver driver = webdriverxx::Start(chrome);
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
				sbuff[i] = 0;
				sb.ClearStr();
				sb.Append(CSTR("window.resizeTo("));
				sb.AppendC(sbuff, i);
				sb.AppendUTF8Char(',');
				sb.AppendP(&sbuff[i + 1], sptr);
				sb.AppendUTF8Char(')');
				driver.Execute((const Char*)sb.v.Ptr());
//				driver.GetCurrentWindow().Maximize();
				//driver.GetCurrentWindow().SetSize(webdriverxx::Size(Text::StrToInt32(sbuff), Text::StrToInt32(&sbuff[i + 1])));
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
