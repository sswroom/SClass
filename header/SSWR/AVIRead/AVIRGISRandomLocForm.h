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
			NotNullPtr<UI::GUIButton> btnArea;
			NotNullPtr<UI::GUIButton> btnRandom;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Math::Coord2DDbl selPt1;
			Math::Coord2DDbl selPt2;
			Bool selecting;
			Bool isDown;
			Math::Coord2D<OSInt> downPt;

			IMapNavigator *navi;

			static Bool __stdcall OnMouseDown(AnyType userObj, Math::Coord2D<OSInt> scnPos);
			static Bool __stdcall OnMouseUp(AnyType userObj, Math::Coord2D<OSInt> scnPos);
			static Bool __stdcall OnMouseMove(AnyType userObj, Math::Coord2D<OSInt> scnPos);

			static void __stdcall OnAreaClicked(AnyType userObj);
			static void __stdcall OnRandomClicked(AnyType userObj);

		public:
			AVIRGISRandomLocForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, IMapNavigator *navi);
			virtual ~AVIRGISRandomLocForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
