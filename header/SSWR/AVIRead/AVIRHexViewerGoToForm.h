#ifndef _SM_SSWR_AVIREAD_AVIRHEXVIEWERGOTOFORM
#define _SM_SSWR_AVIREAD_AVIRHEXVIEWERGOTOFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRHexViewerGoToForm : public UI::GUIForm
		{
		private:
			NotNullPtr<UI::GUILabel> lblMessage;
			NotNullPtr<UI::GUITextBox> txtInput;
			NotNullPtr<UI::GUIButton> btnCancel;
			NotNullPtr<UI::GUIButton> btnOK;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			UInt64 currOfst;
			UInt64 fileLen;

		private:
			static void __stdcall OnOKClicked(void *userObj);
			static void __stdcall OnCancelClicked(void *userObj);

		public:
			AVIRHexViewerGoToForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, UInt64 currOfst, UInt64 fileLen);
			virtual ~AVIRHexViewerGoToForm();

			virtual void OnMonitorChanged();

			UInt64 GetOffset();
		};
	}
}
#endif
