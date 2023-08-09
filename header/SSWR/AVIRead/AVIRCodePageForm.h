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
			UI::GUIListBox *lbCodePages;
			UI::GUIButton *btnOK;
			UI::GUIButton *btnCancel;
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			static void __stdcall OKClicked(void *userObj);
			static void __stdcall CancelClicked(void *userObj);
		public:
			AVIRCodePageForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRCodePageForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
