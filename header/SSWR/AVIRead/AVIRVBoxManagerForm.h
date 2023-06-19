#ifndef _SM_SSWR_AVIREAD_AVIRVBOXMANAGERFORM
#define _SM_SSWR_AVIREAD_AVIRVBOXMANAGERFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRVBoxManagerForm : public UI::GUIForm
		{
		private:
			SSWR::AVIRead::AVIRCore *core;
			UI::GUILabel *lblVersion;
			UI::GUITextBox *txtVersion;

		public:
			AVIRVBoxManagerForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRVBoxManagerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
