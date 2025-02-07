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

		typedef void (__stdcall *StepStatusHandler)(AnyType userObj, UOSInt cmdIndex, Data::Duration dur);
	private:
		NN<Net::TCPClientFactory> clif;
		UOSInt lastErrorIndex;
		Optional<Text::String> lastErrorMsg;
		NN<Text::String> url;
		Bool noPause;

		Bool ErrorClient(NN<Net::WebDriverClient> cli, UOSInt currIndex);
		static NN<Net::WebDriverBrowserOptions> CreateChromeOptions(Text::CString mobileDevice);
		static NN<Net::WebDriverBrowserOptions> CreateMSEdgeOptions(Text::CString mobileDevice);
		static NN<Net::WebDriverBrowserOptions> CreateFirefoxOptions(Text::CString mobileDevice);
		static NN<Net::WebDriverBrowserOptions> CreateWebKitGTKOptions(Text::CString mobileDevice);
		static NN<Net::WebDriverBrowserOptions> CreateOtherOptions(Text::CStringNN browserName);
		static NN<Net::WebDriverBrowserOptions> CreateBrowserOptions(BrowserType browserType, Text::CString mobileDevice);
	public:
		SeleniumIDERunner(NN<Net::TCPClientFactory> clif, UInt16 port);
		~SeleniumIDERunner();

		Bool Run(NN<SeleniumTest> test, BrowserType browserType, Text::CString mobileDevice, Optional<GPSPosition> location, Text::CStringNN url, StepStatusHandler statusHdlr, AnyType userObj);
		void SetURL(Text::CStringNN url) { this->url->Release(); this->url = Text::String::New(url); }
		void SetNoPause(Bool noPause) { this->noPause = noPause; }
		Optional<Text::String> GetLastErrorMsg() const { return this->lastErrorMsg; }
		UOSInt GetLastErrorIndex() const { return this->lastErrorIndex; }
		Optional<Net::WebDriverBy> ParseBy(Text::CStringNN by, UOSInt currIndex);
		Optional<Net::WebDriverBy> ParseOptionLocator(Text::CStringNN locator, UOSInt currIndex);
		static void FillMobileItemSelector(NN<UI::ItemSelector> selector);
		static Text::CStringNN BrowserTypeGetName(BrowserType browserType);
	};
}
#endif
