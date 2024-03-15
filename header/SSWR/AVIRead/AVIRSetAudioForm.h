#ifndef _SM_SSWR_AVIREAD_AVIRSETAUDIOFORM
#define _SM_SSWR_AVIREAD_AVIRSETAUDIOFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRSetAudioForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			NotNullPtr<UI::GUILabel> lblDevice;
			NotNullPtr<UI::GUIListBox> lbDevice;

			NotNullPtr<UI::GUIButton> btnOK;
			NotNullPtr<UI::GUIButton> btnCancel;

			static void __stdcall OnOKClick(AnyType userObj);
			static void __stdcall OnCancelClick(AnyType userObj);
		public:
			AVIRSetAudioForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRSetAudioForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
