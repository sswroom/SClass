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
			NN<SSWR::AVIRead::AVIRCore> core;

			NN<UI::GUILabel> lblMAC;
			NN<UI::GUITextBox> txtMAC;
			NN<UI::GUILabel> lblName;
			NN<UI::GUIComboBox> cboName;
			NN<UI::GUIButton> btnCancel;
			NN<UI::GUIButton> btnOK;
			NN<Text::String> name;

			static void __stdcall OnOKClicked(AnyType userObj);
			static void __stdcall OnCancelClicked(AnyType userObj);
			static OSInt __stdcall MACCompare(Net::MACInfo::MACEntry *obj1, Net::MACInfo::MACEntry *obj2);
		public:
			AVIRMACManagerEntryForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, const UInt8 *mac, Text::CString name);
			virtual ~AVIRMACManagerEntryForm();

			virtual void OnMonitorChanged();

			NN<Text::String> GetNameNew() const;
		};
	}
}
#endif
