#ifndef _SM_UI_LISTBOXLOGGER
#define _SM_UI_LISTBOXLOGGER
#include "IO/LogTool.h"
#include "Sync/Mutex.h"
#include "UI/GUIForm.h"
#include "UI/GUIListBox.h"
#include "UI/GUITextBox.h"

namespace UI
{
	class ListBoxLogger : public IO::LogHandler
	{
	private:
		Sync::Mutex mut;
		NN<UI::GUIListBox> lb;
		NN<UI::GUITimer> tmr;
		NN<UI::GUIForm> frm;
		UOSInt maxLog;
		Bool reverse;
		Text::String **logArr;
		Text::String **tmpLogArr;
		const Char *timeFormat;
		UOSInt logIndex;
		UInt32 logCnt;
		Optional<UI::GUITextBox> txt;

		static void __stdcall TimerTick(AnyType userObj);
		static void __stdcall OnListBoxSelChg(AnyType userObj);
	public:
		ListBoxLogger(NN<UI::GUIForm> frm, NN<UI::GUIListBox> lb, UOSInt maxLog, Bool reverse);
		virtual ~ListBoxLogger();
		virtual void LogClosed();
		virtual void LogAdded(const Data::Timestamp &logTime, Text::CStringNN logMsg, IO::LogHandler::LogLevel logLev);

		void SetTimeFormat(const Char *timeFormat);

		static NN<ListBoxLogger> CreateUI(NN<UI::GUIForm> frm, NN<UI::GUICore> ui, NN<UI::GUIClientControl> ctrl, UOSInt maxLog, Bool reverse);
	};
}
#endif
