#ifndef _SM_SSWR_AVIREAD_AVIRGPSSIMULATORFORM
#define _SM_SSWR_AVIREAD_AVIRGPSSIMULATORFORM
#include "Data/ArrayList.h"
#include "IO/Stream.h"
#include "Map/TileMapLayer.h"
#include "Math/GeographicCoordinateSystem.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "SSWR/AVIRead/IMapNavigator.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRGPSSimulatorForm : public UI::GUIForm
		{
		private:
			UI::GUILabel *lblStreamType;
			UI::GUITextBox *txtStreamType;
			UI::GUIButton *btnStream;
			UI::GUICheckBox *chkAddPoints;
			UI::GUILabel *lblSpeed;
			UI::GUITextBox *txtSpeed;
			UI::GUITextBox *txtSpeedInput;
			UI::GUIButton *btnSpeed;
			UI::GUILabel *lblCurrLat;
			UI::GUITextBox *txtCurrLat;
			UI::GUILabel *lblCurrLon;
			UI::GUITextBox *txtCurrLon;
			UI::GUILabel *lblPoints;
			UI::GUIListBox *lbPoints;

			SSWR::AVIRead::AVIRCore *core;
			IMapNavigator *navi;
			Math::GeographicCoordinateSystem *wgs84;
			IO::Stream *stm;
			Data::ArrayList<Double> *points;
			Double currX;
			Double currY;
			Double speed;

			static Bool __stdcall OnMouseDown(void *userObj, OSInt x, OSInt y);
			static void __stdcall OnStreamClicked(void *userObj);
			static void __stdcall OnSpeedClicked(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
			void GenRecord(Double lat, Double lon, Double dir, Double speed, Bool isValid);
			Char *GenLat(Char *ptr, Double lat);
			Char *GenLon(Char *ptr, Double lon);
			Char *GenCheck(Char *ptr, Char *start);
		public:
			AVIRGPSSimulatorForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, IMapNavigator *navi);
			virtual ~AVIRGPSSimulatorForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
