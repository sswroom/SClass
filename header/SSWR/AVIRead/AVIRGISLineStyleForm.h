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
			Media::ColorManagerSess *colorSess;
			Map::MapEnv *env;
			NotNullPtr<Media::DrawEngine> eng;
			UOSInt lineStyle;
			Bool changed;

			UI::GUIGroupBox *grpStyle;
			SSWR::AVIRead::AVIRLineSelector *lineSelector;
			UI::GUIPanel *pnlStyle;
			UI::GUIPanel *pnlButtons;

			UI::GUIButton *btnAddStyle;
			UI::GUIButton *btnRemoveStyle;
			UI::GUIButton *btnEditStyle;
            
			UI::GUIButton *btnOK;
			UI::GUIButton *btnCancel;

			UI::GUIPopupMenu *mnuLayer;

			static void __stdcall LineSelChanged(void *userObj);
			static void __stdcall AddStyleClicked(void *userObj);
			static void __stdcall RemoveStyleClicked(void *userObj);
			static void __stdcall EditStyleClicked(void *userObj);
			static void __stdcall LineDblClicked(void *userObj);
			static void __stdcall OKClicked(void *userObj);
			static void __stdcall CancelClicked(void *userObj);
		public:
			AVIRGISLineStyleForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, Map::MapEnv *env, NotNullPtr<Media::DrawEngine> eng, UOSInt lineStyle);
			virtual ~AVIRGISLineStyleForm();

			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();

			UOSInt GetLineStyle();
			Bool IsChanged();
		};
	};
};
#endif
