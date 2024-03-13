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
			NotNullPtr<UI::GUIListView> lvLogs;
			NotNullPtr<UI::GUIMainMenu> mnuMain;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			IO::LogFile *logFile;
			IO::LogHandler::LogLevel logLevel;

			void UpdateLogMessages();
			static void __stdcall OnLogsDblClk(void *userObj, UOSInt itemIndex);
		public:
			AVIRLogFileForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, IO::LogFile *logFile);
			virtual ~AVIRLogFileForm();

			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();
		};
	}
}
#endif
