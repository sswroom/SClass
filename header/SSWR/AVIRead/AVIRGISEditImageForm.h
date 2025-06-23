#ifndef _SM_SSWR_AVIREAD_AVIRGISEDITIMAGEFORM
#define _SM_SSWR_AVIREAD_AVIRGISEDITIMAGEFORM
#include "Map/VectorLayer.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "SSWR/AVIRead/AVIRMapNavigator.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"
#include "UI/GUITrackBar.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRGISEditImageForm : public UI::GUIForm
		{
		private:
			NN<UI::GUIPanel> pnlCtrl;
			NN<UI::GUICheckBox> chkAutoPan;
			NN<UI::GUICheckBox> chkEdit;
			NN<UI::GUIListBox> lbImages;
			NN<UI::GUIHSplitter> hspMain;
			NN<UI::GUIPanel> pnlMain;
			NN<UI::GUILabel> lblLeft;
			NN<UI::GUITextBox> txtLeft;
			NN<UI::GUILabel> lblTop;
			NN<UI::GUITextBox> txtTop;
			NN<UI::GUILabel> lblRight;
			NN<UI::GUITextBox> txtRight;
			NN<UI::GUILabel> lblBottom;
			NN<UI::GUITextBox> txtBottom;
			NN<UI::GUILabel> lblAlpha;
			NN<UI::GUITextBox> txtAlpha;
			NN<UI::GUITrackBar> trkAlpha;

			NN<SSWR::AVIRead::AVIRCore> core;
			Int64 currImage;
			Math::Coord2DDbl imgMin;
			Math::Coord2DDbl imgMax;
			Int32 downType;
			Math::Coord2D<OSInt> downPos;

			NN<Map::VectorLayer> lyr;
			NN<AVIRMapNavigator> navi;

			static void __stdcall OnImageChg(AnyType userObj);
			void UpdateImgStat();
			static Bool __stdcall OnMouseDown(AnyType userObj, Math::Coord2D<OSInt> scnPos);
			static Bool __stdcall OnMouseUp(AnyType userObj, Math::Coord2D<OSInt> scnPos);
			static Bool __stdcall OnMouseMove(AnyType userObj, Math::Coord2D<OSInt> scnPos);
			static void __stdcall OnAlphaScrolled(AnyType userObj, UOSInt scrollPos);
			Int32 CalcDownType(Math::Coord2D<OSInt> scnPos);
		public:
			AVIRGISEditImageForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Map::VectorLayer> lyr, NN<AVIRMapNavigator> navi);
			virtual ~AVIRGISEditImageForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
