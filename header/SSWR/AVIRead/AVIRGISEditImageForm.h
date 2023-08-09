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
			UI::GUIPanel *pnlCtrl;
			UI::GUICheckBox *chkAutoPan;
			UI::GUICheckBox *chkEdit;
			UI::GUIListBox *lbImages;
			UI::GUIHSplitter *hspMain;
			UI::GUIPanel *pnlMain;
			UI::GUILabel *lblLeft;
			UI::GUITextBox *txtLeft;
			UI::GUILabel *lblTop;
			UI::GUITextBox *txtTop;
			UI::GUILabel *lblRight;
			UI::GUITextBox *txtRight;
			UI::GUILabel *lblBottom;
			UI::GUITextBox *txtBottom;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Int64 currImage;
			Math::Coord2DDbl imgMin;
			Math::Coord2DDbl imgMax;
			Int32 downType;
			Math::Coord2D<OSInt> downPos;

			Map::VectorLayer *lyr;
			IMapNavigator *navi;

			static void __stdcall OnImageChg(void *userObj);
			void UpdateImgStat();
			static Bool __stdcall OnMouseDown(void *userObj, Math::Coord2D<OSInt> scnPos);
			static Bool __stdcall OnMouseUp(void *userObj, Math::Coord2D<OSInt> scnPos);
			static Bool __stdcall OnMouseMove(void *userObj, Math::Coord2D<OSInt> scnPos);
			Int32 CalcDownType(Math::Coord2D<OSInt> scnPos);
		public:
			AVIRGISEditImageForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, Map::VectorLayer *lyr, IMapNavigator *navi);
			virtual ~AVIRGISEditImageForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
