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
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Media::ColorManagerSess> colorSess;
			NN<Media::ColorConv> colorConv;
			NN<Media::DrawEngine> eng;
			Double lineThick;
			UInt32 lineColor;
			Optional<Media::StaticImage> prevImg;

			NN<UI::GUIPictureBox> pbPreview;
			NN<UI::GUIPanel> pnlMain;
			NN<UI::GUILabel> lblThick;
			NN<UI::GUIHScrollBar> hsbThick;
			NN<UI::GUILabel> lblThickV;
			NN<UI::GUILabel> lblColor;
			NN<UI::GUIPictureBox> pbColor;
            
			NN<UI::GUIButton> btnOK;
			NN<UI::GUIButton> btnCancel;

			static void __stdcall OnThickChanged(AnyType userObj, UOSInt newPos);
			static Bool __stdcall OnColorDown(AnyType userObj, Math::Coord2D<OSInt> scnPos, MouseButton btn);
			static void __stdcall OnOKClicked(AnyType userObj);
			static void __stdcall OnCancelClicked(AnyType userObj);
			void UpdatePreview();
		public:
			AVIRGISLineForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Media::DrawEngine> eng, Double lineThick, UInt32 lineColor);
			virtual ~AVIRGISLineForm();

			virtual void OnMonitorChanged();

			virtual void YUVParamChanged(NN<const Media::IColorHandler::YUVPARAM> yuvParam);
			virtual void RGBParamChanged(NN<const Media::IColorHandler::RGBPARAM2> rgbParam);

			Double GetLineThick();
			UInt32 GetLineColor();
		};
	}
}
#endif
