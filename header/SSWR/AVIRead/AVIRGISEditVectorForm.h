#ifndef _SM_SSWR_AVIREAD_AVIRGISEDITVECTORFORM
#define _SM_SSWR_AVIREAD_AVIRGISEDITVECTORFORM
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
		class AVIRGISEditVectorForm : public UI::GUIForm
		{
		private:
			NN<UI::GUIListBox> lbObjects;
			NN<UI::GUIHSplitter> hspObjects;
			NN<UI::GUIPanel> pnlObjects;
			NN<UI::GUILabel> lblStatus;
			NN<UI::GUITextBox> txtStatus;
			NN<UI::GUILabel> lblNPoints;
			NN<UI::GUITextBox> txtNPoints;
			NN<UI::GUIButton> btnNew;
			NN<UI::GUIButton> btnDelete;
			NN<UI::GUIButton> btnEnd;
			NN<UI::GUIButton> btnBack;

			Math::Coord2D<OSInt> downPos;
			Data::ArrayListA<Math::Coord2DDbl> points;
			UOSInt status;
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Map::VectorLayer> lyr;
			NN<AVIRMapNavigator> navi;

			static Bool __stdcall OnMouseDown(AnyType userObj, Math::Coord2D<OSInt> scnPos);
			static Bool __stdcall OnMouseUp(AnyType userObj, Math::Coord2D<OSInt> scnPos);
			static Bool __stdcall OnMouseMove(AnyType userObj, Math::Coord2D<OSInt> scnPos);
			static void __stdcall OnObjectsDblClk(AnyType userObj);
			static void __stdcall OnObjectsSelChg(AnyType userObj);
			static void __stdcall OnNewClicked(AnyType userObj);
			static void __stdcall OnDeleteClicked(AnyType userObj);
			static void __stdcall OnEndClicked(AnyType userObj);
			static void __stdcall OnBackClicked(AnyType userObj);
			void UpdateList();
		public:
			AVIRGISEditVectorForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Map::VectorLayer> lyr, NN<AVIRMapNavigator> navi);
			virtual ~AVIRGISEditVectorForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
