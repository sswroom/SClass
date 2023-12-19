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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			NotNullPtr<Media::ColorManagerSess> colorSess;
			Media::ColorConv *colorConv;
			Bool changed;
			NotNullPtr<Media::DrawEngine> eng;

			NotNullPtr<Text::String> fontName;
			Double fontSizePt;
			UInt32 fontColor;
			Media::StaticImage *previewImage;

			UI::GUIPictureBox *pbPreview;
			NotNullPtr<UI::GUIPanel> pnlMain;
			NotNullPtr<UI::GUILabel> lblFont;
			NotNullPtr<UI::GUITextBox> txtFont;
			NotNullPtr<UI::GUIButton> btnFont;
			NotNullPtr<UI::GUILabel> lblColor;
			UI::GUIPictureBox *pbColor;
			NotNullPtr<UI::GUIButton> btnOK;
			NotNullPtr<UI::GUIButton> btnCancel;

			static void __stdcall OnFontClicked(void *userObj);
			static Bool __stdcall OnColorClicked(void *userObj, Math::Coord2D<OSInt> scnPos, MouseButton btn);
			static void __stdcall OnOKClicked(void *userObj);
			static void __stdcall OnCancelClicked(void *userObj);

			void UpdateFontText();
			void UpdateFontPreview();
		public:
			AVIRGISFontForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<Media::DrawEngine> eng, Text::String *fontName, Double fontSizePt, UInt32 fontColor);
			virtual ~AVIRGISFontForm();

			virtual void OnMonitorChanged();

			virtual void YUVParamChanged(NotNullPtr<const Media::IColorHandler::YUVPARAM> yuvParam);
			virtual void RGBParamChanged(NotNullPtr<const Media::IColorHandler::RGBPARAM2> rgbParam);

			NotNullPtr<Text::String> GetFontName() const;
			Double GetFontSizePt();
			UInt32 GetFontColor();

			Bool IsChanged();
		};
	}
}
#endif
