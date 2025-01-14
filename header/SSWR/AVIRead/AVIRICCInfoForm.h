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
			NN<UI::GUILabel> lblFileName;
			NN<UI::GUITextBox> txtFileName;
			NN<UI::GUILabel> lblInfo;
			NN<UI::GUITextBox> txtInfo;
			NN<UI::GUIButton> btnRLUT;
			NN<UI::GUIButton> btnGLUT;
			NN<UI::GUIButton> btnBLUT;

			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Media::ICCProfile> icc;

			static void __stdcall OnFileDrop(AnyType userObj, Data::DataArray<NN<Text::String>> files);
			static void __stdcall OnRLUTClicked(AnyType userObj);
			static void __stdcall OnGLUTClicked(AnyType userObj);
			static void __stdcall OnBLUTClicked(AnyType userObj);
		public:
			AVIRICCInfoForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRICCInfoForm();

			virtual void OnMonitorChanged();
			void SetICCProfile(NN<Media::ICCProfile> icc, Text::CStringNN fileName);
		};
	}
}
#endif
