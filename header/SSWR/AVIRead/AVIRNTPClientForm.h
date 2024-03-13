#ifndef _SM_SSWR_AVIREAD_AVIRNTPCLIENTFORM
#define _SM_SSWR_AVIREAD_AVIRNTPCLIENTFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRNTPClientForm : public UI::GUIForm
		{
		private:
			NotNullPtr<UI::GUILabel> lblServer;
			NotNullPtr<UI::GUIComboBox> cboServer;
			NotNullPtr<UI::GUILabel> lblServerTime;
			NotNullPtr<UI::GUITextBox> txtServerTime;
			NotNullPtr<UI::GUILabel> lblStatus;
			NotNullPtr<UI::GUITextBox> txtStatus;
			NotNullPtr<UI::GUIButton> btnGetTime;
			NotNullPtr<UI::GUIButton> btnSyncTime;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			static void __stdcall OnGetTimeClicked(void *userObj);
			static void __stdcall OnSyncTimeClicked(void *userObj);
		public:
			AVIRNTPClientForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRNTPClientForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
