#ifndef _SM_SSWR_AVIREAD_AVIRSSLINFOFORM
#define _SM_SSWR_AVIREAD_AVIRSSLINFOFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIComboBox.h"
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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			NotNullPtr<Net::SocketFactory> sockf;
			Net::SSLEngine *ssl;
			UI::GUILabel *lblHost;
			UI::GUITextBox *txtHost;
			UI::GUILabel *lblPort;
			UI::GUITextBox *txtPort;
			UI::GUILabel *lblMode;
			UI::GUIComboBox *cboMode;
			UI::GUILabel *lblVersion;
			UI::GUIComboBox *cboVersion;
			UI::GUIButton *btnCheck;

			UI::GUILabel *lblStatus;
			UI::GUITextBox *txtStatus;
			UI::GUILabel *lblCert;
			UI::GUITextBox *txtCert;
			UI::GUIButton *btnCert;
			UI::GUIButton *btnRAW;

			Crypto::Cert::X509File *currCerts;
			UInt8 *packetBuff;
			UOSInt packetSize;

		private:
			static void __stdcall OnCheckClicked(void *userObj);
			static void __stdcall OnCertClicked(void *userObj);
			static void __stdcall OnRAWClicked(void *userObj);
			
		public:
			AVIRSSLInfoForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRSSLInfoForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
