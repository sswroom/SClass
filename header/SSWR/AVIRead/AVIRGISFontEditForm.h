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
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Media::ColorManagerSess> colorSess;
			Media::ColorConv *colorConv;
			NN<Map::MapEnv> env;
			UOSInt fontStyle;
			Bool changed;
			NN<Media::DrawEngine> eng;

			Text::String *currFontName;
			Double currFontSizePt;
			UInt32 currColor;
			UOSInt currBuffSize;
			UInt32 currBuffColor;
			Bool isBold;
			Media::StaticImage *previewImage;

			NN<UI::GUIPictureBox> pbFontPreview;
			NN<UI::GUILabel> lblStyleName;
			NN<UI::GUITextBox> txtStyleName;
			NN<UI::GUILabel> lblFontName;
			NN<UI::GUITextBox> txtFontName;
			NN<UI::GUIButton> btnFontName;
			NN<UI::GUILabel> lblFontColor;
			NN<UI::GUIPictureBox> pbFontColor;
			NN<UI::GUILabel> lblBufferSize;
			NN<UI::GUIHScrollBar> hsbBufferSize;
			NN<UI::GUILabel> lblBufferColor;
			NN<UI::GUIPictureBox> pbBufferColor;
			NN<UI::GUIButton> btnOK;
			NN<UI::GUIButton> btnCancel;

			static void __stdcall FontNameClicked(AnyType userObj);
			static Bool __stdcall FontColorClicked(AnyType userObj, Math::Coord2D<OSInt> scnPos, MouseButton mouseBtn);
			static void __stdcall BufferSizeChanged(AnyType userObj, UOSInt scrollPos);
			static Bool __stdcall BufferColorClicked(AnyType userObj, Math::Coord2D<OSInt> scnPos, MouseButton mouseBtn);
			static void __stdcall FontsDoubleClicked(AnyType userObj);
			static void __stdcall OKClicked(AnyType userObj);
			static void __stdcall CancelClicked(AnyType userObj);

			void UpdateFontPreview();
			void UpdateDisplay();
		public:
			AVIRGISFontEditForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Map::MapEnv> env, NN<Media::DrawEngine> eng, UOSInt fontStyle);
			virtual ~AVIRGISFontEditForm();

			virtual void OnMonitorChanged();

			virtual void YUVParamChanged(NN<const Media::IColorHandler::YUVPARAM> yuvParam);
			virtual void RGBParamChanged(NN<const Media::IColorHandler::RGBPARAM2> rgbParam);
		};
	}
}
#endif
