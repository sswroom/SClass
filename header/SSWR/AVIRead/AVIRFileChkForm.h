#ifndef _SM_SSWR_AVIREAD_AVIRFILECHKFORM
#define _SM_SSWR_AVIREAD_AVIRFILECHKFORM
#include "IO/FileCheck.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIForm.h"
#include "UI/GUIListView.h"
#include "UI/GUIMainMenu.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRFileChkForm : public UI::GUIForm
		{
		private:
			NN<UI::GUIMainMenu> mnu;
			NN<UI::GUIListView> lvFileChk;

			NN<SSWR::AVIRead::AVIRCore> core;
			NN<IO::FileCheck> fileChk;

		public:
			AVIRFileChkForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<IO::FileCheck> fileChk);
			virtual ~AVIRFileChkForm();

			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();
		};
	}
}
#endif
