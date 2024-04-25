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
			NN<SSWR::AVIRead::AVIRCore> core;

			NN<UI::GUILabel> lblGPS;
			NN<UI::GUICheckBox> chkGPS;
			NN<UI::GUIButton> btnGPS;
			NN<UI::GUITextBox> txtGPS;

			NN<UI::GUILabel> lblWifi;
			NN<UI::GUICheckBox> chkWifi;

			NN<UI::GUIButton> btnOK;
			NN<UI::GUIButton> btnCancel;

			static void __stdcall OnOKClick(AnyType userObj);
			static void __stdcall OnCancelClick(AnyType userObj);
			static void __stdcall OnGPSClick(AnyType userObj);
		public:
			AVIRSetLocationSvcForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRSetLocationSvcForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
