#ifndef _SM_SSWR_AVIREAD_AVIRIMAGECOLORFORM
#define _SM_SSWR_AVIREAD_AVIRIMAGECOLORFORM
#include "Media/RGBColorFilter.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUIHScrollBar.h"
#include "UI/GUILabel.h"
#include "UI/GUIPictureBoxDD.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRImageColorForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;

			NN<const Media::StaticImage> srcImg;
			NN<Media::StaticImage> destImg;
			Optional<Media::StaticImage> srcPrevImg;
			Optional<Media::StaticImage> destPrevImg;
			NN<UI::GUIPictureBoxDD> previewCtrl;
			Media::RGBColorFilter *rgbFilter;
			Double currBVal;
			Double currCVal;
			Double currGVal;

			NN<UI::GUILabel> lblBright;
			NN<UI::GUIHScrollBar> hsbBright;
			NN<UI::GUILabel> lblBrightV;
			NN<UI::GUILabel> lblContr;
			NN<UI::GUIHScrollBar> hsbContr;
			NN<UI::GUILabel> lblContrV;
			NN<UI::GUILabel> lblGamma;
			NN<UI::GUIHScrollBar> hsbGamma;
			NN<UI::GUILabel> lblGammaV;
			NN<UI::GUIButton> btnOK;
			NN<UI::GUIButton> btnCancel;
			NN<UI::GUIButton> btnLastValue;

			static void __stdcall OnColorChg(AnyType userObj, UOSInt newPos);
			static void __stdcall OnOKClick(AnyType userObj);
			static void __stdcall OnCancelClick(AnyType userObj);
			static void __stdcall OnLastValueClick(AnyType userObj);
		public:
			AVIRImageColorForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<const Media::StaticImage> srcImg, NN<Media::StaticImage> destImg, NN<UI::GUIPictureBoxDD> previewCtrl);
			virtual ~AVIRImageColorForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
