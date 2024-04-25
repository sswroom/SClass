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
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Net::SocketFactory> sockf;
			NN<UI::GUILabel> lblHost;
			NN<UI::GUITextBox> txtHost;
			NN<UI::GUILabel> lblPort;
			NN<UI::GUITextBox> txtPort;
			NN<UI::GUILabel> lblUserName;
			NN<UI::GUITextBox> txtUserName;
			NN<UI::GUIButton> btnQuery;

			NN<UI::GUILabel> lblStatus;
			NN<UI::GUITextBox> txtStatus;
			NN<UI::GUILabel> lblHostKey;
			NN<UI::GUITextBox> txtHostKey;
			NN<UI::GUILabel> lblBanner;
			NN<UI::GUITextBox> txtBanner;
			NN<UI::GUILabel> lblAuthMeth;
			NN<UI::GUIListBox> lbAuthMeth;
			NN<UI::GUITextBox> txtDesc;

		private:
			static void __stdcall OnQueryClicked(AnyType userObj);
			
		public:
			AVIRSSHInfoForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRSSHInfoForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
