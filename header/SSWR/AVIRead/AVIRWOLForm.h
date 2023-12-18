#ifndef _SM_SSWR_AVIREAD_AVIRWOLFORM
#define _SM_SSWR_AVIREAD_AVIRWOLFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRWOLForm : public UI::GUIForm
		{
		private:
			UI::GUILabel *lblAdapter;
			UI::GUIComboBox *cboAdapter;
			UI::GUILabel *lblDeviceMac;
			UI::GUITextBox *txtDeviceMac;
			NotNullPtr<UI::GUIButton> btnSend;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			static void __stdcall OnSendClicked(void *userObj);
		public:
			AVIRWOLForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRWOLForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
