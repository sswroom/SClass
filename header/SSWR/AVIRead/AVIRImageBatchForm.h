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
			NN<SSWR::AVIRead::AVIRCore> core;

			NN<UI::GUIPanel> pnlCtrl;
			NN<UI::GUIButton> btnFolder;
			NN<UI::GUILabel> lblFolder;
			NN<UI::GUIProgressBar> prgMain;
			NN<SSWR::AVIRead::AVIRImageControl> icMain;
			NN<UI::GUIHSplitter> hspLeft;
			NN<UI::GUIPanel> pnlImage;
			NN<UI::GUIPictureBoxDD> pbMain;

			NN<UI::GUILabel> lblBright;
			NN<UI::GUIHScrollBar> hsbBright;
			NN<UI::GUILabel> lblBrightV;
			NN<UI::GUIButton> btnBrightReset;
			NN<UI::GUILabel> lblContr;
			NN<UI::GUIHScrollBar> hsbContr;
			NN<UI::GUILabel> lblContrV;
			NN<UI::GUILabel> lblGamma;
			NN<UI::GUIHScrollBar> hsbGamma;
			NN<UI::GUILabel> lblGammaV;
			NN<UI::GUIButton> btnGammaReset;
			NN<UI::GUILabel> lblHDRLev;
			NN<UI::GUIHScrollBar> hsbHDRLev;
			NN<UI::GUILabel> lblHDRLevV;
			
			NN<Media::ColorManagerSess> colorSess;
			Media::Resizer::LanczosResizer8_C8 *resizer;
			Optional<Media::StaticImage> dispImage;
			Media::StaticImage *previewImage;
			Media::StaticImage *filteredImage;
			Bool initPos;
			UOSInt selCnt;

			static void __stdcall OnFolderClicked(AnyType userObj);
			static void __stdcall OnImageChanged(AnyType userObj, Text::CString fileName, Optional<const SSWR::AVIRead::AVIRImageControl::ImageSetting> setting);
			static void __stdcall OnColorChg(AnyType userObj, UOSInt newPos);
			static Bool __stdcall OnFormClosing(AnyType userObj, CloseReason reason);
			static void __stdcall OnProgressUpdated(AnyType userObj, UOSInt finCnt);
			static void __stdcall OnKeyDown(AnyType userObj, UI::GUIControl::GUIKey key);
			static void __stdcall OnBrightResetClicked(AnyType userObj);
			static void __stdcall OnGammaResetClicked(AnyType userObj);
			static void __stdcall OnFilesDrop(AnyType userObj, Data::DataArray<NN<Text::String>> files);

			void OpenFolder(NN<Text::String> folder);
			void UpdatePreview();
		public:
			AVIRImageBatchForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRImageBatchForm();

			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();
		};
	}
}
#endif
