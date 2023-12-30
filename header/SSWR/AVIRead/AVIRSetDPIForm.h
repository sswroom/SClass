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
			NotNullPtr<UI::GUIPictureBoxSimple> pbPreview;
			NotNullPtr<UI::GUIPanel> pnlDPI;
			NotNullPtr<UI::GUIPanel> pnlBtn;
			NotNullPtr<UI::GUILabel> lblDPI;
			UI::GUIHScrollBar *hsbDPI;
			NotNullPtr<UI::GUILabel> lblDPIV;
			NotNullPtr<UI::GUILabel> lblMagnifyRatio;
			NotNullPtr<UI::GUITextBox> txtMagnifyRatio;
			NotNullPtr<UI::GUILabel> lblDesktopDPI;
			NotNullPtr<UI::GUITextBox> txtDesktopDPI;
			NotNullPtr<UI::GUIButton> btnStandard;
			NotNullPtr<UI::GUIButton> btn1x;
			NotNullPtr<UI::GUIButton> btnLaptop;
			NotNullPtr<UI::GUIButton> btnOK;
			NotNullPtr<UI::GUIButton> btnCancel;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Media::StaticImage *pimg;

			static void __stdcall OnOKClicked(void *userObj);
			static void __stdcall OnCancelClicked(void *userObj);
			static void __stdcall OnPreviewChanged(void *userObj);
			static void __stdcall OnDPIChanged(void *userObj, UOSInt newVal);
			static void __stdcall OnStandardClicked(void *userObj);
			static void __stdcall On1xClicked(void *userObj);
			static void __stdcall OnLaptopClicked(void *userObj);
			void UpdatePreview();
		public:
			AVIRSetDPIForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRSetDPIForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
