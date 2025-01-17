#ifndef _SM_SSWR_AVIREAD_AVIRGENIMAGEFORM
#define _SM_SSWR_AVIREAD_AVIRGENIMAGEFORM
#include "Media/ImageGen/ImageGenMgr.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIGroupBox.h"
#include "UI/GUIRadioButton.h"
#include "UI/GUITextBox.h"
#include "SSWR/AVIRead/AVIRCore.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRGenImageForm : public UI::GUIForm
		{
		private:
			NN<UI::GUIComboBox> cboGenerator;
			NN<UI::GUIComboBox> cboColorProfile;

			NN<UI::GUITextBox> txtWidth;
			NN<UI::GUITextBox> txtHeight;

			NN<UI::GUIButton> btnGenerate;
			NN<UI::GUIButton> btnCancel;
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Media::ImageGen::ImageGenMgr> imgGenMgr;

			static void __stdcall GenerateClicked(AnyType userObj);
			static void __stdcall CancelClicked(AnyType userObj);
		public:
			AVIRGenImageForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRGenImageForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
