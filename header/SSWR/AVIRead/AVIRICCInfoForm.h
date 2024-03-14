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
			NotNullPtr<UI::GUILabel> lblFileName;
			NotNullPtr<UI::GUITextBox> txtFileName;
			NotNullPtr<UI::GUILabel> lblInfo;
			NotNullPtr<UI::GUITextBox> txtInfo;
			NotNullPtr<UI::GUIButton> btnRLUT;
			NotNullPtr<UI::GUIButton> btnGLUT;
			NotNullPtr<UI::GUIButton> btnBLUT;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Media::ICCProfile *icc;

			static void __stdcall OnFileDrop(AnyType userObj, Data::DataArray<NotNullPtr<Text::String>> files);
			static void __stdcall OnRLUTClicked(AnyType userObj);
			static void __stdcall OnGLUTClicked(AnyType userObj);
			static void __stdcall OnBLUTClicked(AnyType userObj);
		public:
			AVIRICCInfoForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRICCInfoForm();

			virtual void OnMonitorChanged();
			void SetICCProfile(NotNullPtr<Media::ICCProfile> icc, Text::CStringNN fileName);
		};
	}
}
#endif
