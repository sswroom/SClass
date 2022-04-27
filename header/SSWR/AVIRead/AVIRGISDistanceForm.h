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
			UI::GUILabel *lblType;
			UI::GUIPanel *pnlType;
			UI::GUIRadioButton *radTypeLine;
			UI::GUIRadioButton *radTypePath;
			UI::GUILabel *lblAction;
			UI::GUIPanel *pnlAction;
			UI::GUIRadioButton *radActionPause;
			UI::GUIRadioButton *radActionMeasure;
			UI::GUILabel *lblDistance;
			UI::GUITextBox *txtDistance;
			UI::GUIComboBox *cboDistanceUnit;

			SSWR::AVIRead::AVIRCore *core;
			IMapNavigator *navi;
			Math::CoordinateSystem *csys;

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
			AVIRGISDistanceForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, IMapNavigator *navi);
			virtual ~AVIRGISDistanceForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
