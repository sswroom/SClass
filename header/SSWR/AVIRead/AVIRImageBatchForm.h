#ifndef _SM_SSWR_AVIREAD_AVIRIMAGEBATCHFORM
#define _SM_SSWR_AVIREAD_AVIRIMAGEBATCHFORM
#include "Media/Resizer/LanczosResizer8_C8.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "SSWR/AVIRead/AVIRImageControl.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIHScrollBar.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIPanel.h"
#include "UI/GUIPictureBoxDD.h"
#include "UI/GUIProgressBar.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRImageBatchForm : public UI::GUIForm
		{
		private:
			SSWR::AVIRead::AVIRCore *core;

			UI::GUIPanel *pnlCtrl;
			UI::GUIButton *btnFolder;
			UI::GUILabel *lblFolder;
			UI::GUIProgressBar *prgMain;
			SSWR::AVIRead::AVIRImageControl *icMain;
			UI::GUIHSplitter *hspLeft;
			UI::GUIPanel *pnlImage;
			UI::GUIPictureBoxDD *pbMain;

			UI::GUILabel *lblBright;
			UI::GUIHScrollBar *hsbBright;
			UI::GUILabel *lblBrightV;
			UI::GUIButton *btnBrightReset;
			UI::GUILabel *lblContr;
			UI::GUIHScrollBar *hsbContr;
			UI::GUILabel *lblContrV;
			UI::GUILabel *lblGamma;
			UI::GUIHScrollBar *hsbGamma;
			UI::GUILabel *lblGammaV;
			UI::GUIButton *btnGammaReset;
			UI::GUILabel *lblHDRLev;
			UI::GUIHScrollBar *hsbHDRLev;
			UI::GUILabel *lblHDRLevV;
			
			Media::ColorManagerSess *colorSess;
			Media::Resizer::LanczosResizer8_C8 *resizer;
			Media::StaticImage *dispImage;
			Media::StaticImage *previewImage;
			Media::StaticImage *filteredImage;
			Bool initPos;
			UOSInt selCnt;

			static void __stdcall OnFolderClicked(void *userObj);
			static void __stdcall OnImageChanged(void *userObj, const UTF8Char *fileName, const SSWR::AVIRead::AVIRImageControl::ImageSetting *setting);
			static void __stdcall OnColorChg(void *userObj, UOSInt newPos);
			static Bool __stdcall OnFormClosing(void *userObj, CloseReason reason);
			static void __stdcall OnProgressUpdated(void *userObj, UOSInt finCnt);
			static void __stdcall OnKeyDown(void *userObj, UI::GUIControl::GUIKey key);
			static void __stdcall OnBrightResetClicked(void *userObj);
			static void __stdcall OnGammaResetClicked(void *userObj);

			void UpdatePreview();
		public:
			AVIRImageBatchForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRImageBatchForm();

			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();
		};
	}
}
#endif
