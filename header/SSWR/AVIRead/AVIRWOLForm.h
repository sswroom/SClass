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
			NN<UI::GUILabel> lblAdapter;
			NN<UI::GUIComboBox> cboAdapter;
			NN<UI::GUILabel> lblDeviceMac;
			NN<UI::GUITextBox> txtDeviceMac;
			NN<UI::GUIButton> btnSend;

			NN<SSWR::AVIRead::AVIRCore> core;

			static void __stdcall OnSendClicked(AnyType userObj);
		public:
			AVIRWOLForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRWOLForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
