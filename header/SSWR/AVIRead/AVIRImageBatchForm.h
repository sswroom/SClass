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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			NotNullPtr<UI::GUIPanel> pnlCtrl;
			NotNullPtr<UI::GUIButton> btnFolder;
			NotNullPtr<UI::GUILabel> lblFolder;
			NotNullPtr<UI::GUIProgressBar> prgMain;
			NotNullPtr<SSWR::AVIRead::AVIRImageControl> icMain;
			NotNullPtr<UI::GUIHSplitter> hspLeft;
			NotNullPtr<UI::GUIPanel> pnlImage;
			NotNullPtr<UI::GUIPictureBoxDD> pbMain;

			NotNullPtr<UI::GUILabel> lblBright;
			NotNullPtr<UI::GUIHScrollBar> hsbBright;
			NotNullPtr<UI::GUILabel> lblBrightV;
			NotNullPtr<UI::GUIButton> btnBrightReset;
			NotNullPtr<UI::GUILabel> lblContr;
			NotNullPtr<UI::GUIHScrollBar> hsbContr;
			NotNullPtr<UI::GUILabel> lblContrV;
			NotNullPtr<UI::GUILabel> lblGamma;
			NotNullPtr<UI::GUIHScrollBar> hsbGamma;
			NotNullPtr<UI::GUILabel> lblGammaV;
			NotNullPtr<UI::GUIButton> btnGammaReset;
			NotNullPtr<UI::GUILabel> lblHDRLev;
			NotNullPtr<UI::GUIHScrollBar> hsbHDRLev;
			NotNullPtr<UI::GUILabel> lblHDRLevV;
			
			NotNullPtr<Media::ColorManagerSess> colorSess;
			Media::Resizer::LanczosResizer8_C8 *resizer;
			Media::StaticImage *dispImage;
			Media::StaticImage *previewImage;
			Media::StaticImage *filteredImage;
			Bool initPos;
			UOSInt selCnt;

			static void __stdcall OnFolderClicked(void *userObj);
			static void __stdcall OnImageChanged(void *userObj, Text::CString fileName, const SSWR::AVIRead::AVIRImageControl::ImageSetting *setting);
			static void __stdcall OnColorChg(void *userObj, UOSInt newPos);
			static Bool __stdcall OnFormClosing(void *userObj, CloseReason reason);
			static void __stdcall OnProgressUpdated(void *userObj, UOSInt finCnt);
			static void __stdcall OnKeyDown(void *userObj, UI::GUIControl::GUIKey key);
			static void __stdcall OnBrightResetClicked(void *userObj);
			static void __stdcall OnGammaResetClicked(void *userObj);
			static void __stdcall OnFilesDrop(void *userObj, NotNullPtr<Text::String> *files, UOSInt nFiles);

			void OpenFolder(NotNullPtr<Text::String> folder);
			void UpdatePreview();
		public:
			AVIRImageBatchForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRImageBatchForm();

			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();
		};
	}
}
#endif
