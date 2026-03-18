#ifndef _SM_SSWR_AVIREAD_AVIROSMTILEBOUNDSFORM
#define _SM_SSWR_AVIREAD_AVIROSMTILEBOUNDSFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIROSMTileBoundsForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;

			NN<UI::GUILabel> lblLevel;
			NN<UI::GUITextBox> txtLevel;
			NN<UI::GUILabel> lblX;
			NN<UI::GUITextBox> txtX;
			NN<UI::GUILabel> lblY;
			NN<UI::GUITextBox> txtY;
			NN<UI::GUIButton> btnCalc;
			NN<UI::GUILabel> lblMinX;
			NN<UI::GUITextBox> txtMinX;
			NN<UI::GUILabel> lblMinY;
			NN<UI::GUITextBox> txtMinY;
			NN<UI::GUILabel> lblMaxX;
			NN<UI::GUITextBox> txtMaxX;
			NN<UI::GUILabel> lblMaxY;
			NN<UI::GUITextBox> txtMaxY;

			static void __stdcall OnCalcClicked(AnyType userObj);
		public:
			AVIROSMTileBoundsForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIROSMTileBoundsForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
