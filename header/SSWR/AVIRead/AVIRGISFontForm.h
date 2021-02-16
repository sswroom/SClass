#ifndef _SM_SSWR_AVIREAD_AVIRGISFONTFORM
#define _SM_SSWR_AVIREAD_AVIRGISFONTFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "SSWR/AVIRead/AVIRFontSelector.h"
#include "SSWR/AVIRead/AVIRGISReplayForm.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUIPictureBox.h"
#include "UI/GUIHScrollBar.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRGISFontForm : public UI::GUIForm, public Media::IColorHandler
		{
		private:
			SSWR::AVIRead::AVIRCore *core;
			Media::ColorManagerSess *colorSess;
			Media::ColorConv *colorConv;
			Map::MapEnv *env;
			Bool changed;
			Media::DrawEngine *eng;

			const UTF8Char *fontName;
			Double fontSizePt;
			Int32 fontColor;
			Media::StaticImage *previewImage;

			UI::GUIPictureBox *pbPreview;
			UI::GUIPanel *pnlMain;
			UI::GUILabel *lblFont;
			UI::GUITextBox *txtFont;
			UI::GUIButton *btnFont;
			UI::GUILabel *lblColor;
			UI::GUIPictureBox *pbColor;
			UI::GUIButton *btnOK;
			UI::GUIButton *btnCancel;

			static void __stdcall OnFontClicked(void *userObj);
			static Bool __stdcall OnColorClicked(void *userObj, OSInt scnX, OSInt scnY, MouseButton btn);
			static void __stdcall OnOKClicked(void *userObj);
			static void __stdcall OnCancelClicked(void *userObj);

			void UpdateFontText();
			void UpdateFontPreview();
		public:
			AVIRGISFontForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Media::DrawEngine *eng, const UTF8Char *fontName, Double fontSizePt, Int32 fontColor);
			virtual ~AVIRGISFontForm();

			virtual void OnMonitorChanged();

			virtual void YUVParamChanged(const Media::IColorHandler::YUVPARAM *yuvParam);
			virtual void RGBParamChanged(const Media::IColorHandler::RGBPARAM2 *rgbParam);

			const UTF8Char *GetFontName();
			Double GetFontSizePt();
			Int32 GetFontColor();

			Bool IsChanged();
		};
	}
}
#endif
