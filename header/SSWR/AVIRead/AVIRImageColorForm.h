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
			SSWR::AVIRead::AVIRCore *core;

			Media::StaticImage *srcImg;
			Media::StaticImage *destImg;
			Media::StaticImage *srcPrevImg;
			Media::StaticImage *destPrevImg;
			UI::GUIPictureBoxDD *previewCtrl;
			Media::RGBColorFilter *rgbFilter;
			Double currBVal;
			Double currCVal;
			Double currGVal;

			UI::GUILabel *lblBright;
			UI::GUIHScrollBar *hsbBright;
			UI::GUILabel *lblBrightV;
			UI::GUILabel *lblContr;
			UI::GUIHScrollBar *hsbContr;
			UI::GUILabel *lblContrV;
			UI::GUILabel *lblGamma;
			UI::GUIHScrollBar *hsbGamma;
			UI::GUILabel *lblGammaV;
			UI::GUIButton *btnOK;
			UI::GUIButton *btnCancel;
			UI::GUIButton *btnLastValue;

			static void __stdcall OnColorChg(void *userObj, UOSInt newPos);
			static void __stdcall OnOKClick(void *userObj);
			static void __stdcall OnCancelClick(void *userObj);
			static void __stdcall OnLastValueClick(void *userObj);
		public:
			AVIRImageColorForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Media::StaticImage *srcImg, Media::StaticImage *destImg, UI::GUIPictureBoxDD *previewCtrl);
			virtual ~AVIRImageColorForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
