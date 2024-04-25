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
			NN<SSWR::AVIRead::AVIRCore> core;

			NN<UI::GUILabel> lblErrorCode;
			NN<UI::GUITextBox> txtErrorCode;

			NN<UI::GUILabel> lblErrorName;
			NN<UI::GUITextBox> txtErrorName;

			static void __stdcall OnErrorCodeChanged(AnyType userObj);

		public:
			AVIRWindowsErrorForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRWindowsErrorForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
