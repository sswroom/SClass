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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			NotNullPtr<UI::GUILabel> lblMAC;
			NotNullPtr<UI::GUITextBox> txtMAC;
			NotNullPtr<UI::GUILabel> lblName;
			NotNullPtr<UI::GUIComboBox> cboName;
			NotNullPtr<UI::GUIButton> btnCancel;
			NotNullPtr<UI::GUIButton> btnOK;
			NotNullPtr<Text::String> name;

			static void __stdcall OnOKClicked(AnyType userObj);
			static void __stdcall OnCancelClicked(AnyType userObj);
			static OSInt __stdcall MACCompare(Net::MACInfo::MACEntry *obj1, Net::MACInfo::MACEntry *obj2);
		public:
			AVIRMACManagerEntryForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, const UInt8 *mac, Text::CString name);
			virtual ~AVIRMACManagerEntryForm();

			virtual void OnMonitorChanged();

			NotNullPtr<Text::String> GetNameNew() const;
		};
	}
}
#endif
