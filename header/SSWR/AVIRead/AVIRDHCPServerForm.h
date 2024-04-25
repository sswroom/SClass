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
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Net::SocketFactory> sockf;
			Net::DHCPServer *svr;

			NN<UI::GUIPanel> pnlControl;
			NN<UI::GUILabel> lblIP;
			NN<UI::GUIComboBox> cboIP;
			NN<UI::GUILabel> lblSubnet;
			NN<UI::GUITextBox> txtSubnet;
			NN<UI::GUILabel> lblFirstIP;
			NN<UI::GUITextBox> txtFirstIP;
			NN<UI::GUILabel> lblDevCount;
			NN<UI::GUITextBox> txtDevCount;
			NN<UI::GUILabel> lblGateway;
			NN<UI::GUITextBox> txtGateway;
			NN<UI::GUILabel> lblDNS1;
			NN<UI::GUITextBox> txtDNS1;
			NN<UI::GUILabel> lblDNS2;
			NN<UI::GUITextBox> txtDNS2;
			NN<UI::GUIButton> btnStart;
			NN<UI::GUIListView> lvDevices;

			static void __stdcall OnStartClicked(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
		public:
			AVIRDHCPServerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRDHCPServerForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
