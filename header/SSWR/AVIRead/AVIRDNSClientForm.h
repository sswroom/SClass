#ifndef _SM_SSWR_AVIREAD_AVIRDNSCLIENTFORM
#define _SM_SSWR_AVIREAD_AVIRDNSCLIENTFORM

#include "Net/DNSClient.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIRadioButton.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRDNSClientForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			NotNullPtr<UI::GUIPanel> pnlRequest;
			NotNullPtr<UI::GUILabel> lblServer;
			NotNullPtr<UI::GUITextBox> txtServer;
			NotNullPtr<UI::GUILabel> lblRequest;
			NotNullPtr<UI::GUITextBox> txtRequest;
			NotNullPtr<UI::GUIComboBox> cboRequest;
//			NotNullPtr<UI::GUIRadioButton> radRequestNormal;
//			NotNullPtr<UI::GUIRadioButton> radRequestMX;
			NotNullPtr<UI::GUIButton> btnRequest;
			NotNullPtr<UI::GUILabel> lblRequestTime;
			NotNullPtr<UI::GUITextBox> txtRequestTime;
			NotNullPtr<UI::GUIListBox> lbAnswer;
			NotNullPtr<UI::GUIHSplitter> hspAnswer;
			NotNullPtr<UI::GUIPanel> pnlAnswer;
			NotNullPtr<UI::GUILabel> lblAnsName;
			NotNullPtr<UI::GUITextBox> txtAnsName;
			NotNullPtr<UI::GUILabel> lblAnsType;
			NotNullPtr<UI::GUITextBox> txtAnsType;
			NotNullPtr<UI::GUILabel> lblAnsClass;
			NotNullPtr<UI::GUITextBox> txtAnsClass;
			NotNullPtr<UI::GUILabel> lblAnsTTL;
			NotNullPtr<UI::GUITextBox> txtAnsTTL;
			NotNullPtr<UI::GUILabel> lblAnsRD;
			NotNullPtr<UI::GUITextBox> txtAnsRD;

			Data::ArrayList<Net::DNSClient::RequestAnswer*> ansList;
			NotNullPtr<Net::SocketFactory> sockf;

			static void __stdcall OnRequestClicked(AnyType userObj);
			static void __stdcall OnAnswerSelChg(AnyType userObj);
		public:
			AVIRDNSClientForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRDNSClientForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
