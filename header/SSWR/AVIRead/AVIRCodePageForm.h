#ifndef _SM_SSWR_AVIREAD_AVIRCODEPAGEFORM
#define _SM_SSWR_AVIREAD_AVIRCODEPAGEFORM
#include "UI/GUIForm.h"
#include "UI/GUIListBox.h"
#include "SSWR/AVIRead/AVIRCore.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRCodePageForm : public UI::GUIForm
		{
		private:
			NN<UI::GUIListBox> lbCodePages;
			NN<UI::GUIButton> btnOK;
			NN<UI::GUIButton> btnCancel;
			NN<SSWR::AVIRead::AVIRCore> core;

			static void __stdcall OKClicked(AnyType userObj);
			static void __stdcall CancelClicked(AnyType userObj);
		public:
			AVIRCodePageForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRCodePageForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
