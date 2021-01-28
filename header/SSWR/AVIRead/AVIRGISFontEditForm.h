#ifndef _SM_SSWR_AVIREAD_AVIRGISFONTEDITFORM
#define _SM_SSWR_AVIREAD_AVIRGISFONTEDITFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUIGroupBox.h"
#include "UI/GUIHScrollBar.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUIPictureBox.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRGISFontEditForm : public UI::GUIForm, public Media::IColorHandler
		{
		private:
			SSWR::AVIRead::AVIRCore *core;
			Media::ColorManagerSess *colorSess;
			Media::ColorConv *colorConv;
			Map::MapEnv *env;
			UOSInt fontStyle;
			Bool changed;
			Media::DrawEngine *eng;

			const UTF8Char *currFontName;
			Double currFontSize;
			Int32 currColor;
			UOSInt currBuffSize;
			Int32 currBuffColor;
			Bool isBold;
			Media::StaticImage *previewImage;

			UI::GUIPictureBox *pbFontPreview;
			UI::GUILabel *lblStyleName;
			UI::GUITextBox *txtStyleName;
			UI::GUILabel *lblFontName;
			UI::GUITextBox *txtFontName;
			UI::GUIButton *btnFontName;
			UI::GUILabel *lblFontColor;
			UI::GUIPictureBox *pbFontColor;
			UI::GUILabel *lblBufferSize;
			UI::GUIHScrollBar *hsbBufferSize;
			UI::GUILabel *lblBufferColor;
			UI::GUIPictureBox *pbBufferColor;
			UI::GUIButton *btnOK;
			UI::GUIButton *btnCancel;

			static void __stdcall FontNameClicked(void *userObj);
			static Bool __stdcall FontColorClicked(void *userObj, OSInt x, OSInt y, MouseButton mouseBtn);
			static void __stdcall BufferSizeChanged(void *userObj, OSInt scrollPos);
			static Bool __stdcall BufferColorClicked(void *userObj, OSInt x, OSInt y, MouseButton mouseBtn);
			static void __stdcall FontsDoubleClicked(void *userObj);
			static void __stdcall OKClicked(void *userObj);
			static void __stdcall CancelClicked(void *userObj);

			void UpdateFontPreview();
			void UpdateDisplay();
		public:
			AVIRGISFontEditForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Map::MapEnv *env, Media::DrawEngine *eng, UOSInt fontStyle);
			virtual ~AVIRGISFontEditForm();

			virtual void OnMonitorChanged();

			virtual void YUVParamChanged(const Media::IColorHandler::YUVPARAM *yuvParam);
			virtual void RGBParamChanged(const Media::IColorHandler::RGBPARAM2 *rgbParam);
		};
	}
}
#endif
