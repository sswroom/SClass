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
			NotNullPtr<UI::GUITabControl> tcMain;

			NotNullPtr<UI::GUITabPage> tpMain;
			NotNullPtr<UI::GUIButton> btnChgProfile;
			NotNullPtr<UI::GUILabel> lblProfile;
			
			NotNullPtr<UI::GUITabPage> tpProfile;
			NotNullPtr<UI::GUIPanel> pnlProfile1;
			NotNullPtr<UI::GUIListBox> lbProfile;
			NotNullPtr<UI::GUIButton> btnProfileDel;
		//	NotNullPtr<UI::GUIHSplitter> hspProfile;
			NotNullPtr<UI::GUIPanel> pnlProfile2;
			NotNullPtr<UI::GUITextBox> txtProfileName;
			NotNullPtr<UI::GUITextBox> txtSuffix;

			NotNullPtr<UI::GUIGroupBox> grpSize;
			NotNullPtr<UI::GUIRadioButton> radSize;
			NotNullPtr<UI::GUITextBox> txtWidth;
			NotNullPtr<UI::GUITextBox> txtHeight;
			NotNullPtr<UI::GUIRadioButton> radDPI;
			NotNullPtr<UI::GUITextBox> txtHDPI;
			NotNullPtr<UI::GUITextBox> txtVDPI;

			NotNullPtr<UI::GUITextBox> txtWatermark;
			NotNullPtr<UI::GUIRadioButton> radTIFF;
			NotNullPtr<UI::GUIRadioButton> radJPEGQ;
			NotNullPtr<UI::GUIRadioButton> radJPEGSize;
			NotNullPtr<UI::GUIRadioButton> radPNG;
			NotNullPtr<UI::GUIRadioButton> radWEBPQ;
			NotNullPtr<UI::GUITextBox> txtJPEGQuality;
			NotNullPtr<UI::GUITextBox> txtJPEGSize;
			NotNullPtr<UI::GUITextBox> txtWEBPQuality;
			NotNullPtr<UI::GUIButton> btnProfileAdd;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			NotNullPtr<Media::ColorManagerSess> colorSess;
			Media::ProfiledResizer *resizer;

			static void __stdcall OnChgClicked(AnyType userObj);
			static void __stdcall OnFileDrop(AnyType userObj, Data::DataArray<NotNullPtr<Text::String>> files);
			static void __stdcall OnClickedDelProfile(AnyType userObj);
			static void __stdcall OnClickedAddProfile(AnyType userObj);
			void ChangeProfile();
			void UpdateProfileDisp();
			void UpdateProfileList();
		public:
			AVIRProfiledResizerForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRProfiledResizerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
