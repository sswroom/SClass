#ifndef _SM_SSWR_AVIREAD_AVIRGISLINEFORM
#define _SM_SSWR_AVIREAD_AVIRGISLINEFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "SSWR/AVIRead/AVIRGISLineForm.h"
#include "SSWR/AVIRead/AVIRLineSelector.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUIGroupBox.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIPanel.h"
#include "UI/GUIPictureBox.h"
#include "UI/GUIHScrollBar.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRGISLineForm : public UI::GUIForm, public Media::IColorHandler
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			NotNullPtr<Media::ColorManagerSess> colorSess;
			Media::ColorConv *colorConv;
			NotNullPtr<Media::DrawEngine> eng;
			Double lineThick;
			UInt32 lineColor;
			Media::StaticImage *prevImg;

			UI::GUIPictureBox *pbPreview;
			NotNullPtr<UI::GUIPanel> pnlMain;
			UI::GUILabel *lblThick;
			UI::GUIHScrollBar *hsbThick;
			UI::GUILabel *lblThickV;
			UI::GUILabel *lblColor;
			UI::GUIPictureBox *pbColor;
            
			UI::GUIButton *btnOK;
			UI::GUIButton *btnCancel;

			static void __stdcall OnThickChanged(void *userObj, UOSInt newPos);
			static Bool __stdcall OnColorDown(void *userObj, Math::Coord2D<OSInt> scnPos, MouseButton btn);
			static void __stdcall OnOKClicked(void *userObj);
			static void __stdcall OnCancelClicked(void *userObj);
			void UpdatePreview();
		public:
			AVIRGISLineForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<Media::DrawEngine> eng, Double lineThick, UInt32 lineColor);
			virtual ~AVIRGISLineForm();

			virtual void OnMonitorChanged();

			virtual void YUVParamChanged(NotNullPtr<const Media::IColorHandler::YUVPARAM> yuvParam);
			virtual void RGBParamChanged(NotNullPtr<const Media::IColorHandler::RGBPARAM2> rgbParam);

			Double GetLineThick();
			UInt32 GetLineColor();
		};
	}
}
#endif
