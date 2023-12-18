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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			NotNullPtr<Media::ColorManagerSess> colorSess;
			NotNullPtr<Map::MapEnv> env;
			NotNullPtr<Media::DrawEngine> eng;
			UOSInt lineStyle;
			Bool changed;

			NotNullPtr<UI::GUIGroupBox> grpStyle;
			SSWR::AVIRead::AVIRLineSelector *lineSelector;
			NotNullPtr<UI::GUIPanel> pnlStyle;
			NotNullPtr<UI::GUIPanel> pnlButtons;

			NotNullPtr<UI::GUIButton> btnAddStyle;
			NotNullPtr<UI::GUIButton> btnRemoveStyle;
			NotNullPtr<UI::GUIButton> btnEditStyle;
            
			NotNullPtr<UI::GUIButton> btnOK;
			NotNullPtr<UI::GUIButton> btnCancel;

			UI::GUIPopupMenu *mnuLayer;

			static void __stdcall LineSelChanged(void *userObj);
			static void __stdcall AddStyleClicked(void *userObj);
			static void __stdcall RemoveStyleClicked(void *userObj);
			static void __stdcall EditStyleClicked(void *userObj);
			static void __stdcall LineDblClicked(void *userObj);
			static void __stdcall OKClicked(void *userObj);
			static void __stdcall CancelClicked(void *userObj);
		public:
			AVIRGISLineStyleForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<Map::MapEnv> env, NotNullPtr<Media::DrawEngine> eng, UOSInt lineStyle);
			virtual ~AVIRGISLineStyleForm();

			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();

			UOSInt GetLineStyle();
			Bool IsChanged();
		};
	}
}
#endif
