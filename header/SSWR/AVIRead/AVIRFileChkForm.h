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
			UI::GUIMainMenu *mnu;
			UI::GUIListView *lvFileChk;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			NotNullPtr<IO::FileCheck> fileChk;

		public:
			AVIRFileChkForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<IO::FileCheck> fileChk);
			virtual ~AVIRFileChkForm();

			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();
		};
	}
}
#endif
