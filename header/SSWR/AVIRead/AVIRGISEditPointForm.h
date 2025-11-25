#ifndef _SM_SSWR_AVIREAD_AVIRGISEDITPOINTFORM
#define _SM_SSWR_AVIREAD_AVIRGISEDITPOINTFORM
#include "Data/ArrayListA.hpp"
#include "Map/VectorLayer.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "SSWR/AVIRead/AVIRMapNavigator.h"
#include "UI/GUIButton.h"
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
		class AVIRGISEditPointForm : public UI::GUIForm
		{
		private:
			NN<UI::GUIListBox> lbObjects;
			NN<UI::GUIHSplitter> hspObjects;
			NN<UI::GUIPanel> pnlObjects;
			NN<UI::GUILabel> lblStatus;
			NN<UI::GUITextBox> txtStatus;
			NN<UI::GUILabel> lblName;
			NN<UI::GUITextBox> txtName;
			NN<UI::GUIButton> btnNew;
			NN<UI::GUIButton> btnDelete;

			UOSInt status;
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Map::VectorLayer> lyr;
			NN<AVIRMapNavigator> navi;
			Math::Coord2D<OSInt> downPos;

			static Bool __stdcall OnMouseDown(AnyType userObj, Math::Coord2D<OSInt> scnPos);
			static Bool __stdcall OnMouseUp(AnyType userObj, Math::Coord2D<OSInt> scnPos);
			static void __stdcall OnObjectsDblClk(AnyType userObj);
			static void __stdcall OnObjectsSelChg(AnyType userObj);
			static void __stdcall OnNewClicked(AnyType userObj);
			static void __stdcall OnDeleteClicked(AnyType userObj);
			void UpdateList();
		public:
			AVIRGISEditPointForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Map::VectorLayer> lyr, NN<AVIRMapNavigator> navi);
			virtual ~AVIRGISEditPointForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
