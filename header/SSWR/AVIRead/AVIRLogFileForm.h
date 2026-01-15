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
			NN<UI::GUIListView> lvLogs;
			NN<UI::GUIMainMenu> mnuMain;

			NN<SSWR::AVIRead::AVIRCore> core;
			NN<IO::LogFile> logFile;
			IO::LogHandler::LogLevel logLevel;

			void UpdateLogMessages();
			static void __stdcall OnLogsDblClk(AnyType userObj, UIntOS itemIndex);
		public:
			AVIRLogFileForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<IO::LogFile> logFile);
			virtual ~AVIRLogFileForm();

			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();
		};
	}
}
#endif
