#ifndef _SM_SSWR_AVIREAD_AVIRWMIFORM
#define _SM_SSWR_AVIREAD_AVIRWMIFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRWMIForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;

			NN<UI::GUILabel> lblNS;
			NN<UI::GUIListBox> lbNS;

			NN<UI::GUIButton> btnOK;
			NN<UI::GUIButton> btnCancel;

			static void __stdcall OnOKClick(AnyType userObj);
			static void __stdcall OnCancelClick(AnyType userObj);
			static void __stdcall OnDblClicked(AnyType userObj);

		public:
			AVIRWMIForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRWMIForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
