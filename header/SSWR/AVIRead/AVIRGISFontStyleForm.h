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
			SSWR::AVIRead::AVIRCore *core;
			Media::ColorManagerSess *colorSess;
			Map::MapEnv *env;
			UOSInt fontStyle;
			Bool changed;
			Media::DrawEngine *eng;

			UI::GUIPanel *pnlBtn;
			UI::GUIButton *btnOK;
			UI::GUIButton *btnCancel;

			UI::GUIGroupBox *grpStyle;
			UI::GUIPanel *pnlStyle;
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
			AVIRGISFontStyleForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Map::MapEnv *env, Media::DrawEngine *eng, UOSInt fontStyle);
			virtual ~AVIRGISFontStyleForm();

			virtual void OnMonitorChanged();

			virtual void YUVParamChanged(const Media::IColorHandler::YUVPARAM *yuvParam);
			virtual void RGBParamChanged(const Media::IColorHandler::RGBPARAM2 *rgbParam);

			UOSInt GetFontStyle();
			Bool IsChanged();
		};
	}
}
#endif
