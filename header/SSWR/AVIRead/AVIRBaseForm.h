#ifndef _SM_SSWR_AVIREAD_AVIRBASEFORM
#define _SM_SSWR_AVIREAD_AVIRBASEFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIForm.h"
#include "UI/GUIMainMenu.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRBaseForm : public UI::GUIForm
		{
		private:
			SSWR::AVIRead::AVIRCore *core;
			UI::GUIMainMenu *mnuMain;

		private:
			static void __stdcall FileHandler(void *userObj, const UTF8Char **files, UOSInt nFiles);
		public:
			AVIRBaseForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRBaseForm();

			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();
		};
	}
}
#endif
