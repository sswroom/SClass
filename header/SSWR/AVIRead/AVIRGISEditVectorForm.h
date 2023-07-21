#ifndef _SM_SSWR_AVIREAD_AVIRGISEDITVECTORFORM
#define _SM_SSWR_AVIREAD_AVIRGISEDITVECTORFORM
#include "Map/VectorLayer.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "SSWR/AVIRead/IMapNavigator.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRGISEditVectorForm : public UI::GUIForm
		{
		private:
			UI::GUIListBox *lbObjects;

			SSWR::AVIRead::AVIRCore *core;
			Map::VectorLayer *lyr;
			IMapNavigator *navi;

			static Bool __stdcall OnMouseDown(void *userObj, Math::Coord2D<OSInt> scnPos);
			static Bool __stdcall OnMouseUp(void *userObj, Math::Coord2D<OSInt> scnPos);
			static Bool __stdcall OnMouseMove(void *userObj, Math::Coord2D<OSInt> scnPos);
			static void __stdcall OnObjectsDblClk(void *userObj);
		public:
			AVIRGISEditVectorForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, SSWR::AVIRead::AVIRCore *core, Map::VectorLayer *lyr, IMapNavigator *navi);
			virtual ~AVIRGISEditVectorForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
