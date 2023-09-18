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
		NotNullPtr<UI::GUIListBox> lb;
		NotNullPtr<UI::GUITimer> tmr;
		NotNullPtr<UI::GUIForm> frm;
		UOSInt maxLog;
		Bool reverse;
		Text::String **logArr;
		Text::String **tmpLogArr;
		const Char *timeFormat;
		UOSInt logIndex;
		UInt32 logCnt;
		UI::GUITextBox *txt;

		static void __stdcall TimerTick(void *userObj);
		static void __stdcall OnListBoxSelChg(void *userObj);
	public:
		ListBoxLogger(NotNullPtr<UI::GUIForm> frm, NotNullPtr<UI::GUIListBox> lb, UOSInt maxLog, Bool reverse);
		virtual ~ListBoxLogger();
		virtual void LogClosed();
		virtual void LogAdded(const Data::Timestamp &logTime, Text::CStringNN logMsg, IO::LogHandler::LogLevel logLev);

		void SetTimeFormat(const Char *timeFormat);

		static NotNullPtr<ListBoxLogger> CreateUI(NotNullPtr<UI::GUIForm> frm, NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIClientControl> ctrl, UOSInt maxLog, Bool reverse);
	};
}
#endif
