#ifndef _SM_SSWR_AVIREAD_AVIRSSHINFOFORM
#define _SM_SSWR_AVIREAD_AVIRSSHINFOFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRSSHInfoForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			NotNullPtr<Net::SocketFactory> sockf;
			NotNullPtr<UI::GUILabel> lblHost;
			NotNullPtr<UI::GUITextBox> txtHost;
			NotNullPtr<UI::GUILabel> lblPort;
			NotNullPtr<UI::GUITextBox> txtPort;
			NotNullPtr<UI::GUILabel> lblUserName;
			NotNullPtr<UI::GUITextBox> txtUserName;
			NotNullPtr<UI::GUIButton> btnQuery;

			NotNullPtr<UI::GUILabel> lblStatus;
			NotNullPtr<UI::GUITextBox> txtStatus;
			NotNullPtr<UI::GUILabel> lblHostKey;
			NotNullPtr<UI::GUITextBox> txtHostKey;
			NotNullPtr<UI::GUILabel> lblBanner;
			NotNullPtr<UI::GUITextBox> txtBanner;
			NotNullPtr<UI::GUILabel> lblAuthMeth;
			NotNullPtr<UI::GUIListBox> lbAuthMeth;
			NotNullPtr<UI::GUITextBox> txtDesc;

		private:
			static void __stdcall OnQueryClicked(AnyType userObj);
			
		public:
			AVIRSSHInfoForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRSSHInfoForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
