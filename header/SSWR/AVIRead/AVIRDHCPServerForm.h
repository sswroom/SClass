#ifndef _SM_SSWR_AVIREAD_AVIRDHCPSERVERFORM
#define _SM_SSWR_AVIREAD_AVIRDHCPSERVERFORM
#include "Net/DHCPServer.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRDHCPServerForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			NotNullPtr<Net::SocketFactory> sockf;
			Net::DHCPServer *svr;

			NotNullPtr<UI::GUIPanel> pnlControl;
			NotNullPtr<UI::GUILabel> lblIP;
			NotNullPtr<UI::GUIComboBox> cboIP;
			NotNullPtr<UI::GUILabel> lblSubnet;
			NotNullPtr<UI::GUITextBox> txtSubnet;
			NotNullPtr<UI::GUILabel> lblFirstIP;
			NotNullPtr<UI::GUITextBox> txtFirstIP;
			NotNullPtr<UI::GUILabel> lblDevCount;
			NotNullPtr<UI::GUITextBox> txtDevCount;
			NotNullPtr<UI::GUILabel> lblGateway;
			NotNullPtr<UI::GUITextBox> txtGateway;
			NotNullPtr<UI::GUILabel> lblDNS1;
			NotNullPtr<UI::GUITextBox> txtDNS1;
			NotNullPtr<UI::GUILabel> lblDNS2;
			NotNullPtr<UI::GUITextBox> txtDNS2;
			NotNullPtr<UI::GUIButton> btnStart;
			UI::GUIListView *lvDevices;

			static void __stdcall OnStartClicked(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
		public:
			AVIRDHCPServerForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRDHCPServerForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
