#ifndef _SM_SSWR_AVIREAD_AVIRGPSSIMULATORFORM
#define _SM_SSWR_AVIREAD_AVIRGPSSIMULATORFORM
#include "Data/ArrayListA.h"
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
			NotNullPtr<UI::GUILabel> lblStreamType;
			NotNullPtr<UI::GUITextBox> txtStreamType;
			NotNullPtr<UI::GUIButton> btnStream;
			NotNullPtr<UI::GUICheckBox> chkAddPoints;
			NotNullPtr<UI::GUILabel> lblSpeed;
			NotNullPtr<UI::GUITextBox> txtSpeed;
			NotNullPtr<UI::GUITextBox> txtSpeedInput;
			NotNullPtr<UI::GUIButton> btnSpeed;
			NotNullPtr<UI::GUILabel> lblCurrLat;
			NotNullPtr<UI::GUITextBox> txtCurrLat;
			NotNullPtr<UI::GUILabel> lblCurrLon;
			NotNullPtr<UI::GUITextBox> txtCurrLon;
			NotNullPtr<UI::GUILabel> lblPoints;
			NotNullPtr<UI::GUIListBox> lbPoints;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			IMapNavigator *navi;
			NotNullPtr<Math::GeographicCoordinateSystem> wgs84;
			IO::Stream *stm;
			Data::ArrayListA<Math::Coord2DDbl> points;
			Math::Coord2DDbl currPos;
			Double speed;

			static Bool __stdcall OnMouseDown(void *userObj, Math::Coord2D<OSInt> scnPos);
			static void __stdcall OnStreamClicked(void *userObj);
			static void __stdcall OnSpeedClicked(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
			void GenRecord(Math::Coord2DDbl pt, Double dir, Double speed, Bool isValid);
			Char *GenLat(Char *ptr, Double lat);
			Char *GenLon(Char *ptr, Double lon);
			Char *GenCheck(Char *ptr, Char *start);
		public:
			AVIRGPSSimulatorForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, IMapNavigator *navi);
			virtual ~AVIRGPSSimulatorForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
