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
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Net::SocketFactory> sockf;
			Optional<Net::SSLEngine> ssl;
			NN<UI::GUILabel> lblHost;
			NN<UI::GUITextBox> txtHost;
			NN<UI::GUILabel> lblPort;
			NN<UI::GUITextBox> txtPort;
			NN<UI::GUILabel> lblMode;
			NN<UI::GUIComboBox> cboMode;
			NN<UI::GUILabel> lblVersion;
			NN<UI::GUIComboBox> cboVersion;
			NN<UI::GUIButton> btnCheck;

			NN<UI::GUILabel> lblStatus;
			NN<UI::GUITextBox> txtStatus;
			NN<UI::GUILabel> lblCert;
			NN<UI::GUITextBox> txtCert;
			NN<UI::GUIButton> btnCert;
			NN<UI::GUIButton> btnRAW;

			Optional<Crypto::Cert::X509File> currCerts;
			UnsafeArrayOpt<UInt8> packetBuff;
			UIntOS packetSize;

		private:
			static void __stdcall OnCheckClicked(AnyType userObj);
			static void __stdcall OnCertClicked(AnyType userObj);
			static void __stdcall OnRAWClicked(AnyType userObj);
			
		public:
			AVIRSSLInfoForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRSSLInfoForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
