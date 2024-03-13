#ifndef _SM_SSWR_AVIREAD_AVIRWHOISCLIENTFORM
#define _SM_SSWR_AVIREAD_AVIRWHOISCLIENTFORM

#include "Net/WhoisClient.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRWHOISClientForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			NotNullPtr<UI::GUIPanel> pnlRequest;
			NotNullPtr<UI::GUILabel> lblIP;
			NotNullPtr<UI::GUITextBox> txtIP;
			NotNullPtr<UI::GUILabel> lblRespTime;
			NotNullPtr<UI::GUITextBox> txtRespTime;
			NotNullPtr<UI::GUIButton> btnRequest;
			NotNullPtr<UI::GUIListBox> lbResponse;

			NotNullPtr<Net::SocketFactory> sockf;

			static void __stdcall OnRequestClicked(void *userObj);
		public:
			AVIRWHOISClientForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRWHOISClientForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
