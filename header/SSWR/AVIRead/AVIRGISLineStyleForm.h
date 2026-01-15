#ifndef _SM_SSWR_AVIREAD_AVIRGISLINESTYLEFORM
#define _SM_SSWR_AVIREAD_AVIRGISLINESTYLEFORM
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUIGroupBox.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIPanel.h"
#include "UI/GUIPictureBox.h"
#include "UI/GUIHScrollBar.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "SSWR/AVIRead/AVIRLineSelector.h"
#include "SSWR/AVIRead/AVIRGISLineForm.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRGISLineStyleForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Media::ColorManagerSess> colorSess;
			NN<Map::MapEnv> env;
			NN<Media::DrawEngine> eng;
			UIntOS lineStyle;
			Bool changed;

			NN<UI::GUIGroupBox> grpStyle;
			NN<SSWR::AVIRead::AVIRLineSelector> lineSelector;
			NN<UI::GUIPanel> pnlStyle;
			NN<UI::GUIPanel> pnlButtons;

			NN<UI::GUIButton> btnAddStyle;
			NN<UI::GUIButton> btnRemoveStyle;
			NN<UI::GUIButton> btnEditStyle;
            
			NN<UI::GUIButton> btnOK;
			NN<UI::GUIButton> btnCancel;

			NN<UI::GUIPopupMenu> mnuLayer;

			static void __stdcall LineSelChanged(AnyType userObj);
			static void __stdcall AddStyleClicked(AnyType userObj);
			static void __stdcall RemoveStyleClicked(AnyType userObj);
			static void __stdcall EditStyleClicked(AnyType userObj);
			static void __stdcall LineDblClicked(AnyType userObj);
			static void __stdcall OKClicked(AnyType userObj);
			static void __stdcall CancelClicked(AnyType userObj);
		public:
			AVIRGISLineStyleForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Map::MapEnv> env, NN<Media::DrawEngine> eng, UIntOS lineStyle);
			virtual ~AVIRGISLineStyleForm();

			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();

			UIntOS GetLineStyle();
			Bool IsChanged();
		};
	}
}
#endif
