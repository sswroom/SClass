#ifndef _SM_SSWR_AVIREAD_AVIRGISRANDOMLOCFORM
#define _SM_SSWR_AVIREAD_AVIRGISRANDOMLOCFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "SSWR/AVIRead/IMapNavigator.h"
#include "Sync/Mutex.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRGISRandomLocForm : public UI::GUIForm
		{
		private:
			UI::GUIButton *btnArea;
			UI::GUIButton *btnRandom;

			SSWR::AVIRead::AVIRCore *core;
			Math::Coord2D<Double> selPt1;
			Math::Coord2D<Double> selPt2;
			Bool selecting;
			Bool isDown;
			Math::Coord2D<OSInt> downPt;

			IMapNavigator *navi;

			static Bool __stdcall OnMouseDown(void *userObj, Math::Coord2D<OSInt> scnPos);
			static Bool __stdcall OnMouseUp(void *userObj, Math::Coord2D<OSInt> scnPos);
			static Bool __stdcall OnMouseMove(void *userObj, Math::Coord2D<OSInt> scnPos);

			static void __stdcall OnAreaClicked(void *userObj);
			static void __stdcall OnRandomClicked(void *userObj);

		public:
			AVIRGISRandomLocForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, IMapNavigator *navi);
			virtual ~AVIRGISRandomLocForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
