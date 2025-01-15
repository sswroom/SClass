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
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Media::ColorManagerSess> colorSess;
			NN<Media::ColorConv> colorConv;
			Bool changed;
			NN<Media::DrawEngine> eng;

			NN<Text::String> fontName;
			Double fontSizePt;
			UInt32 fontColor;
			Optional<Media::StaticImage> previewImage;

			NN<UI::GUIPictureBox> pbPreview;
			NN<UI::GUIPanel> pnlMain;
			NN<UI::GUILabel> lblFont;
			NN<UI::GUITextBox> txtFont;
			NN<UI::GUIButton> btnFont;
			NN<UI::GUILabel> lblColor;
			NN<UI::GUIPictureBox> pbColor;
			NN<UI::GUIButton> btnOK;
			NN<UI::GUIButton> btnCancel;

			static void __stdcall OnFontClicked(AnyType userObj);
			static Bool __stdcall OnColorClicked(AnyType userObj, Math::Coord2D<OSInt> scnPos, MouseButton btn);
			static void __stdcall OnOKClicked(AnyType userObj);
			static void __stdcall OnCancelClicked(AnyType userObj);

			void UpdateFontText();
			void UpdateFontPreview();
		public:
			AVIRGISFontForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Media::DrawEngine> eng, Optional<Text::String> fontName, Double fontSizePt, UInt32 fontColor);
			virtual ~AVIRGISFontForm();

			virtual void OnMonitorChanged();

			virtual void YUVParamChanged(NN<const Media::IColorHandler::YUVPARAM> yuvParam);
			virtual void RGBParamChanged(NN<const Media::IColorHandler::RGBPARAM2> rgbParam);

			NN<Text::String> GetFontName() const;
			Double GetFontSizePt();
			UInt32 GetFontColor();

			Bool IsChanged();
		};
	}
}
#endif
