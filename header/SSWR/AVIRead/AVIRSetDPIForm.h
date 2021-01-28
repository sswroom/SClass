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
			UI::GUIPictureBoxSimple *pbPreview;
			UI::GUIPanel *pnlDPI;
			UI::GUIPanel *pnlBtn;
			UI::GUILabel *lblDPI;
			UI::GUIHScrollBar *hsbDPI;
			UI::GUILabel *lblDPIV;
			UI::GUILabel *lblMagnifyRatio;
			UI::GUITextBox *txtMagnifyRatio;
			UI::GUILabel *lblDesktopDPI;
			UI::GUITextBox *txtDesktopDPI;
			UI::GUIButton *btnStandard;
			UI::GUIButton *btn1x;
			UI::GUIButton *btnLaptop;
			UI::GUIButton *btnOK;
			UI::GUIButton *btnCancel;

			SSWR::AVIRead::AVIRCore *core;
			Media::StaticImage *pimg;

			static void __stdcall OnOKClicked(void *userObj);
			static void __stdcall OnCancelClicked(void *userObj);
			static void __stdcall OnPreviewChanged(void *userObj);
			static void __stdcall OnDPIChanged(void *userObj, OSInt newVal);
			static void __stdcall OnStandardClicked(void *userObj);
			static void __stdcall On1xClicked(void *userObj);
			static void __stdcall OnLaptopClicked(void *userObj);
			void UpdatePreview();
		public:
			AVIRSetDPIForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRSetDPIForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
