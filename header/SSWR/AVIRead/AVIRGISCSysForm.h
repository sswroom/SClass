#ifndef _SM_SSWR_AVIREAD_AVIRGISCSYSFORM
#define _SM_SSWR_AVIREAD_AVIRGISCSYSFORM
#include "Math/CoordinateSystem.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"
#include "UI/GUIRadioButton.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRGISCSysForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Math::CoordinateSystem *oriCSys;
			Math::CoordinateSystem *outCSys;

			UI::GUITextBox *txtCurrCSys;
			NotNullPtr<UI::GUIPanel> pnlSel;
			UI::GUIRadioButton *radGeo;
			UI::GUIComboBox *cboGeo;
			UI::GUIRadioButton *radProj;
			UI::GUIComboBox *cboProj;
			UI::GUIButton *btnOK;
			UI::GUIButton *btnCancel;
			NotNullPtr<UI::GUIPanel> pnlInfo;

			static void __stdcall OnOKClicked(void *userObj);
			static void __stdcall OnCancelClicked(void *userObj);
			static void __stdcall OnGeoSelChg(void *userObj);
			static void __stdcall OnProjSelChg(void *userObj);
		public:
			AVIRGISCSysForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, Math::CoordinateSystem *csys);
			virtual ~AVIRGISCSysForm();

			virtual void OnMonitorChanged();

			Math::CoordinateSystem *GetCSys();
		};
	}
}
#endif
