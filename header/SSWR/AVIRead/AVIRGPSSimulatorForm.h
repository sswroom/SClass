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
			NN<UI::GUILabel> lblStreamType;
			NN<UI::GUITextBox> txtStreamType;
			NN<UI::GUIButton> btnStream;
			NN<UI::GUICheckBox> chkAddPoints;
			NN<UI::GUILabel> lblSpeed;
			NN<UI::GUITextBox> txtSpeed;
			NN<UI::GUITextBox> txtSpeedInput;
			NN<UI::GUIButton> btnSpeed;
			NN<UI::GUILabel> lblCurrLat;
			NN<UI::GUITextBox> txtCurrLat;
			NN<UI::GUILabel> lblCurrLon;
			NN<UI::GUITextBox> txtCurrLon;
			NN<UI::GUILabel> lblPoints;
			NN<UI::GUIListBox> lbPoints;

			NN<SSWR::AVIRead::AVIRCore> core;
			IMapNavigator *navi;
			NN<Math::GeographicCoordinateSystem> wgs84;
			IO::Stream *stm;
			Data::ArrayListA<Math::Coord2DDbl> points;
			Math::Coord2DDbl currPos;
			Double speed;

			static Bool __stdcall OnMouseDown(AnyType userObj, Math::Coord2D<OSInt> scnPos);
			static void __stdcall OnStreamClicked(AnyType userObj);
			static void __stdcall OnSpeedClicked(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
			void GenRecord(Math::Coord2DDbl pt, Double dir, Double speed, Bool isValid);
			Char *GenLat(Char *ptr, Double lat);
			Char *GenLon(Char *ptr, Double lon);
			Char *GenCheck(Char *ptr, Char *start);
		public:
			AVIRGPSSimulatorForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, IMapNavigator *navi);
			virtual ~AVIRGPSSimulatorForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
