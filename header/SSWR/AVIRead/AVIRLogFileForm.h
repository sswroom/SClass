#ifndef _SM_SSWR_AVIREAD_AVIRLOGFILEFORM
#define _SM_SSWR_AVIREAD_AVIRLOGFILEFORM
#include "IO/LogFile.h"
#include "UI/GUIForm.h"
#include "UI/GUIListView.h"
#include "SSWR/AVIRead/AVIRCore.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRLogFileForm : public UI::GUIForm
		{
		private:
			UI::GUIListView *lvLogs;
			UI::GUIMainMenu *mnuMain;

			SSWR::AVIRead::AVIRCore *core;
			IO::LogFile *logFile;
			IO::LogHandler::LogLevel logLevel;

			void UpdateLogMessages();
			static void __stdcall OnLogsDblClk(void *userObj, UOSInt itemIndex);
		public:
			AVIRLogFileForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, IO::LogFile *logFile);
			virtual ~AVIRLogFileForm();

			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();
		};
	}
}
#endif
