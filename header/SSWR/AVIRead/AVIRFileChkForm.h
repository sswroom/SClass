#ifndef _SM_SSWR_AVIREAD_AVIRFILECHKFORM
#define _SM_SSWR_AVIREAD_AVIRFILECHKFORM
#include "IO/FileCheck.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIMainMenu.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRFileChkForm : public UI::GUIForm, public IO::ProgressHandler
		{
		private:
			NN<UI::GUIMainMenu> mnu;
			NN<UI::GUIPanel> pnlStatus;
			NN<UI::GUILabel> lblStatus;
			NN<UI::GUITextBox> txtStatus;
			NN<UI::GUILabel> lblTotalFiles;
			NN<UI::GUITextBox> txtTotalFiles;
			NN<UI::GUILabel> lblValidFiles;
			NN<UI::GUITextBox> txtValidFiles;
			NN<UI::GUIListView> lvFileChk;

			NN<SSWR::AVIRead::AVIRCore> core;
			NN<IO::FileCheck> fileChk;

		public:
			AVIRFileChkForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<IO::FileCheck> fileChk);
			virtual ~AVIRFileChkForm();

			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();

			virtual void ProgressStart(Text::CStringNN name, UInt64 count);
			virtual void ProgressUpdate(UInt64 currCount, UInt64 newCount);
			virtual void ProgressEnd();
		};
	}
}
#endif
