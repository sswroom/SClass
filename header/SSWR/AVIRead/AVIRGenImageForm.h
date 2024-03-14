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
			NotNullPtr<UI::GUIComboBox> cboGenerator;
			NotNullPtr<UI::GUIComboBox> cboColorProfile;

			NotNullPtr<UI::GUITextBox> txtWidth;
			NotNullPtr<UI::GUITextBox> txtHeight;

			NotNullPtr<UI::GUIButton> btnGenerate;
			NotNullPtr<UI::GUIButton> btnCancel;
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Media::ImageGen::ImageGenMgr *imgGenMgr;

			static void __stdcall GenerateClicked(AnyType userObj);
			static void __stdcall CancelClicked(AnyType userObj);
		public:
			AVIRGenImageForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRGenImageForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
