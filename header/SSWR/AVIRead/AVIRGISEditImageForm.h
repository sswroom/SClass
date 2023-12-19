#ifndef _SM_SSWR_AVIREAD_AVIRGISEDITIMAGEFORM
#define _SM_SSWR_AVIREAD_AVIRGISEDITIMAGEFORM
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
		class AVIRGISEditImageForm : public UI::GUIForm
		{
		private:
			NotNullPtr<UI::GUIPanel> pnlCtrl;
			NotNullPtr<UI::GUICheckBox> chkAutoPan;
			NotNullPtr<UI::GUICheckBox> chkEdit;
			UI::GUIListBox *lbImages;
			NotNullPtr<UI::GUIHSplitter> hspMain;
			NotNullPtr<UI::GUIPanel> pnlMain;
			NotNullPtr<UI::GUILabel> lblLeft;
			NotNullPtr<UI::GUITextBox> txtLeft;
			NotNullPtr<UI::GUILabel> lblTop;
			NotNullPtr<UI::GUITextBox> txtTop;
			NotNullPtr<UI::GUILabel> lblRight;
			NotNullPtr<UI::GUITextBox> txtRight;
			NotNullPtr<UI::GUILabel> lblBottom;
			NotNullPtr<UI::GUITextBox> txtBottom;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Int64 currImage;
			Math::Coord2DDbl imgMin;
			Math::Coord2DDbl imgMax;
			Int32 downType;
			Math::Coord2D<OSInt> downPos;

			NotNullPtr<Map::VectorLayer> lyr;
			IMapNavigator *navi;

			static void __stdcall OnImageChg(void *userObj);
			void UpdateImgStat();
			static Bool __stdcall OnMouseDown(void *userObj, Math::Coord2D<OSInt> scnPos);
			static Bool __stdcall OnMouseUp(void *userObj, Math::Coord2D<OSInt> scnPos);
			static Bool __stdcall OnMouseMove(void *userObj, Math::Coord2D<OSInt> scnPos);
			Int32 CalcDownType(Math::Coord2D<OSInt> scnPos);
		public:
			AVIRGISEditImageForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<Map::VectorLayer> lyr, IMapNavigator *navi);
			virtual ~AVIRGISEditImageForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
