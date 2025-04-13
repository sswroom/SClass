#ifndef _SM_SSWR_AVIREAD_AVIRGISDISTANCEFORM
#define _SM_SSWR_AVIREAD_AVIRGISDISTANCEFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "SSWR/AVIRead/AVIRMapNavigator.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIPanel.h"
#include "UI/GUIRadioButton.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRGISDistanceForm : public UI::GUIForm
		{
		private:
			NN<UI::GUILabel> lblType;
			NN<UI::GUIPanel> pnlType;
			NN<UI::GUIRadioButton> radTypeLine;
			NN<UI::GUIRadioButton> radTypePath;
			NN<UI::GUILabel> lblAction;
			NN<UI::GUIPanel> pnlAction;
			NN<UI::GUIRadioButton> radActionPause;
			NN<UI::GUIRadioButton> radActionMeasure;
			NN<UI::GUILabel> lblDistance;
			NN<UI::GUITextBox> txtDistance;
			NN<UI::GUIComboBox> cboDistanceUnit;

			NN<SSWR::AVIRead::AVIRCore> core;
			NN<AVIRMapNavigator> navi;
			NN<Math::CoordinateSystem> csys;

			Data::ArrayList<Double> ptList;
			Math::Coord2DDbl lastMapPos;
			Double pathDist;
			Double dispDist;

		private:
			static void __stdcall OnTypeSelChg(AnyType userObj, Bool newState);
			static void __stdcall OnMeasureSelChg(AnyType userObj, Bool newState);
			static void __stdcall OnDistanceUnitChg(AnyType userObj);
			static Bool __stdcall OnMapMouseDown(AnyType userObj, Math::Coord2D<OSInt> scnPos);
			static Bool __stdcall OnMapMouseMove(AnyType userObj, Math::Coord2D<OSInt> scnPos);
			void UpdateDistDisp();

		public:
			AVIRGISDistanceForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<AVIRMapNavigator> navi);
			virtual ~AVIRGISDistanceForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
