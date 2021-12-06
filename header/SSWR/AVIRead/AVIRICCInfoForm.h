#ifndef _SM_SSWR_AVIREAD_AVIRICCINFOFORM
#define _SM_SSWR_AVIREAD_AVIRICCINFOFORM
#include "Media/ICCProfile.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRICCInfoForm : public UI::GUIForm
		{
		private:
			UI::GUILabel *lblFileName;
			UI::GUITextBox *txtFileName;
			UI::GUILabel *lblInfo;
			UI::GUITextBox *txtInfo;
			UI::GUIButton *btnRLUT;
			UI::GUIButton *btnGLUT;
			UI::GUIButton *btnBLUT;

			SSWR::AVIRead::AVIRCore *core;
			Media::ICCProfile *icc;

			static void __stdcall OnFileDrop(void *userObj, const UTF8Char **files, UOSInt fileCnt);
			static void __stdcall OnRLUTClicked(void *userObj);
			static void __stdcall OnGLUTClicked(void *userObj);
			static void __stdcall OnBLUTClicked(void *userObj);
		public:
			AVIRICCInfoForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRICCInfoForm();

			virtual void OnMonitorChanged();
			void SetICCProfile(Media::ICCProfile *icc, const UTF8Char *fileName);
		};
	}
}
#endif
