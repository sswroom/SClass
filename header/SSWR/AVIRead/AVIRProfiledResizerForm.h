#ifndef _SM_SSWR_AVIREAD_AVIRPROFILEDRESIZERFORM
#define _SM_SSWR_AVIREAD_AVIRPROFILEDRESIZERFORM
#include "Media/ProfiledResizer.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUICore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUIGroupBox.h"
//#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIRadioButton.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRProfiledResizerForm : public UI::GUIForm
		{
		private:
			UI::GUITabControl *tcMain;

			NotNullPtr<UI::GUITabPage> tpMain;
			NotNullPtr<UI::GUIButton> btnChgProfile;
			NotNullPtr<UI::GUILabel> lblProfile;
			
			NotNullPtr<UI::GUITabPage> tpProfile;
			NotNullPtr<UI::GUIPanel> pnlProfile1;
			UI::GUIListBox *lbProfile;
			NotNullPtr<UI::GUIButton> btnProfileDel;
		//	NotNullPtr<UI::GUIHSplitter> hspProfile;
			NotNullPtr<UI::GUIPanel> pnlProfile2;
			NotNullPtr<UI::GUITextBox> txtProfileName;
			NotNullPtr<UI::GUITextBox> txtSuffix;

			NotNullPtr<UI::GUIGroupBox> grpSize;
			UI::GUIRadioButton *radSize;
			NotNullPtr<UI::GUITextBox> txtWidth;
			NotNullPtr<UI::GUITextBox> txtHeight;
			UI::GUIRadioButton *radDPI;
			NotNullPtr<UI::GUITextBox> txtHDPI;
			NotNullPtr<UI::GUITextBox> txtVDPI;

			NotNullPtr<UI::GUITextBox> txtWatermark;
			UI::GUIRadioButton *radTIFF;
			UI::GUIRadioButton *radJPEGQ;
			UI::GUIRadioButton *radJPEGSize;
			UI::GUIRadioButton *radPNG;
			UI::GUIRadioButton *radWEBPQ;
			NotNullPtr<UI::GUITextBox> txtJPEGQuality;
			NotNullPtr<UI::GUITextBox> txtJPEGSize;
			NotNullPtr<UI::GUITextBox> txtWEBPQuality;
			NotNullPtr<UI::GUIButton> btnProfileAdd;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			NotNullPtr<Media::ColorManagerSess> colorSess;
			Media::ProfiledResizer *resizer;

			static void __stdcall OnChgClicked(void *userObj);
			static void __stdcall OnFileDrop(void *userObj, NotNullPtr<Text::String> *files, UOSInt nFiles);
			static void __stdcall OnClickedDelProfile(void *userObj);
			static void __stdcall OnClickedAddProfile(void *userObj);
			void ChangeProfile();
			void UpdateProfileDisp();
			void UpdateProfileList();
		public:
			AVIRProfiledResizerForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRProfiledResizerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
