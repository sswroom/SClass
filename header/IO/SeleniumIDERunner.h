#ifndef _SM_IO_SELENIUMIDERUNNER
#define _SM_IO_SELENIUMIDERUNNER
#include "IO/SeleniumIDE.h"
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

		typedef void (__stdcall *StepStatusHandler)(AnyType userObj, UOSInt cmdIndex, Data::Duration dur);
	private:
		UOSInt lastErrorIndex;
		Optional<Text::String> lastErrorMsg;
		NN<Text::String> url;
	public:
		SeleniumIDERunner();
		~SeleniumIDERunner();

		Bool Run(NN<SeleniumTest> test, Text::CString mobileDevice, Optional<GPSPosition> location, StepStatusHandler statusHdlr, AnyType userObj);
		void SetURL(Text::CStringNN url) { this->url->Release(); this->url = Text::String::New(url); }
		Optional<Text::String> GetLastErrorMsg() const { return this->lastErrorMsg; }
		UOSInt GetLastErrorIndex() const { return this->lastErrorIndex; }
		static void FillMobileItemSelector(NN<UI::ItemSelector> selector);
	};
}
#endif
