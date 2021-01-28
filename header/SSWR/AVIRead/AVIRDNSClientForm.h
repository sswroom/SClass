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
			SSWR::AVIRead::AVIRCore *core;

			UI::GUIPanel *pnlRequest;
			UI::GUILabel *lblServer;
			UI::GUITextBox *txtServer;
			UI::GUILabel *lblRequest;
			UI::GUITextBox *txtRequest;
			UI::GUIComboBox *cboRequest;
//			UI::GUIRadioButton *radRequestNormal;
//			UI::GUIRadioButton *radRequestMX;
			UI::GUIButton *btnRequest;
			UI::GUILabel *lblRequestTime;
			UI::GUITextBox *txtRequestTime;
			UI::GUIListBox *lbAnswer;
			UI::GUIHSplitter *hspAnswer;
			UI::GUIPanel *pnlAnswer;
			UI::GUILabel *lblAnsName;
			UI::GUITextBox *txtAnsName;
			UI::GUILabel *lblAnsType;
			UI::GUITextBox *txtAnsType;
			UI::GUILabel *lblAnsClass;
			UI::GUITextBox *txtAnsClass;
			UI::GUILabel *lblAnsTTL;
			UI::GUITextBox *txtAnsTTL;
			UI::GUILabel *lblAnsRD;
			UI::GUITextBox *txtAnsRD;

			Data::ArrayList<Net::DNSClient::RequestAnswer*> *ansList;
			Net::SocketFactory *sockf;

			static void __stdcall OnRequestClicked(void *userObj);
			static void __stdcall OnAnswerSelChg(void *userObj);
		public:
			AVIRDNSClientForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRDNSClientForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
