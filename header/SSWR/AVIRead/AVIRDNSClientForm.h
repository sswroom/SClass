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
			NN<SSWR::AVIRead::AVIRCore> core;

			NN<UI::GUIPanel> pnlRequest;
			NN<UI::GUILabel> lblServer;
			NN<UI::GUITextBox> txtServer;
			NN<UI::GUILabel> lblRequest;
			NN<UI::GUITextBox> txtRequest;
			NN<UI::GUIComboBox> cboRequest;
//			NN<UI::GUIRadioButton> radRequestNormal;
//			NN<UI::GUIRadioButton> radRequestMX;
			NN<UI::GUIButton> btnRequest;
			NN<UI::GUILabel> lblRequestTime;
			NN<UI::GUITextBox> txtRequestTime;
			NN<UI::GUIListBox> lbAnswer;
			NN<UI::GUIHSplitter> hspAnswer;
			NN<UI::GUIPanel> pnlAnswer;
			NN<UI::GUILabel> lblAnsName;
			NN<UI::GUITextBox> txtAnsName;
			NN<UI::GUILabel> lblAnsType;
			NN<UI::GUITextBox> txtAnsType;
			NN<UI::GUILabel> lblAnsClass;
			NN<UI::GUITextBox> txtAnsClass;
			NN<UI::GUILabel> lblAnsTTL;
			NN<UI::GUITextBox> txtAnsTTL;
			NN<UI::GUILabel> lblAnsRD;
			NN<UI::GUITextBox> txtAnsRD;

			Data::ArrayListNN<Net::DNSClient::RequestAnswer> ansList;
			NN<Net::SocketFactory> sockf;

			static void __stdcall OnRequestClicked(AnyType userObj);
			static void __stdcall OnAnswerSelChg(AnyType userObj);
		public:
			AVIRDNSClientForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRDNSClientForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
