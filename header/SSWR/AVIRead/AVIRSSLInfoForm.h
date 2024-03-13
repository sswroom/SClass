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
			Optional<Net::SSLEngine> ssl;
			NotNullPtr<UI::GUILabel> lblHost;
			NotNullPtr<UI::GUITextBox> txtHost;
			NotNullPtr<UI::GUILabel> lblPort;
			NotNullPtr<UI::GUITextBox> txtPort;
			NotNullPtr<UI::GUILabel> lblMode;
			NotNullPtr<UI::GUIComboBox> cboMode;
			NotNullPtr<UI::GUILabel> lblVersion;
			NotNullPtr<UI::GUIComboBox> cboVersion;
			NotNullPtr<UI::GUIButton> btnCheck;

			NotNullPtr<UI::GUILabel> lblStatus;
			NotNullPtr<UI::GUITextBox> txtStatus;
			NotNullPtr<UI::GUILabel> lblCert;
			NotNullPtr<UI::GUITextBox> txtCert;
			NotNullPtr<UI::GUIButton> btnCert;
			NotNullPtr<UI::GUIButton> btnRAW;

			Crypto::Cert::X509File *currCerts;
			UInt8 *packetBuff;
			UOSInt packetSize;

		private:
			static void __stdcall OnCheckClicked(void *userObj);
			static void __stdcall OnCertClicked(void *userObj);
			static void __stdcall OnRAWClicked(void *userObj);
			
		public:
			AVIRSSLInfoForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRSSLInfoForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
