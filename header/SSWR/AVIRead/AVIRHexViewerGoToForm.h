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
			UI::GUILabel *lblMessage;
			UI::GUITextBox *txtInput;
			UI::GUIButton *btnCancel;
			UI::GUIButton *btnOK;

			SSWR::AVIRead::AVIRCore *core;
			UInt64 currOfst;
			UInt64 fileLen;

		private:
			static void __stdcall OnOKClicked(void *userObj);
			static void __stdcall OnCancelClicked(void *userObj);

		public:
			AVIRHexViewerGoToForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, UInt64 currOfst, UInt64 fileLen);
			virtual ~AVIRHexViewerGoToForm();

			virtual void OnMonitorChanged();

			UInt64 GetOffset();
		};
	}
}
#endif