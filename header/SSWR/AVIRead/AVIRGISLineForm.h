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
			SSWR::AVIRead::AVIRCore *core;
			Media::ColorManagerSess *colorSess;
			Media::ColorConv *colorConv;
			Media::DrawEngine *eng;
			UOSInt lineThick;
			UInt32 lineColor;
			Media::StaticImage *prevImg;

			UI::GUIPictureBox *pbPreview;
			UI::GUIPanel *pnlMain;
			UI::GUILabel *lblThick;
			UI::GUIHScrollBar *hsbThick;
			UI::GUILabel *lblThickV;
			UI::GUILabel *lblColor;
			UI::GUIPictureBox *pbColor;
            
			UI::GUIButton *btnOK;
			UI::GUIButton *btnCancel;

			static void __stdcall OnThickChanged(void *userObj, UOSInt newPos);
			static Bool __stdcall OnColorDown(void *userObj, OSInt x, OSInt y, MouseButton btn);
			static void __stdcall OnOKClicked(void *userObj);
			static void __stdcall OnCancelClicked(void *userObj);
			void UpdatePreview();
		public:
			AVIRGISLineForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Media::DrawEngine *eng, UOSInt lineThick, UInt32 lineColor);
			virtual ~AVIRGISLineForm();

			virtual void OnMonitorChanged();

			virtual void YUVParamChanged(const Media::IColorHandler::YUVPARAM *yuvParam);
			virtual void RGBParamChanged(const Media::IColorHandler::RGBPARAM2 *rgbParam);

			UOSInt GetLineThick();
			UInt32 GetLineColor();
		};
	}
}
#endif
