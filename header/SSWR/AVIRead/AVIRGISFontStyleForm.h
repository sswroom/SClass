#ifndef _SM_SSWR_AVIREAD_AVIRGISFONTSTYLEFORM
#define _SM_SSWR_AVIREAD_AVIRGISFONTSTYLEFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "SSWR/AVIRead/AVIRFontSelector.h"
#include "SSWR/AVIRead/AVIRGISReplayForm.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUIGroupBox.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUIPictureBox.h"
#include "UI/GUIHScrollBar.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRGISFontStyleForm : public UI::GUIForm, public Media::IColorHandler
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			NotNullPtr<Media::ColorManagerSess> colorSess;
			NotNullPtr<Map::MapEnv> env;
			UOSInt fontStyle;
			Bool changed;
			NotNullPtr<Media::DrawEngine> eng;

			NotNullPtr<UI::GUIPanel> pnlBtn;
			UI::GUIButton *btnOK;
			UI::GUIButton *btnCancel;

			NotNullPtr<UI::GUIGroupBox> grpStyle;
			NotNullPtr<UI::GUIPanel> pnlStyle;
			UI::GUIButton *btnAddStyle;
			UI::GUIButton *btnRemoveStyle;
			UI::GUIButton *btnEditStyle;
			SSWR::AVIRead::AVIRFontSelector *fsFonts;

			static void __stdcall AddStyleClicked(void *userObj);
			static void __stdcall RemoveStyleClicked(void *userObj);
			static void __stdcall EditStyleClicked(void *userObj);
			static void __stdcall FontsSelectedChg(void *userObj);
			static void __stdcall FontsDoubleClicked(void *userObj);
			static void __stdcall OKClicked(void *userObj);
			static void __stdcall CancelClicked(void *userObj);

			void UpdatePreview();
		public:
			AVIRGISFontStyleForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<Map::MapEnv> env, NotNullPtr<Media::DrawEngine> eng, UOSInt fontStyle);
			virtual ~AVIRGISFontStyleForm();

			virtual void OnMonitorChanged();

			virtual void YUVParamChanged(NotNullPtr<const Media::IColorHandler::YUVPARAM> yuvParam);
			virtual void RGBParamChanged(NotNullPtr<const Media::IColorHandler::RGBPARAM2> rgbParam);

			UOSInt GetFontStyle();
			Bool IsChanged();
		};
	}
}
#endif
