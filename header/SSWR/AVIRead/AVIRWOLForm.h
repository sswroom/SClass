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
			NotNullPtr<UI::GUILabel> lblAdapter;
			NotNullPtr<UI::GUIComboBox> cboAdapter;
			NotNullPtr<UI::GUILabel> lblDeviceMac;
			NotNullPtr<UI::GUITextBox> txtDeviceMac;
			NotNullPtr<UI::GUIButton> btnSend;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			static void __stdcall OnSendClicked(AnyType userObj);
		public:
			AVIRWOLForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRWOLForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
