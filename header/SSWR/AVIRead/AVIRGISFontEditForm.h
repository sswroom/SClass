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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Media::ColorManagerSess *colorSess;
			Media::ColorConv *colorConv;
			Map::MapEnv *env;
			UOSInt fontStyle;
			Bool changed;
			NotNullPtr<Media::DrawEngine> eng;

			Text::String *currFontName;
			Double currFontSizePt;
			UInt32 currColor;
			UOSInt currBuffSize;
			UInt32 currBuffColor;
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
			static Bool __stdcall FontColorClicked(void *userObj, Math::Coord2D<OSInt> scnPos, MouseButton mouseBtn);
			static void __stdcall BufferSizeChanged(void *userObj, UOSInt scrollPos);
			static Bool __stdcall BufferColorClicked(void *userObj, Math::Coord2D<OSInt> scnPos, MouseButton mouseBtn);
			static void __stdcall FontsDoubleClicked(void *userObj);
			static void __stdcall OKClicked(void *userObj);
			static void __stdcall CancelClicked(void *userObj);

			void UpdateFontPreview();
			void UpdateDisplay();
		public:
			AVIRGISFontEditForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, Map::MapEnv *env, NotNullPtr<Media::DrawEngine> eng, UOSInt fontStyle);
			virtual ~AVIRGISFontEditForm();

			virtual void OnMonitorChanged();

			virtual void YUVParamChanged(const Media::IColorHandler::YUVPARAM *yuvParam);
			virtual void RGBParamChanged(const Media::IColorHandler::RGBPARAM2 *rgbParam);
		};
	}
}
#endif
