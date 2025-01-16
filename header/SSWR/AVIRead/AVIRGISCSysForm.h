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
			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Math::CoordinateSystem> oriCSys;
			Optional<Math::CoordinateSystem> outCSys;

			NN<UI::GUITextBox> txtCurrCSys;
			NN<UI::GUIPanel> pnlSel;
			NN<UI::GUIRadioButton> radGeo;
			NN<UI::GUIComboBox> cboGeo;
			NN<UI::GUIRadioButton> radProj;
			NN<UI::GUIComboBox> cboProj;
			NN<UI::GUIButton> btnOK;
			NN<UI::GUIButton> btnCancel;
			NN<UI::GUIPanel> pnlInfo;

			static void __stdcall OnOKClicked(AnyType userObj);
			static void __stdcall OnCancelClicked(AnyType userObj);
			static void __stdcall OnGeoSelChg(AnyType userObj);
			static void __stdcall OnProjSelChg(AnyType userObj);
		public:
			AVIRGISCSysForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, Optional<Math::CoordinateSystem> csys);
			virtual ~AVIRGISCSysForm();

			virtual void OnMonitorChanged();

			Optional<Math::CoordinateSystem> GetCSys();
		};
	}
}
#endif
