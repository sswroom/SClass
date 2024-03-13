#ifndef _SM_SSWR_AVIREAD_AVIRWINDOWSERRORFORM
#define _SM_SSWR_AVIREAD_AVIRWINDOWSERRORFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRWindowsErrorForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			NotNullPtr<UI::GUILabel> lblErrorCode;
			NotNullPtr<UI::GUITextBox> txtErrorCode;

			NotNullPtr<UI::GUILabel> lblErrorName;
			NotNullPtr<UI::GUITextBox> txtErrorName;

			static void __stdcall OnErrorCodeChanged(void *userObj);

		public:
			AVIRWindowsErrorForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRWindowsErrorForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
