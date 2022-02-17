#ifndef _SM_SSWR_AVIREAD_AVIRMACMANAGERENTRYFORM
#define _SM_SSWR_AVIREAD_AVIRMACMANAGERENTRYFORM
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
		class AVIRMACManagerEntryForm : public UI::GUIForm
		{
		private:
			SSWR::AVIRead::AVIRCore *core;

			UI::GUILabel *lblMAC;
			UI::GUITextBox *txtMAC;
			UI::GUILabel *lblName;
			UI::GUIComboBox *cboName;
			UI::GUIButton *btnCancel;
			UI::GUIButton *btnOK;
			Text::String *name;

			static void __stdcall OnOKClicked(void *userObj);
			static void __stdcall OnCancelClicked(void *userObj);
		public:
			AVIRMACManagerEntryForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, const UInt8 *mac, Text::CString name);
			virtual ~AVIRMACManagerEntryForm();

			virtual void OnMonitorChanged();

			Text::String *GetNameNew();
		};
	}
}
#endif
