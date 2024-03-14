#ifndef _SM_SSWR_AVIREAD_AVIRSETLOCATIONSVCFORM
#define _SM_SSWR_AVIREAD_AVIRSETLOCATIONSVCFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRSetLocationSvcForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			NotNullPtr<UI::GUILabel> lblGPS;
			NotNullPtr<UI::GUICheckBox> chkGPS;
			NotNullPtr<UI::GUIButton> btnGPS;
			NotNullPtr<UI::GUITextBox> txtGPS;

			NotNullPtr<UI::GUILabel> lblWifi;
			NotNullPtr<UI::GUICheckBox> chkWifi;

			NotNullPtr<UI::GUIButton> btnOK;
			NotNullPtr<UI::GUIButton> btnCancel;

			static void __stdcall OnOKClick(AnyType userObj);
			static void __stdcall OnCancelClick(AnyType userObj);
			static void __stdcall OnGPSClick(AnyType userObj);
		public:
			AVIRSetLocationSvcForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRSetLocationSvcForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
