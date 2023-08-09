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

			UI::GUILabel *lblGPS;
			UI::GUICheckBox *chkGPS;
			UI::GUIButton *btnGPS;
			UI::GUITextBox *txtGPS;

			UI::GUILabel *lblWifi;
			UI::GUICheckBox *chkWifi;

			UI::GUIButton *btnOK;
			UI::GUIButton *btnCancel;

			static void __stdcall OnOKClick(void *userObj);
			static void __stdcall OnCancelClick(void *userObj);
			static void __stdcall OnGPSClick(void *userObj);
		public:
			AVIRSetLocationSvcForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRSetLocationSvcForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
