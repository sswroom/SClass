#ifndef _SM_SSWR_AVIREAD_AVIRSSLINFOFORM
#define _SM_SSWR_AVIREAD_AVIRSSLINFOFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIGroupBox.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIPanel.h"
#include "UI/GUIRadioButton.h"
#include "UI/GUIRealtimeLineChart.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRSSLInfoForm : public UI::GUIForm
		{
		private:
			SSWR::AVIRead::AVIRCore *core;
			Net::SocketFactory *sockf;
			Net::SSLEngine *ssl;
			UI::GUILabel *lblHost;
			UI::GUITextBox *txtHost;
			UI::GUILabel *lblPort;
			UI::GUITextBox *txtPort;
			UI::GUIButton *btnCheck;
			UI::GUILabel *lblStatus;
			UI::GUITextBox *txtStatus;
			UI::GUILabel *lblCert;
			UI::GUITextBox *txtCert;
			UI::GUIButton *btnCert;

			Crypto::Cert::X509File *currCerts;

		private:
			static void __stdcall OnCheckClicked(void *userObj);
			static void __stdcall OnCertClicked(void *userObj);

		public:
			AVIRSSLInfoForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRSSLInfoForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
