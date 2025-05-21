#ifndef _SM_IO_SELENIUMIDERUNNER
#define _SM_IO_SELENIUMIDERUNNER
#include "IO/SeleniumIDE.h"
#include "Net/TCPClientFactory.h"
#include "Net/WebDriver.h"
#include "UI/ItemSelector.h"

namespace IO
{
	class SeleniumIDERunner
	{
	public:
		struct GPSPosition
		{
			Double latitude;
			Double longitude;
			Double accuracy;
		};

		enum class BrowserType
		{
			Chrome,
			MSEdge,
			Firefox,
			HtmlUnit,
			InternetExplorer,
			IPad,
			IPhone,
			Opera,
			Safari,
			WebKitGTK,
			Mock,
			PhantomJS
		};

		struct RunOptions;

		typedef void (__stdcall *StepStatusHandler)(AnyType userObj, UOSInt cmdIndex, Data::Duration dur);
	private:
		NN<Net::TCPClientFactory> clif;
		UOSInt lastErrorIndex;
		Optional<Text::String> lastErrorMsg;
		NN<Text::String> webdriverURL;
		Optional<Text::String> userDataDir;
		Bool noPause;

		Bool ErrorClient(NN<Net::WebDriverClient> cli, UOSInt currIndex);
		static NN<Net::WebDriverBrowserOptions> CreateChromeOptions(Text::CString mobileDevice, NN<RunOptions> options);
		static NN<Net::WebDriverBrowserOptions> CreateMSEdgeOptions(Text::CString mobileDevice, NN<RunOptions> options);
		static NN<Net::WebDriverBrowserOptions> CreateFirefoxOptions(Text::CString mobileDevice, NN<RunOptions> options);
		static NN<Net::WebDriverBrowserOptions> CreateWebKitGTKOptions(Text::CString mobileDevice, NN<RunOptions> options);
		static NN<Net::WebDriverBrowserOptions> CreateOtherOptions(Text::CStringNN browserName, NN<RunOptions> options);
		static NN<Net::WebDriverBrowserOptions> CreateBrowserOptions(BrowserType browserType, Text::CString mobileDevice, NN<RunOptions> options);
	public:
		SeleniumIDERunner(NN<Net::TCPClientFactory> clif, UInt16 port);
		~SeleniumIDERunner();

		Optional<Net::WebDriverSession> BeginTest(BrowserType browserType, Text::CString mobileDevice, Optional<GPSPosition> location, Text::CStringNN url, NN<RunOptions> options);
		Bool RunTest(NN<Net::WebDriverSession> sess, NN<SeleniumTest> test, Text::CStringNN url, StepStatusHandler statusHdlr, AnyType userObj);
		Bool Run(NN<SeleniumTest> test, BrowserType browserType, Text::CString mobileDevice, Optional<GPSPosition> location, Text::CStringNN url, NN<RunOptions> options, StepStatusHandler statusHdlr, AnyType userObj);
		void SetURL(Text::CStringNN url) { this->webdriverURL->Release(); this->webdriverURL = Text::String::New(url); }
		void SetNoPause(Bool noPause) { this->noPause = noPause; }
		void SetUserDataDir(Text::CStringNN userDataDir) { OPTSTR_DEL(this->userDataDir); this->userDataDir = Text::String::New(userDataDir); }
		Optional<Text::String> GetLastErrorMsg() const { return this->lastErrorMsg; }
		UOSInt GetLastErrorIndex() const { return this->lastErrorIndex; }
		Optional<Net::WebDriverBy> ParseBy(Text::CStringNN by, UOSInt currIndex);
		Optional<Net::WebDriverBy> ParseOptionLocator(Text::CStringNN locator, UOSInt currIndex);
		static void FillMobileItemSelector(NN<UI::ItemSelector> selector);
		static Text::CStringNN BrowserTypeGetName(BrowserType browserType);
		static Int64 GetDefaultScriptTimeout() { return 30000; }
		static Int64 GetDefaultPageLoadTimeout() { return 30000; }
		static Int64 GetDefaultImplicitTimeout() { return 1000; }
		static Int64 GetDefaultCommandTimeout() { return 15000; }

		struct RunOptions
		{
			Bool headless;
			Bool disableGPU;
			Bool noSandbox;
			Int64 scriptTimeout;
			Int64 pageLoadTimeout;
			Int64 implicitTimeout;
			Int64 cmdTimeout;

			RunOptions()
			{
				this->headless = false;
				this->disableGPU = false;
				this->noSandbox = false;
				this->scriptTimeout = GetDefaultScriptTimeout();
				this->pageLoadTimeout = GetDefaultPageLoadTimeout();
				this->implicitTimeout = GetDefaultImplicitTimeout();
				this->cmdTimeout = GetDefaultCommandTimeout();
			}
		};
	};
}
#endif
