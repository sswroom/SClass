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
			NN<UI::GUILabel> lblMessage;
			NN<UI::GUITextBox> txtInput;
			NN<UI::GUIButton> btnCancel;
			NN<UI::GUIButton> btnOK;

			NN<SSWR::AVIRead::AVIRCore> core;
			UInt64 currOfst;
			UInt64 fileLen;

		private:
			static void __stdcall OnOKClicked(AnyType userObj);
			static void __stdcall OnCancelClicked(AnyType userObj);

		public:
			AVIRHexViewerGoToForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, UInt64 currOfst, UInt64 fileLen);
			virtual ~AVIRHexViewerGoToForm();

			virtual void OnMonitorChanged();

			UInt64 GetOffset();
		};
	}
}
#endif
