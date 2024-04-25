#ifndef _SM_SSWR_AVIREAD_AVIRSETDPIFORM
#define _SM_SSWR_AVIREAD_AVIRSETDPIFORM

#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUIHScrollBar.h"
#include "UI/GUILabel.h"
#include "UI/GUIPanel.h"
#include "UI/GUIPictureBoxSimple.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRSetDPIForm : public UI::GUIForm
		{
		private:
			NN<UI::GUIPictureBoxSimple> pbPreview;
			NN<UI::GUIPanel> pnlDPI;
			NN<UI::GUIPanel> pnlBtn;
			NN<UI::GUILabel> lblDPI;
			NN<UI::GUIHScrollBar> hsbDPI;
			NN<UI::GUILabel> lblDPIV;
			NN<UI::GUILabel> lblMagnifyRatio;
			NN<UI::GUITextBox> txtMagnifyRatio;
			NN<UI::GUILabel> lblDesktopDPI;
			NN<UI::GUITextBox> txtDesktopDPI;
			NN<UI::GUIButton> btnStandard;
			NN<UI::GUIButton> btn1x;
			NN<UI::GUIButton> btnLaptop;
			NN<UI::GUIButton> btnOK;
			NN<UI::GUIButton> btnCancel;

			NN<SSWR::AVIRead::AVIRCore> core;
			Media::StaticImage *pimg;

			static void __stdcall OnOKClicked(AnyType userObj);
			static void __stdcall OnCancelClicked(AnyType userObj);
			static void __stdcall OnPreviewChanged(AnyType userObj);
			static void __stdcall OnDPIChanged(AnyType userObj, UOSInt newVal);
			static void __stdcall OnStandardClicked(AnyType userObj);
			static void __stdcall On1xClicked(AnyType userObj);
			static void __stdcall OnLaptopClicked(AnyType userObj);
			void UpdatePreview();
		public:
			AVIRSetDPIForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRSetDPIForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
