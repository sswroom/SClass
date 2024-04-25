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
			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpMain;
			NN<UI::GUIButton> btnChgProfile;
			NN<UI::GUILabel> lblProfile;
			
			NN<UI::GUITabPage> tpProfile;
			NN<UI::GUIPanel> pnlProfile1;
			NN<UI::GUIListBox> lbProfile;
			NN<UI::GUIButton> btnProfileDel;
		//	NN<UI::GUIHSplitter> hspProfile;
			NN<UI::GUIPanel> pnlProfile2;
			NN<UI::GUITextBox> txtProfileName;
			NN<UI::GUITextBox> txtSuffix;

			NN<UI::GUIGroupBox> grpSize;
			NN<UI::GUIRadioButton> radSize;
			NN<UI::GUITextBox> txtWidth;
			NN<UI::GUITextBox> txtHeight;
			NN<UI::GUIRadioButton> radDPI;
			NN<UI::GUITextBox> txtHDPI;
			NN<UI::GUITextBox> txtVDPI;

			NN<UI::GUITextBox> txtWatermark;
			NN<UI::GUIRadioButton> radTIFF;
			NN<UI::GUIRadioButton> radJPEGQ;
			NN<UI::GUIRadioButton> radJPEGSize;
			NN<UI::GUIRadioButton> radPNG;
			NN<UI::GUIRadioButton> radWEBPQ;
			NN<UI::GUITextBox> txtJPEGQuality;
			NN<UI::GUITextBox> txtJPEGSize;
			NN<UI::GUITextBox> txtWEBPQuality;
			NN<UI::GUIButton> btnProfileAdd;

			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Media::ColorManagerSess> colorSess;
			Media::ProfiledResizer *resizer;

			static void __stdcall OnChgClicked(AnyType userObj);
			static void __stdcall OnFileDrop(AnyType userObj, Data::DataArray<NN<Text::String>> files);
			static void __stdcall OnClickedDelProfile(AnyType userObj);
			static void __stdcall OnClickedAddProfile(AnyType userObj);
			void ChangeProfile();
			void UpdateProfileDisp();
			void UpdateProfileList();
		public:
			AVIRProfiledResizerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRProfiledResizerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
