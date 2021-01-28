#ifndef _SM_UI_LISTBOXLOGGER
#define _SM_UI_LISTBOXLOGGER
#include "Sync/Mutex.h"
#include "IO/LogTool.h"
#include "UI/GUIListBox.h"
#include "UI/GUIForm.h"

namespace UI
{
	class ListBoxLogger : public IO::ILogHandler
	{
	private:
		Sync::Mutex *mut;
		UI::GUIListBox *lb;
		UI::GUITimer *tmr;
		UI::GUIForm *frm;
		Int32 maxLog;
		Bool reverse;
		const UTF8Char **logArr;
		const UTF8Char **tmpLogArr;
		const Char *timeFormat;
		OSInt logIndex;
		Int32 logCnt;

		static void __stdcall TimerTick(void *userObj);
	public:
		ListBoxLogger(UI::GUIForm *frm, UI::GUIListBox *lb, Int32 maxLog, Bool reverse);
		virtual ~ListBoxLogger();
		virtual void LogClosed();
		virtual void LogAdded(Data::DateTime *logTime, const UTF8Char *logMsg, IO::ILogHandler::LogLevel logLev);

		void SetTimeFormat(const Char *timeFormat);
	};
}
#endif
