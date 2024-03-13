#ifndef _SM_SSWR_AVIREAD_AVIRGISDISTANCEFORM
#define _SM_SSWR_AVIREAD_AVIRGISDISTANCEFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "SSWR/AVIRead/IMapNavigator.h"
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
			NotNullPtr<UI::GUILabel> lblType;
			NotNullPtr<UI::GUIPanel> pnlType;
			NotNullPtr<UI::GUIRadioButton> radTypeLine;
			NotNullPtr<UI::GUIRadioButton> radTypePath;
			NotNullPtr<UI::GUILabel> lblAction;
			NotNullPtr<UI::GUIPanel> pnlAction;
			NotNullPtr<UI::GUIRadioButton> radActionPause;
			NotNullPtr<UI::GUIRadioButton> radActionMeasure;
			NotNullPtr<UI::GUILabel> lblDistance;
			NotNullPtr<UI::GUITextBox> txtDistance;
			NotNullPtr<UI::GUIComboBox> cboDistanceUnit;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			IMapNavigator *navi;
			NotNullPtr<Math::CoordinateSystem> csys;

			Data::ArrayList<Double> *ptList;
			Math::Coord2DDbl lastMapPos;
			Double pathDist;
			Double dispDist;

		private:
			static void __stdcall OnTypeSelChg(void *userObj, Bool newState);
			static void __stdcall OnMeasureSelChg(void *userObj, Bool newState);
			static void __stdcall OnDistanceUnitChg(void *userObj);
			static Bool __stdcall OnMapMouseDown(void *userObj, Math::Coord2D<OSInt> scnPos);
			static Bool __stdcall OnMapMouseMove(void *userObj, Math::Coord2D<OSInt> scnPos);
			void UpdateDistDisp();

		public:
			AVIRGISDistanceForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, IMapNavigator *navi);
			virtual ~AVIRGISDistanceForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
